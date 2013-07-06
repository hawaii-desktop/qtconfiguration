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

#ifndef QCONFIGURATIONGLOBAL_H
#define QCONFIGURATIONGLOBAL_H

#include <QtCore/qglobal.h>

QT_BEGIN_NAMESPACE

#ifndef QT_STATIC
#  if defined(QT_BUILD_CONFIGURATION_LIB)
#    define Q_CONFIGURATION_EXPORT Q_DECL_EXPORT
#  else
#    define Q_CONFIGURATION_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_CONFIGURATION_EXPORT
#endif

// The macro has been available only since Qt 5.0
#ifndef Q_DECL_OVERRIDE
#define Q_DECL_OVERRIDE
#endif

QT_END_NAMESPACE

#endif // QCONFIGURATIONGLOBAL_H
