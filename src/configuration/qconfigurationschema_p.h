/****************************************************************************
 * This file is part of QtConfiguration.
 *
 * Copyright (C) 2013 Pier Luigi Fiorini
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

#ifndef QCONFIGURATIONSCHEMA_P_H
#define QCONFIGURATIONSCHEMA_P_H

#include <QtCore/QList>
#include <QtCore/QPoint>
#include <QtCore/QRect>
#include <QtCore/QSize>
#include <QtCore/QUrl>
#include <QtCore/QVariant>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtConfiguration API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

QT_BEGIN_NAMESPACE

class QConfigurationKey
{
public:
    QString group;
    QString type;
    QString name;
    QString summary;
    QString description;
    QVariant defaultValue;
    int minValue;
    int maxValue;
    QVariantList choices;

    QConfigurationKey() {
    }
};

class QConfigurationSchema
{
public:
    QConfigurationSchema(const QString &schema);

    bool process(const QString &fileName);

    QConfigurationKey *lookupKey(const QString &keyName);

private:
    QString m_schema;
    QStringList m_allNames;
    QList<QConfigurationKey *> m_entries;
};

QT_END_NAMESPACE

#endif // QCONFIGURATIONSCHEMA_P_H
