/****************************************************************************
 * This file is part of QtConfiguration.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QtConfiguration/QConfiguration>

#include "configurationitem.h"

QT_BEGIN_NAMESPACE

ConfigurationItem::ConfigurationItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_conf(0)
{
}

ConfigurationItem::~ConfigurationItem()
{
    delete m_conf;
}

QString ConfigurationItem::schema() const
{
    if (m_conf)
        return m_conf->schemaName();
    return QString();
}

void ConfigurationItem::setSchema(const QString &schema)
{
    if (schema.isEmpty())
        return;

    delete m_conf;
    m_conf = new QConfiguration(schema);
    emit schemaChanged();
    connect(m_conf, SIGNAL(changed()), this, SIGNAL(valueChanged()));
}

QString ConfigurationItem::group() const
{
    return m_group;
}

void ConfigurationItem::setGroup(const QString &group)
{
    if (group.isEmpty())
        return;

    m_group = group;
    emit groupChanged();
}

QVariant ConfigurationItem::value(const QString &key)
{
    if (m_group.isEmpty())
        return QVariant();

    if (m_conf)
        return m_conf->value(m_group + QLatin1Char('/') + key);
    return QVariant();
}

void ConfigurationItem::setValue(const QString &key, const QVariant &value)
{
    if (m_group.isEmpty())
        return;

    if (m_conf)
        m_conf->setValue(m_group + QLatin1Char('/') + key, value);
}

QT_END_NAMESPACE
