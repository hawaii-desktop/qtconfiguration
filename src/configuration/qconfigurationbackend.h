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

#ifndef QCONFIGURATIONBACKEND_H
#define QCONFIGURATIONBACKEND_H

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include <QtConfiguration/qconfigurationglobal.h>

QT_BEGIN_NAMESPACE

class Q_CONFIGURATION_EXPORT QConfigurationBackend : public QObject
{
    Q_OBJECT
public:
    explicit QConfigurationBackend(QObject *parent = 0);

    virtual void setCategory(const QString &category);

    virtual bool contains(const QString &key) const;

    virtual QVariant value(const QString &key,
                           const QVariant &defaultValue = QVariant()) const;
    virtual void setValue(const QString &key, const QVariant &value);
};

QT_END_NAMESPACE

#endif // QCONFIGURATIONBACKEND_H
