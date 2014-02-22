/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
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

#include "qqmlconfiguration_p.h"
#include <qcoreevent.h>
#include <qpointer.h>
#include <qdebug.h>
#include <qhash.h>

#include "qstaticconfiguration.h"

QT_BEGIN_NAMESPACE

/*!
    \qmlmodule Hawaii.Configuration 1.0
    \title Qt Labs Settings QML Types
    \ingroup qmlmodules
    \brief Provides persistent platform-independent application settings.

    To use this module, import the module with the following line:

    \code
    import Hawaii.Configuration 1.0
    \endcode
*/

/*!
    \qmltype Settings
    \instantiates QQmlConfiguration
    \inqmlmodule Hawaii.Configuration 1.0
    \ingroup settings
    \brief Provides persistent platform-independent application settings.

    The Settings type provides persistent platform-independent application settings.

    \note This type is made available by importing the \b Hawaii.Configuration module.
    \e {Types in the Qt.labs module are not guaranteed to remain compatible
    in future versions.}

    Users normally expect an application to remember its settings (window sizes
    and positions, options, etc.) across sessions. The Settings type enables you
    to save and restore such application settings with the minimum of effort.

    Individual setting values are specified by declaring properties within a
    Settings element. All \l {QML Basic Types}{basic type} properties are
    supported. The recommended approach is to use property aliases in order
    to get automatic property updates both ways. The following example shows
    how to use Settings to store and restore the geometry of a window.

    \qml
    import QtQuick.Window 2.1
    import Hawaii.Configuration 1.0

    Window {
        id: window

        width: 800
        height: 600

        Settings {
            property alias x: window.x
            property alias y: window.y
            property alias width: window.width
            property alias height: window.height
        }
    }
    \endqml

    At first application startup, the window gets default dimensions specified
    as 800x600. Notice that no default position is specified - we let the window
    manager handle that. Later when the window geometry changes, new values will
    be automatically stored to the persistent settings. The second application
    run will get initial values from the persistent settings, bringing the window
    back to the previous position and size.

    A fully declarative syntax, achieved by using property aliases, comes at the
    cost of storing persistent settings whenever the values of aliased properties
    change. Normal properties can be used to gain more fine-grained control over
    storing the persistent settings. The following example illustrates how to save
    a setting on component destruction.

    \qml
    import QtQuick 2.1
    import Hawaii.Configuration 1.0

    Item {
        id: page

        state: settings.state

        states: [
            State {
                name: "active"
                // ...
            },
            State {
                name: "inactive"
                // ...
            }
        ]

        Settings {
            id: settings
            property string state: "active"
        }

        Component.onDestruction: {
            settings.state = page.state
        }
    }
    \endqml

    Notice how the default value is now specified in the persistent setting property,
    and the actual property is bound to the setting in order to get the initial value
    from the persistent settings.

    \section1 Application Identifiers

    Application specific settings are identified by providing application
    \l {QCoreApplication::applicationName}{name},
    \l {QCoreApplication::organizationName}{organization} and
    \l {QCoreApplication::organizationDomain}{domain}.

    \code
    #include <QGuiApplication>
    #include <QQmlApplicationEngine>

    int main(int argc, char *argv[])
    {
        QGuiApplication app(argc, argv);
        app.setOrganizationName("Some Company");
        app.setOrganizationDomain("somecompany.com");
        app.setApplicationName("Amazing Application");

        QQmlApplicationEngine engine("main.qml");
        return app.exec();
    }
    \endcode

    These are typically specified in C++ in the beginning of \c main(),
    but can also be controlled in QML via the following properties:
    \list
        \li \l {Qt::application}{Qt.application.name},
        \li \l {Qt::application}{Qt.application.organization} and
        \li \l {Qt::application}{Qt.application.domain}.
    \endlist

    \section1 Categories

    Application settings may be divided into logical categories by specifying
    a category name via the \l category property. Using logical categories not
    only provides a cleaner settings structure, but also prevents possible
    conflicts between setting keys.

    \qml
    Item {
        id: panel

        visible: true

        Settings {
            category: "OutputPanel"
            property alias visible: panel.visible
            // ...
        }
    }
    \endqml

    Instead of ensuring that all settings in the application have unique names,
    the settings can be divided into unique categories that may then contain
    settings using the same names that are used in other categories - without
    a conflict.

    \section1 Notes

    The current implementation is based on \l QStaticConfiguration. This imposes certain
    limitations, such as missing change notifications. Writing a setting value
    using one instance of Settings does not update the value in another Settings
    instance, even if they are referring to the same setting in the same category.

    The information is stored in the system registry on Windows, and in XML
    preferences files on OS X. On other Unix systems, in the absence of a
    standard, INI text files are used. See \l QStaticConfiguration documentation for
    more details.

    \sa QStaticConfiguration
*/

// #define SETTINGS_DEBUG

static const int settingsWriteDelay = 500;

class QQmlConfigurationPrivate
{
    Q_DECLARE_PUBLIC(QQmlConfiguration)

public:
    QQmlConfigurationPrivate();

    QStaticConfiguration *instance() const;

    void init();
    void reset();

    void load();
    void store();

    void _q_propertyChanged();

    QQmlConfiguration *q_ptr;
    int timerId;
    bool initialized;
    QString category;
    mutable QPointer<QStaticConfiguration> settings;
    QHash<const char *, QVariant> changedProperties;
};

QQmlConfigurationPrivate::QQmlConfigurationPrivate()
    : q_ptr(0), timerId(0), initialized(false)
{
}

QStaticConfiguration *QQmlConfigurationPrivate::instance() const
{
    if (!settings) {
        QQmlConfiguration *q = const_cast<QQmlConfiguration*>(q_func());
        settings = new QStaticConfiguration(q);
        if (!category.isEmpty())
            settings->setCategory(category);
        if (initialized)
            q->d_func()->load();
    }
    return settings;
}

void QQmlConfigurationPrivate::init()
{
    if (!initialized) {
        load();
        initialized = true;
    }
}

void QQmlConfigurationPrivate::reset()
{
    if (initialized && settings && !changedProperties.isEmpty())
        store();
    delete settings;
}

void QQmlConfigurationPrivate::load()
{
    Q_Q(QQmlConfiguration);
    const QMetaObject *mo = q->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();
    for (int i = offset; i < count; ++i) {
        QMetaProperty property = mo->property(i);

        const QVariant previousValue = property.read(q);
        const QVariant currentValue = instance()->value(property.name(), previousValue);

        if (!currentValue.isNull() && (!previousValue.isValid()
                || (currentValue.canConvert(previousValue.type()) && previousValue != currentValue))) {
            property.write(q, currentValue);
#ifdef SETTINGS_DEBUG
            qDebug() << "QQmlConfiguration: load" << property.name() << "setting:" << currentValue << "default:" << previousValue;
#endif
        }

        // ensure that a non-existent setting gets written
        // even if the property wouldn't change later
        if (!instance()->contains(property.name()))
            _q_propertyChanged();

        // setup change notifications on first load
        if (!initialized && property.hasNotifySignal()) {
            static const int propertyChangedIndex = mo->indexOfSlot("_q_propertyChanged()");
            QMetaObject::connect(q, property.notifySignalIndex(), q, propertyChangedIndex);
        }
    }
}

void QQmlConfigurationPrivate::store()
{
    QHash<const char *, QVariant>::iterator it = changedProperties.begin();
    while (it != changedProperties.end()) {
        instance()->setValue(it.key(), it.value());
#ifdef SETTINGS_DEBUG
        qDebug() << "QQmlConfiguration: store" << it.key() << ":" << it.value();
#endif
        it = changedProperties.erase(it);
    }
}

void QQmlConfigurationPrivate::_q_propertyChanged()
{
    Q_Q(QQmlConfiguration);
    const QMetaObject *mo = q->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();
    for (int i = offset; i < count; ++i) {
        const QMetaProperty &property = mo->property(i);
        changedProperties.insert(property.name(), property.read(q));
#ifdef SETTINGS_DEBUG
        qDebug() << "QQmlConfiguration: cache" << property.name() << ":" << property.read(q);
#endif
    }
    if (timerId != 0)
        q->killTimer(timerId);
    timerId = q->startTimer(settingsWriteDelay);
}

QQmlConfiguration::QQmlConfiguration(QObject *parent)
    : QObject(parent), d_ptr(new QQmlConfigurationPrivate)
{
    Q_D(QQmlConfiguration);
    d->q_ptr = this;
}

QQmlConfiguration::~QQmlConfiguration()
{
    Q_D(QQmlConfiguration);
    d->reset(); // flush pending changes
}

/*!
    \qmlproperty string Settings::category

    This property holds the name of the settings category.

    Categories can be used to group related settings together.
*/
QString QQmlConfiguration::category() const
{
    Q_D(const QQmlConfiguration);
    return d->category;
}

void QQmlConfiguration::setCategory(const QString &category)
{
    Q_D(QQmlConfiguration);
    if (d->category != category) {
        d->reset();
        d->category = category;
        if (d->initialized)
            d->load();
    }
}

void QQmlConfiguration::classBegin()
{
}

void QQmlConfiguration::componentComplete()
{
    Q_D(QQmlConfiguration);
    d->init();
}

void QQmlConfiguration::timerEvent(QTimerEvent *event)
{
    Q_D(QQmlConfiguration);
    if (event->timerId() == d->timerId) {
        if (d->changedProperties.isEmpty()) {
            killTimer(d->timerId);
            d->timerId = 0;
        } else {
            d->store();
        }
    }
    QObject::timerEvent(event);
}

QT_END_NAMESPACE

#include "moc_qqmlconfiguration_p.cpp"
