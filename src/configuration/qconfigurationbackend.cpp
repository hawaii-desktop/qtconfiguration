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

#include "qconfigurationbackend.h"

QConfigurationBackend::QConfigurationBackend(QObject *parent)
    : QObject(parent)
{
}

void QConfigurationBackend::setCategory(const QString &category)
{
}

bool QConfigurationBackend::contains(const QString &key) const
{
    Q_UNUSED(key);
    return false;
}

QVariant QConfigurationBackend::value(const QString &key, const QVariant &defaultValue) const
{
    Q_UNUSED(key);
    return defaultValue;
}

void QConfigurationBackend::setValue(const QString &key, const QVariant &value)
{
    Q_UNUSED(key);
    Q_UNUSED(value);
}

#include "moc_qconfigurationbackend.cpp"
