// Copyright (C) 2022 The Qt Company Ltd.
// Copyright (C) 2016 Intel Corporation.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QSINGLESHOTTIMER_P_H
#define QSINGLESHOTTIMER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/qobject.h>
#include <QtCore/qabstracteventdispatcher.h>
#include <QtCore/qnamespace.h>

QT_BEGIN_NAMESPACE

namespace QtPrivate {
class QSlotObjectBase;
}

class QSingleShotTimer : public QObject
{
    Q_OBJECT

    Qt::TimerId timerId = Qt::TimerId::Invalid;

public:
    // use the same duration type
    using Duration = QAbstractEventDispatcher::Duration;

    explicit QSingleShotTimer(Duration interval, Qt::TimerType timerType,
                              const QObject *r, const char *member);
    explicit QSingleShotTimer(Duration interval, Qt::TimerType timerType,
                              const QObject *r, QtPrivate::QSlotObjectBase *slotObj);
    ~QSingleShotTimer() override;

    void startTimerForReceiver(Duration interval, Qt::TimerType timerType,
                               const QObject *receiver);
Q_SIGNALS:
    void timeout();

private:
    void timerEvent(QTimerEvent *) override;
};

QT_END_NAMESPACE

#endif // QSINGLESHOTTIMER_P_H
