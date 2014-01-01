/****************************************************************************
**
** Copyright (C) 2013-2014 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013-2014 Pier Luigi Fiorini.
** Contact: http://www.qt-project.org/legal
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCONFIGURATION_H
#define QCONFIGURATION_H

#include <QtCore/QObject>

#include <QtConfiguration/qconfigurationglobal.h>

QT_BEGIN_NAMESPACE

class QConfigurationPrivate;

class Q_CONFIGURATION_EXPORT QConfiguration : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString category READ category WRITE setCategory FINAL)
public:
    explicit QConfiguration(QObject *target, const QString &category, QObject *parent = 0);
    QConfiguration(QObject *target, QObject *parent = 0);
    ~QConfiguration();

    QObject *target() const;

    QString category() const;
    void setCategory(const QString &category);

protected:
    void timerEvent(QTimerEvent *event);

private:
    Q_DISABLE_COPY(QConfiguration)
    Q_DECLARE_PRIVATE(QConfiguration)
    QScopedPointer<QConfigurationPrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_propertyChanged())
};

QT_END_NAMESPACE

#endif // QCONFIGURATION_H
