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
#include <dconf-dbus-1/dconf-dbus-1.h>
}

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtDBus/QDBusConnection>

#include "qconfiguration.h"
#include "qdconfconfigurationbackend.h"
#include "qgvariantutils.h"

#define QDCONF_DEBUG 1

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

static void qdconf_notify(DConfDBusClient *client, const char *key, void *user_data)
{
    QDConfConfigurationBackend *self = (QDConfConfigurationBackend *)user_data;

    GVariant *variant = dconf_dbus_client_read(client, key);
    QVariant value = convertValue(variant);

    self->notify(QString::fromUtf8(key), value);
}

class QDConfConfigurationBackendPrivate
{
public:
    QDConfConfigurationBackendPrivate()
    {
        client = dconf_client_new();

        DBusConnection *sessionBus = (DBusConnection *)QDBusConnection::sessionBus().internalPointer();
        DBusConnection *systemBus = (DBusConnection *)QDBusConnection::systemBus().internalPointer();
        dbusClient = dconf_dbus_client_new(NULL, sessionBus, systemBus);
    }

    ~QDConfConfigurationBackendPrivate()
    {
        if (client)
            g_object_unref(client);
        if (dbusClient)
            dconf_dbus_client_unref(dbusClient);
    }

    DConfClient *client;
    DConfDBusClient *dbusClient;
    QString prefix;
    QString category;
};

QDConfConfigurationBackend::QDConfConfigurationBackend(QObject *parent)
    : QConfigurationBackend(parent)
    , d_ptr(new QDConfConfigurationBackendPrivate)
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

#if 0
    if (d->category != QString::fromUtf8(fullPath))
        dconf_dbus_client_unsubscribe(d->dbusClient, qdconf_notify, this);
#endif

    d->category = QString::fromUtf8(fullPath);

#ifdef QDCONF_DEBUG
    qDebug() << "QDConfConfigurationBackend: setCategory" << fullPath;
#endif

    dconf_dbus_client_subscribe(d->dbusClient, fullPath.constData(),
                                qdconf_notify, this);
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
