/****************************************************************************
**
** Copyright (C) 2013-2014 Pier Luigi Fiorini.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
#include "qconfigurationutils.h"
#include "qstaticconfiguration.h"

//#define QDCONF_DEBUG 1

class QDConfConfigurationBackendPrivate
{
    Q_DECLARE_PUBLIC(QDConfConfigurationBackend)
public:
    QDConfConfigurationBackendPrivate(QDConfConfigurationBackend *parent)
        : q_ptr(parent)
    {
        client = dconf_client_new();
        g_object_ref(client);

        g_signal_connect(G_OBJECT(client), "changed",
                         G_CALLBACK(&changeCallback), (gpointer)this);
    }

    ~QDConfConfigurationBackendPrivate()
    {
        if (!category.isEmpty())
            dconf_client_unwatch_fast(client, qPrintable(category));
        if (client) {
            g_signal_handlers_disconnect_by_data(G_OBJECT(client), (gpointer)this);
            g_object_unref(client);
        }
    }

    static void changeCallback(DConfClient *, char *prefix_, char **changes, char *, gpointer userData)
    {
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
    QString confCategory;

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

QString QDConfConfigurationBackend::category() const
{
    Q_D(const QDConfConfigurationBackend);
    return d->confCategory;
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
        d->confCategory = category;

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
        return;
    }

    QStaticConfiguration *staticConf = qobject_cast<QStaticConfiguration *>(parent());
    if (staticConf) {
#ifdef QDCONF_DEBUG
        qDebug() << "QDConfConfigurationBackend: notify" << name << "value:" << value;
#endif
        QMetaObject::invokeMethod(staticConf, "valueChanged",
                                  Qt::QueuedConnection,
                                  Q_ARG(const QString, name),
                                  Q_ARG(const QVariant, value));
    }
}

#include "moc_qdconfconfigurationbackend.cpp"
