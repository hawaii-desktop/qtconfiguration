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
    Q_DECLARE_PRIVATE(QConfiguration)
public:
    explicit QConfiguration(const QString &schemaFileName);
    ~QConfiguration();

    QString schemaName() const;

    QVariant value(const QString &key) const;
    void setValue(const QString &key, const QVariant &value);

signals:
    void changed();

private:
    Q_PRIVATE_SLOT(d_ptr, void _q_fileChanged(const QString &fileName))

    QConfigurationPrivate *const d_ptr;
};

QT_END_NAMESPACE

#endif // QCONFIGURATION_H
