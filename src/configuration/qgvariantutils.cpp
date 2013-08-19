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

#include <QtCore/QRect>
#include <QtCore/QStringList>
#include <QtCore/QUrl>

#include "qgvariantutils.h"

QT_BEGIN_NAMESPACE

QVariant convertValue(GVariant *value)
{
    if (!value)
        return QVariant(QVariant::Invalid);

    switch (g_variant_classify(value)) {
    case G_VARIANT_CLASS_BOOLEAN:
        return QVariant(g_variant_get_boolean(value));
    case G_VARIANT_CLASS_BYTE:
        return QVariant((char)g_variant_get_byte(value));
    case G_VARIANT_CLASS_INT16:
        return QVariant((qint16)g_variant_get_int16(value));
    case G_VARIANT_CLASS_UINT16:
        return QVariant((quint16)g_variant_get_uint16(value));
    case G_VARIANT_CLASS_INT32:
        return QVariant((qint32)g_variant_get_int32(value));
    case G_VARIANT_CLASS_UINT32:
        return QVariant((quint32)g_variant_get_uint32(value));
    case G_VARIANT_CLASS_INT64:
        return QVariant((qint64)g_variant_get_int64(value));
    case G_VARIANT_CLASS_UINT64:
        return QVariant((quint64)g_variant_get_uint64(value));
    case G_VARIANT_CLASS_DOUBLE:
        return QVariant((double)g_variant_get_double(value));
    case G_VARIANT_CLASS_STRING: {
        QString str = QString::fromUtf8(g_variant_get_string(value, NULL));
        if (str.startsWith(QLatin1Char('@')) && str.endsWith(QLatin1Char(')'))) {
            if (str.startsWith(QLatin1String("@Url(")))
                return QVariant(QUrl::fromUserInput(str.mid(5, str.size() - 6)));
            else if (str.startsWith(QLatin1String("@Variant("))) {
#ifndef QT_NO_DATASTREAM
                QByteArray byteArray(str.toUtf8().mid(9));
                QDataStream stream(&byteArray, QIODevice::ReadOnly);
                stream.setVersion(QDataStream::Qt_4_0);

                QVariant result;
                stream >> result;
                return result;
#else
                Q_ASSERT("QConfiguration: cannot load custom type without QDataStream support");
#endif
            }
        }
        return QVariant(str);
    }
    case G_VARIANT_CLASS_ARRAY:
        if (g_variant_is_of_type(value, G_VARIANT_TYPE_STRING_ARRAY)) {
            GVariantIter iter;
            QStringList list;
            const gchar *str;

            g_variant_iter_init(&iter, value);

            while (g_variant_iter_next(&iter, "&s", &str))
                list.append(QString::fromUtf8(str));

            return QVariant(list);
        } else if (g_variant_is_of_type(value, G_VARIANT_TYPE_BYTESTRING)) {
            return QVariant(QByteArray(g_variant_get_bytestring(value)));
        }
    default:
        break;
    }

    return QVariant(QVariant::Invalid);
}

GVariant *convertVariant(const QVariant &variant)
{
    switch ((QMetaType::Type)variant.type()) {
    case QMetaType::Bool:
        return g_variant_new_boolean(variant.toBool());
    case QMetaType::Char:
        return g_variant_new_byte(variant.toChar().toLatin1());
    case QMetaType::Short: {
        bool ok = false;
        int value = variant.toInt(&ok);
        if (ok)
            return g_variant_new_int16((qint16)value);
        return 0;
    }
    case QMetaType::UShort: {
        bool ok = false;
        uint value = variant.toUInt(&ok);
        if (ok)
            return g_variant_new_uint16((quint16)value);
        return 0;
    }
    case QMetaType::Int:
    case QMetaType::Long: {
        bool ok = false;
        int value = variant.toInt(&ok);
        if (ok)
            return g_variant_new_int32((qint32)value);
        return 0;
    }
    case QMetaType::UInt:
    case QMetaType::ULong: {
        bool ok = false;
        uint value = variant.toUInt(&ok);
        if (ok)
            return g_variant_new_uint32((quint32)value);
        return 0;
    }
    case QMetaType::LongLong: {
        bool ok = false;
        qlonglong value = variant.toLongLong(&ok);
        if (ok)
            return g_variant_new_int64((qint64)value);
        return 0;
    }
    case QMetaType::ULongLong: {
        bool ok = false;
        qulonglong value = variant.toULongLong(&ok);
        if (ok)
            return g_variant_new_int64((quint64)value);
        return 0;
    }
    case QMetaType::Float: {
        bool ok = false;
        float value = variant.toFloat(&ok);
        if (ok)
            return g_variant_new_double((gdouble)value);
    }
    case QMetaType::Double: {
        bool ok = false;
        double value = variant.toDouble(&ok);
        if (ok)
            return g_variant_new_double(value);
    }
    case QMetaType::QString: {
        QString value = variant.toString();
        return g_variant_new_string(value.toUtf8().constData());
    }
    case QMetaType::QUrl: {
        QString value = QLatin1String("@Url(") + variant.toUrl().toString() + QLatin1String(")");
        return g_variant_new_string(value.toUtf8().constData());
    }
    case QMetaType::QRect: {
        QRect value = variant.toRect();
        GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE("ai"));
        g_variant_builder_add(builder, "i", value.x());
        g_variant_builder_add(builder, "i", value.y());
        g_variant_builder_add(builder, "i", value.width());
        g_variant_builder_add(builder, "i", value.height());
        GVariant *result = g_variant_new("ai", builder);
        g_variant_builder_unref(builder);
        return result;
    }
    default: {
#ifndef QT_NO_DATASTREAM
        QByteArray byteArray;
        QDataStream stream(&byteArray, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_0);
        stream << variant;

        QString value = QLatin1String("@Variant(");
        value += QString::fromUtf8(byteArray.constData());
        value += QLatin1String(")");
        return g_variant_new_string(value.toUtf8().constData());
#else
        Q_ASSERT("QConfiguration: cannot save custom types without QDataStream support");
#endif
    }
    }

    return 0;
}

QT_END_NAMESPACE
