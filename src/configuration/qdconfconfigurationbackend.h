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

#ifndef QDCONFCONFIGURATIONBACKEND_H
#define QDCONFCONFIGURATIONBACKEND_H

#include <QtConfiguration/qconfigurationbackend.h>

QT_BEGIN_NAMESPACE

class QDConfConfigurationBackendPrivate;

class Q_CONFIGURATION_EXPORT QDConfConfigurationBackend : public QConfigurationBackend
{
    Q_OBJECT
public:
    explicit QDConfConfigurationBackend(QObject *parent = 0);
    ~QDConfConfigurationBackend();

    void setCategory(const QString &category);

    bool contains(const QString &key) const;

    QVariant value(const QString &key,
                   const QVariant &defaultValue = QVariant()) const;
    void setValue(const QString &key, const QVariant &value);

    void notify(const QString &name, const QVariant &value);

private:
    Q_DECLARE_PRIVATE(QDConfConfigurationBackend)
    QDConfConfigurationBackendPrivate *d_ptr;
};

QT_END_NAMESPACE

#endif // QDCONFCONFIGURATIONBACKEND_H
