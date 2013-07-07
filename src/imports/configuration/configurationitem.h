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

#ifndef CONFIGURATIONITEM_H
#define CONFIGURATIONITEM_H

#include <QQuickItem>

QT_BEGIN_NAMESPACE

class QConfiguration;

class ConfigurationItem : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QString schema READ schema WRITE setSchema NOTIFY schemaChanged)
    Q_PROPERTY(QString group READ group WRITE setGroup NOTIFY groupChanged)
public:
    ConfigurationItem(QQuickItem *parent = 0);
    ~ConfigurationItem();

    QString schema() const;
    void setSchema(const QString &schema);

    QString group() const;
    void setGroup(const QString &group);

    Q_INVOKABLE QVariant value(const QString &key);
    Q_INVOKABLE void setValue(const QString &key, const QVariant &value);

signals:
    void schemaChanged();
    void groupChanged();
    void valueChanged();

private:
    QConfiguration *m_conf;
    QString m_group;
};

QT_END_NAMESPACE

#endif // CONFIGURATIONITEM_H
