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

#include <QtCore/QDebug>
#include <QtCore/QPointer>
#include <QtCore/QHash>
#include <QtCore/QMetaProperty>
#include <QtCore/QTimerEvent>

#include "qconfiguration.h"
#include "qconfiguration_p.h"
#include "qdconfconfigurationbackend.h"

QT_BEGIN_NAMESPACE

#define QCONFIGURATION_DEBUG 1

static int configurationWriteDelay = 500;

/*
 * QConfigurationPrivate
 */

QConfigurationPrivate::QConfigurationPrivate()
    : q_ptr(0)
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
}

void QConfigurationPrivate::load()
{
    Q_Q(QConfiguration);

    qDebug() << "**************" << q->staticMetaObject.propertyCount() << q->staticMetaObject.property(0).name();
    const QMetaObject *mo = q->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();
    qDebug() << "***" << mo << mo->className() << offset << count;

    for (int i = offset; i < count; ++i) {
        QMetaProperty property = mo->property(i);
        qDebug() << "===" << i << property.name();

        const QString keyName = QLatin1String(property.name());
        const QVariant previousValue = property.read(q);
        const QVariant currentValue = instance()->value(keyName, previousValue);

        if (!currentValue.isNull() &&
                currentValue.canConvert(previousValue.type()) &&
                previousValue != currentValue) {
            property.write(q, currentValue);
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
            static const int propertyChangedIndex = mo->indexOfSlot("_q_propertyChanged()");
            QMetaObject::connect(q, property.notifySignalIndex(), q, propertyChangedIndex);
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

    const QMetaObject *mo = q->metaObject();
    const int offset = mo->propertyOffset();
    const int count = mo->propertyCount();

    for (int i = offset; i < count; ++i) {
        const QMetaProperty &property = mo->property(i);
        changedProperties.insert(property.name(), property.read(q));
#ifdef QCONFIGURATION_DEBUG
        qDebug() << "QConfiguration: cache" << property.name() << ":" << property.read(q);
#endif
    }

    if (timerId != 0)
        q->killTimer(timerId);
    timerId = q->startTimer(configurationWriteDelay);
}

/*
 * QConfiguration
 */

QConfiguration::QConfiguration(QObject *parent)
    : QObject(parent)
    , d_ptr(new QConfigurationPrivate)
{
    Q_D(QConfiguration);

    d->q_ptr = this;
    d->init();
}

QConfiguration::~QConfiguration()
{
    Q_D(QConfiguration);
    d->reset();
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
