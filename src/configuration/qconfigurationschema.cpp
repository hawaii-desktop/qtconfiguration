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

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>
#include <QtCore/QRegularExpression>
#include <QtCore/QStandardPaths>

#include "qconfigurationschema_p.h"

QConfigurationSchema::QConfigurationSchema(const QString &schema)
    : m_schema(schema)
{
    QString fileName = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                              QLatin1String("hawaii/settings/") + schema + QLatin1String(".json"));

    if (fileName.isEmpty())
        qWarning("Couldn't find \"%s\" configuration schema!", schema.toUtf8().constData());
    else
        process(fileName);
}

bool QConfigurationSchema::process(const QString &fileName)
{
    QFile schemaFile(fileName);
    if (!schemaFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open settings schema file" << fileName;
        return false;
    }

    // Open the document
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(schemaFile.readAll(), &error);

    // Close the file
    schemaFile.close();

    // Handle parsing errors
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Error parsing settings schema file" << fileName
                   << "at character" << error.offset << ":" << error.errorString();
        return false;
    }

    // Can't continue with an empty document
    if (doc.isEmpty()) {
        qWarning() << "Settings schema file" << fileName << "is empty";
        return false;
    }

    // Check if it's the right schema
    QString schema = doc.object().value(QLatin1String("schema")).toString(m_schema);
    if (schema != m_schema) {
        qWarning() << "Expected schema" << m_schema << "not found," << schema << "is specified instead";
        return false;
    }

    QJsonArray groups = doc.object().value(QLatin1String("groups")).toArray();
    if (groups.isEmpty()) {
        qWarning() << "The" << m_schema << "settings schema has an empty \"groups\" entry";
        return false;
    }

    for (int i = 0; i < groups.size(); i++) {
        QJsonObject object = groups.at(i).toObject();

        QString group = object.value(QLatin1String("name")).toString();
        if (group.isEmpty())
            continue;

        QJsonObject entries = object.value(QLatin1String("keys")).toObject();
        foreach(QString key, entries.keys()) {
            QJsonObject entry = entries.value(key).toObject();

            QConfigurationKey *settingsKey = new QConfigurationKey();
            settingsKey->group = group;
            settingsKey->name = key;
            settingsKey->type = entry.value(QLatin1String("type")).toString();
            settingsKey->summary = entry.value(QLatin1String("summary")).toString();
            settingsKey->description = entry.value(QLatin1String("description")).toString();

            if (settingsKey->type == QLatin1String("enum"))
                settingsKey->choices = entry.value(QLatin1String("choices")).toArray().toVariantList();
            else if (settingsKey->type.contains(QRegularExpression(QLatin1String("(int|uint)")))) {
                settingsKey->minValue = entry.value(QLatin1String("minValue")).toVariant().toInt();
                settingsKey->maxValue = entry.value(QLatin1String("maxValue")).toVariant().toInt();
            }

            QVariant defaultValue = entry.value(QLatin1String("default")).toVariant();
            if (settingsKey->type == QLatin1String("int"))
                settingsKey->defaultValue = QVariant(defaultValue.toInt());
            else if (settingsKey->type == QLatin1String("uint"))
                settingsKey->defaultValue = QVariant(defaultValue.toUInt());
            else if (settingsKey->type == QLatin1String("float"))
                settingsKey->defaultValue = QVariant(defaultValue.toFloat());
            else if (settingsKey->type == QLatin1String("double"))
                settingsKey->defaultValue = QVariant(defaultValue.toDouble());
            else if (settingsKey->type == QLatin1String("rect")) {
                QList<QVariant> list = defaultValue.toList();
                if (list.size() == 4)
                    settingsKey->defaultValue = QVariant(QRect(list.at(0).toInt(), list.at(1).toInt(),
                                                               list.at(2).toInt(), list.at(3).toInt()));
            } else if (settingsKey->type == QLatin1String("size")) {
                QList<QVariant> list = defaultValue.toList();
                if (list.size() == 2)
                    settingsKey->defaultValue = QVariant(QSize(list.at(0).toInt(), list.at(1).toInt()));
            } else if (settingsKey->type == QLatin1String("url"))
                settingsKey->defaultValue = QVariant(QUrl::fromUserInput(defaultValue.toString()));
            else if (settingsKey->type == QLatin1String("urls")) {
                QStringList list = defaultValue.toStringList();
                QList<QUrl> urlList;
                foreach(QString url, list)
                urlList.append(QUrl::fromUserInput(url));
                settingsKey->defaultValue = QVariant::fromValue(urlList);
            } else
                settingsKey->defaultValue = defaultValue;

            m_entries.append(settingsKey);
        }
    }

    return true;
}

QConfigurationKey *QConfigurationSchema::lookupKey(const QString &keyName)
{
    // Split keyName to get group and key name
    QStringList parts = keyName.split(QLatin1Char('/'));
    if (parts.length() != 2) {
        qWarning("Settings key '%s' has a wrong notation!", keyName.toUtf8().constData());
        return 0;
    }

    // FIXME: This need to be optimized, for example instead of a list of
    // QConfigurationKey we need a map between groups and lists of keys.
    foreach(QConfigurationKey * key, m_entries) {
        // Ignore keys of other groups
        if (key->group != parts.at(0))
            continue;

        if (key->name == parts.at(1))
            return key;
    }

    return 0;
}
