/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 Pier Luigi Fiorini.
** Contact: http://www.qt-project.org/legal
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

#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtCore/QMetaProperty>
#include <QtCore/QTimerEvent>

#include "qconfiguration.h"
#include "qconfiguration_p.h"
#include "qdconfconfigurationbackend.h"

QT_BEGIN_NAMESPACE

//#define QCONFIGURATION_DEBUG 1

static int configurationWriteDelay = 500;

/*
 * QConfigurationPrivate
 */

QConfigurationPrivate::QConfigurationPrivate()
    : q_ptr(0)
    , target(0)
    , timerId(0)
    , initialized(false)
    , backend(0)
{
}

QConfigurationBackend *QConfigurationPrivate::instance() const
{
    if (!backend) {
        QConfiguration *q = const_cast<QConfiguration *>(q_func());
        backend = new QDConfConfigurationBackend(q);

        if (!category.isEmpty())
            backend->setCategory(category);

        if (initialized)
            q->d_func()->load();
    }

    return backend;
}

void QConfigurationPrivate::init()
{
    if (!initialized) {
        load();
        initialized = true;
    }
}

void QConfigurationPrivate::reset()
{
    if (initialized && backend && !changedProperties.isEmpty())
        store();
    delete backend;
    backend = 0;
}

void QConfigurationPrivate::load()
{
    Q_Q(QConfiguration);

    const QMetaObject *qmo = q->metaObject();
    const QMetaObject *mo = target->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();

    for (int i = offset; i < count; ++i) {
        QMetaProperty property = mo->property(i);

        const QString keyName = QLatin1String(property.name());
        const QVariant previousValue = property.read(target);
        const QVariant currentValue = instance()->value(keyName, previousValue);

        if (!currentValue.isNull() &&
                currentValue.canConvert(previousValue.type()) &&
                previousValue != currentValue) {
            property.write(target, currentValue);
#ifdef QCONFIGURATION_DEBUG
            qDebug() << "QConfiguration: load" << property.name() << "setting:" << currentValue
                     << "default:" << previousValue;
#endif
        }

        // ensure that a non-existent setting gets written
        // even if the property wouldn't change later
        if (!instance()->contains(keyName))
            _q_propertyChanged();

        // setup change notifications on first load
        if (!initialized && property.hasNotifySignal()) {
            static const int propertyChangedIndex = qmo->indexOfSlot("_q_propertyChanged()");
            QMetaObject::connect(target, property.notifySignalIndex(), q, propertyChangedIndex);
        }
    }
}

void QConfigurationPrivate::store()
{
    QHash<const char *, QVariant>::iterator it = changedProperties.begin();
    while (it != changedProperties.end()) {
        const QString keyName = QLatin1String(it.key());
        instance()->setValue(keyName, it.value());
#ifdef QCONFIGURATION_DEBUG
        qDebug() << "QConfiguration: store" << it.key() << ":" << it.value();
#endif
        it = changedProperties.erase(it);
    }
}

void QConfigurationPrivate::_q_propertyChanged()
{
    Q_Q(QConfiguration);

    const QMetaObject *mo = target->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();

    for (int i = offset; i < count; ++i) {
        const QMetaProperty &property = mo->property(i);
        changedProperties.insert(property.name(), property.read(target));
#ifdef QCONFIGURATION_DEBUG
        qDebug() << "QConfiguration: cache" << property.name() << ":" << property.read(target);
#endif
    }

    if (timerId != 0)
        q->killTimer(timerId);
    timerId = q->startTimer(configurationWriteDelay);
}

/*
 * QConfiguration
 */

QConfiguration::QConfiguration(QObject *target, const QString &category, QObject *parent)
    : QObject(parent)
    , d_ptr(new QConfigurationPrivate)
{
    Q_D(QConfiguration);

    d->q_ptr = this;
    d->target = target;
    d->category = category;
    d->init();
}

QConfiguration::QConfiguration(QObject *target, QObject *parent)
    : QObject(parent)
    , d_ptr(new QConfigurationPrivate)
{
    Q_D(QConfiguration);

    d->q_ptr = this;
    d->target = target;
    d->init();
}

QConfiguration::~QConfiguration()
{
    Q_D(QConfiguration);
    d->reset();
}

QObject *QConfiguration::target() const
{
    Q_D(const QConfiguration);
    return d->target;
}

QString QConfiguration::category() const
{
    Q_D(const QConfiguration);
    return d->category;
}

void QConfiguration::setCategory(const QString &category)
{
    Q_D(QConfiguration);
    if (d->category != category) {
        d->reset();
        d->category = category;

        if (d->initialized)
            d->load();
    }
}

void QConfiguration::timerEvent(QTimerEvent *event)
{
    Q_D(QConfiguration);

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

#include "moc_qconfiguration.cpp"
