/****************************************************************************
**
** Copyright (C) 2014 Pier Luigi Fiorini.
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

#include <QtCore/QPointer>

#include "qdconfconfigurationbackend.h"
#include "qstaticconfiguration.h"

/*
 * QStaticConfigurationPrivate
 */

class QStaticConfigurationPrivate
{
    Q_DECLARE_PUBLIC(QStaticConfiguration)
public:
    QStaticConfigurationPrivate();

    QStaticConfiguration *q_ptr;
    QPointer<QConfigurationBackend> backend;
};

QStaticConfigurationPrivate::QStaticConfigurationPrivate()
{
}

/*
 * QStaticConfiguration
 */

QStaticConfiguration::QStaticConfiguration(QObject *parent)
    : QObject(parent)
    , d_ptr(new QStaticConfigurationPrivate())
{
    Q_D(QStaticConfiguration);
    d->q_ptr = this;
    d->backend = new QDConfConfigurationBackend(this);
}

QString QStaticConfiguration::category() const
{
    Q_D(const QStaticConfiguration);
    return d->backend->category();
}

void QStaticConfiguration::setCategory(const QString &category)
{
    Q_D(QStaticConfiguration);
    d->backend->setCategory(category);
}

QStringList QStaticConfiguration::allKeys() const
{
    return QStringList();
}

bool QStaticConfiguration::contains(const QString &key) const
{
    Q_D(const QStaticConfiguration);
    return d->backend->contains(key);
}

QVariant QStaticConfiguration::value(const QString &key, const QVariant &defaultValue) const
{
    Q_D(const QStaticConfiguration);
    return d->backend->value(key, defaultValue);
}

void QStaticConfiguration::setValue(const QString &key, const QVariant &value)
{
    Q_D(const QStaticConfiguration);
    d->backend->setValue(key, value);
}

#include "moc_qstaticconfiguration.cpp"
