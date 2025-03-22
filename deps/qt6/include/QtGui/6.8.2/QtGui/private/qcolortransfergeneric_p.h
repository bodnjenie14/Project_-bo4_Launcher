// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCOLORTRANSFERGENERIC_P_H
#define QCOLORTRANSFERGENERIC_P_H

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

#include <QtGui/private/qtguiglobal_p.h>

#include <cmath>

QT_BEGIN_NAMESPACE

// Defines the a generic transfer function for our HDR functions
class QColorTransferGenericFunction
{
public:
    using ConverterPtr = float (*)(float);
    constexpr QColorTransferGenericFunction(ConverterPtr toLinear = nullptr, ConverterPtr fromLinear = nullptr) noexcept
        : m_toLinear(toLinear), m_fromLinear(fromLinear)
    {}

    static QColorTransferGenericFunction hlg()
    {
        return QColorTransferGenericFunction(hlgToLinear, hlgFromLinear);
    }
    static QColorTransferGenericFunction pq()
    {
        return QColorTransferGenericFunction(pqToLinear, pqFromLinear);
    }

    float apply(float x) const
    {
        return m_toLinear(x);
    }

    float applyInverse(float x) const
    {
        return m_fromLinear(x);
    }

    bool operator==(const QColorTransferGenericFunction &o) const noexcept
    {
        return m_toLinear == o.m_toLinear && m_fromLinear == o.m_fromLinear;
    }
    bool operator!=(const QColorTransferGenericFunction &o) const noexcept
    {
        return m_toLinear != o.m_toLinear || m_fromLinear != o.m_fromLinear;
    }

private:
    ConverterPtr m_toLinear = nullptr;
    ConverterPtr m_fromLinear = nullptr;

    // HLG from linear [0-12] -> [0-1]
    static float hlgFromLinear(float x)
    {
        if (x > 1.f)
            return m_hlg_a * std::log(x - m_hlg_b) + m_hlg_c;
        return std::sqrt(x * 0.25f);
    }

    // HLG to linear [0-1] -> [0-12]
    static float hlgToLinear(float x)
    {
        if (x < 0.5f)
            return (x * x) * 4.f;
        return std::exp((x - m_hlg_c) / m_hlg_a) + m_hlg_b;
    }

    constexpr static float m_hlg_a = 0.17883277f;
    constexpr static float m_hlg_b = 1.f - (4.f * m_hlg_a);
    constexpr static float m_hlg_c = 0.55991073f; // 0.5 - a * ln(4 * a)

    // PQ to linear [0-1] -> [0-64]
    static float pqToLinear(float x)
    {
        x = std::pow(x, 1.f / m_pq_m2);
        return std::pow((m_pq_c1 - x) / (m_pq_c3 * x - m_pq_c2), (1.f / m_pq_m1)) * m_pq_f;
    }

    // PQ from linear [0-64] -> [0-1]
    static float pqFromLinear(float x)
    {
        x = std::pow(x * (1.f / m_pq_f), m_pq_m1);
        return std::pow((m_pq_c1 + m_pq_c2 * x) / (1.f + m_pq_c3 * x), m_pq_m2);
    }

    constexpr static float m_pq_c1 =  107.f / 128.f; // c3 - c2 + 1
    constexpr static float m_pq_c2 = 2413.f / 128.f;
    constexpr static float m_pq_c3 = 2392.f / 128.f;
    constexpr static float m_pq_m1 = 1305.f / 8192.f;
    constexpr static float m_pq_m2 = 2523.f / 32.f;
    constexpr static float m_pq_f = 64.f; // This might need to be set based on scene metadata
};

QT_END_NAMESPACE

#endif // QCOLORTRANSFERGENERIC_P_H
