/****************************************************************************
 * This file is part of QtConfiguration.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:LGPL2$
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; version 2.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $END_LICENSE$
 ***************************************************************************/

extern "C" {
#include <dconf/client/dconf-client.h>
#include <dconf/common/dconf-paths.h>
}

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>

#include "qconfiguration.h"
#include "qdconfconfigurationbackend.h"
#include "qgvariantutils.h"

//#define QDCONF_DEBUG 1

static QString comify(const QString &organization)
{
    for (int i = organization.size() - 1; i >= 0; --i) {
        QChar ch = organization.at(i);
        if (ch == QLatin1Char('.') || ch == QChar(0x3002) || ch == QChar(0xff0e)
                || ch == QChar(0xff61)) {
            QString suffix = organization.mid(i + 1).toLower();
            if (suffix.size() == 2 || suffix == QLatin1String("com")
                    || suffix == QLatin1String("org") || suffix == QLatin1String("net")
                    || suffix == QLatin1String("edu") || suffix == QLatin1String("gov")
                    || suffix == QLatin1String("mil") || suffix == QLatin1String("biz")
                    || suffix == QLatin1String("info") || suffix == QLatin1String("name")
                    || suffix == QLatin1String("pro") || suffix == QLatin1String("aero")
                    || suffix == QLatin1String("coop") || suffix == QLatin1String("museum")) {
                QString result = organization;
                result.replace(QLatin1Char('/'), QLatin1Char(' '));
                return result;
            }
            break;
        }
        int uc = ch.unicode();
        if ((uc < 'a' || uc > 'z') && (uc < 'A' || uc > 'Z'))
            break;
    }

    QString domain;
    for (int i = 0; i < organization.size(); ++i) {
        QChar ch = organization.at(i);
        int uc = ch.unicode();
        if ((uc >= 'a' && uc <= 'z') || (uc >= '0' && uc <= '9')) {
            domain += ch;
        } else if (uc >= 'A' && uc <= 'Z') {
            domain += ch.toLower();
        } else {
            domain += QLatin1Char(' ');
        }
    }
    domain = domain.simplified();
    domain.replace(QLatin1Char(' '), QLatin1Char('-'));
    if (!domain.isEmpty())
        domain.append(QLatin1String(".com"));
    return domain;
}

class QDConfConfigurationBackendPrivate
{
    Q_DECLARE_PUBLIC(QDConfConfigurationBackend)
public:
    QDConfConfigurationBackendPrivate(QDConfConfigurationBackend *parent)
        : q_ptr(parent)
    {
        client = dconf_client_new();
        g_object_ref(client);

        g_signal_connect(G_OBJECT(client), "changed", G_CALLBACK(changeCallback), this);
    }

    ~QDConfConfigurationBackendPrivate()
    {
        if (!category.isEmpty())
            dconf_client_unwatch_fast(client, qPrintable(category));
        if (client)
            g_object_unref(client);
    }

    static void changeCallback(DConfClient *, char *prefix_, char **changes, char *tag, void *userData)
    {
        Q_UNUSED(tag);

        QDConfConfigurationBackendPrivate *self =
                static_cast<QDConfConfigurationBackendPrivate *>(userData);
        if (!self || !prefix_ || !changes)
            return;

        if (strcmp(changes[0], "") == 0) {
            if (dconf_is_key(prefix_, NULL)) {
                // If a single key has changed then prefix will be equal
                // to the key and changes will contain a single empty item
                QString key = QString::fromUtf8(prefix_).replace(self->category, QStringLiteral(""));
                GVariant *gvalue = dconf_client_read(self->client, prefix_);
                if (gvalue) {
                    QVariant value = convertValue(gvalue);
                    self->q_ptr->notify(key, value);
                }
            } else if (dconf_is_dir(prefix_, NULL)) {
                // If any key under a dir has changed then prefix will be equal to
                // the dir and changes will contain a single empty item
                gint length = 0;
                gchar **array = dconf_client_list(self->client, prefix_, &length);
                for (int i = 0; i < length; ++i) {
                    QString key = QString::fromUtf8(prefix_) + QString::fromUtf8(array[i]);
                    GVariant *gvalue = dconf_client_read(self->client, qPrintable(key));
                    if (gvalue) {
                        QVariant value = convertValue(gvalue);
                        self->q_ptr->notify(key, value);
                    }
                }
                g_strfreev(array);
            }
        } else {
            // If more than one change is being reported then changes will have more than one item
            while (changes) {
                QString tmpKey = QString::fromUtf8(prefix_) + QString::fromUtf8(changes[0]);
                QString key = tmpKey.replace(self->category, QStringLiteral(""));

                GVariant *gvalue = dconf_client_read(self->client, qPrintable(key));
                if (gvalue) {
                    QVariant value = convertValue(gvalue);
                    self->q_ptr->notify(key, value);
                }

                ++changes;
            }
        }
    }

    DConfClient *client;
    QString prefix;
    QString category;

protected:
    QDConfConfigurationBackend *q_ptr;
};

QDConfConfigurationBackend::QDConfConfigurationBackend(QObject *parent)
    : QConfigurationBackend(parent)
    , d_ptr(new QDConfConfigurationBackendPrivate(this))
{
    Q_D(QDConfConfigurationBackend);

    QString organization = QCoreApplication::organizationDomain().isEmpty()
            ? QCoreApplication::organizationName()
            : QCoreApplication::organizationDomain();
    QString domainName = comify(organization);
    if (domainName.isEmpty())
        // If domain name is empty use a hard-coded string
        domainName = QLatin1String("qt-project.org");

    // Convert domain name into java package name
    QString javaPackageName;
    int curPos = 0, nextDot;
    while ((nextDot = domainName.indexOf(QLatin1Char('.'), curPos)) != -1) {
        javaPackageName.prepend(domainName.mid(curPos, nextDot - curPos));
        javaPackageName.prepend(QLatin1Char('.'));
        curPos = nextDot + 1;
    }
    javaPackageName.prepend(domainName.mid(curPos));
    javaPackageName = javaPackageName.toLower();
    if (curPos == 0)
        javaPackageName.prepend(QLatin1String("com."));

    // Turn java package name into a path
    d->prefix = QLatin1Char('/') + javaPackageName.replace(QLatin1Char('.'), QLatin1Char('/'));

    // Aappend the application name if makes sense
    QString appName = QString::null;
    if (organization.isEmpty()) {
        appName = QCoreApplication::applicationName();
        if (appName.isEmpty()) {
            QFileInfo fileInfo(QCoreApplication::applicationFilePath());
            appName = fileInfo.baseName();
        } else {
            appName = appName.toLower();
            appName = appName.replace(QLatin1Char(' '), QLatin1Char('-'));
        }
    }
    setCategory(appName);
}

QDConfConfigurationBackend::~QDConfConfigurationBackend()
{
    delete d_ptr;
}

void QDConfConfigurationBackend::setCategory(const QString &category)
{
    Q_D(QDConfConfigurationBackend);

    QByteArray fullPath = d->prefix.toUtf8() + QByteArrayLiteral("/");
    if (!category.isEmpty())
        fullPath += category.toUtf8() + QByteArrayLiteral("/");

    if (d->category != QString::fromUtf8(fullPath)) {
        // Unsubscribe the old path if it was really changed
        dconf_client_unwatch_fast(d->client, fullPath.constData());

        // Watch the new path
        dconf_client_watch_fast(d->client, fullPath.constData());

        // Save the new category
        d->category = QString::fromUtf8(fullPath);

#ifdef QDCONF_DEBUG
        qDebug() << "QDConfConfigurationBackend: setCategory" << fullPath;
#endif
    }
}

bool QDConfConfigurationBackend::contains(const QString &key) const
{
    Q_D(const QDConfConfigurationBackend);

    QByteArray path = d->prefix.toUtf8() + QByteArrayLiteral("/") + key.toUtf8();
    gint length = 0;
    gchar **array = dconf_client_list(d->client, path.constData(), &length);

    for (gchar **item = array; *item; ++item) {
        qDebug() << "***********" << *item;
    }

    return false;
}

QVariant QDConfConfigurationBackend::value(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const QDConfConfigurationBackend);

    QByteArray path = d->category.toUtf8() + key.toUtf8();
#ifdef QDCONF_DEBUG
    qDebug() << "QDConfConfigurationBackend: value" << path << "default:" << defaultValue;
#endif
    GVariant *gvalue = dconf_client_read(d->client, path.constData());
    if (gvalue)
        return convertValue(gvalue);

    return defaultValue;
}

void QDConfConfigurationBackend::setValue(const QString &key, const QVariant &value)
{
    Q_D(QDConfConfigurationBackend);

    GError *error = 0;

    QByteArray path = d->category.toUtf8() + key.toUtf8();
#ifdef QDCONF_DEBUG
    qDebug() << "QDConfConfigurationBackend: setValue" << path << "value:" << value;
#endif
    GVariant *gvalue = convertVariant(value);
    if (gvalue)
        dconf_client_write_fast(d->client, path.constData(), gvalue, &error);

    if (error) {
        qWarning() << "An error occurred writing the value for" << key
                   << "to dconf: " << error->message;
        g_error_free(error);
    }
}

void QDConfConfigurationBackend::notify(const QString &name, const QVariant &value)
{
    Q_D(QDConfConfigurationBackend);

    QConfiguration *configuration = qobject_cast<QConfiguration *>(parent());
    if (configuration) {
#ifdef QDCONF_DEBUG
        qDebug() << "QDConfConfigurationBackend: notify" << name << "value:" << value;
#endif
        QString propertyName = name;
        propertyName = propertyName.replace(d->category, QLatin1String(""));
        configuration->target()->setProperty(propertyName.toUtf8().constData(), value);
    }
}

#include "moc_qdconfconfigurationbackend.cpp"
