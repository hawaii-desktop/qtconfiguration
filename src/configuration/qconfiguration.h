/****************************************************************************
 * This file is part of QtConfiguration.
 *
 * Copyright (c) 2012-2013 Pier Luigi Fiorini
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

#ifndef QCONFIGURATION_H
#define QCONFIGURATION_H

#include <QtCore/QObject>

#include <QtConfiguration/qconfigurationglobal.h>

QT_BEGIN_NAMESPACE

class QConfigurationPrivate;

class Q_CONFIGURATION_EXPORT QConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString category READ category WRITE setCategory FINAL)
public:
    explicit QConfiguration(QObject *target, QObject *parent = 0);
    ~QConfiguration();

    QObject *target() const;

    QString category() const;
    void setCategory(const QString &category);

protected:
    void timerEvent(QTimerEvent *event);

private:
    Q_DISABLE_COPY(QConfiguration)
    Q_DECLARE_PRIVATE(QConfiguration)
    QScopedPointer<QConfigurationPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_propertyChanged())
};

QT_END_NAMESPACE

#endif // QCONFIGURATION_H
