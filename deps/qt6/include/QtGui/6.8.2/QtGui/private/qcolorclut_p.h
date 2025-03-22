// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QCOLORCLUT_H
#define QCOLORCLUT_H

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

#include <QtCore/qlist.h>
#include <QtGui/private/qcolormatrix_p.h>

QT_BEGIN_NAMESPACE

// A 3/4-dimensional lookup table compatible with ICC lut8, lut16, mAB, and mBA formats.
class QColorCLUT
{
    inline static QColorVector interpolate(const QColorVector &a, const QColorVector &b, float t)
    {
        return a + (b - a) * t; // faster than std::lerp by assuming no super large or non-number floats
    }
    inline static void interpolateIn(QColorVector &a, const QColorVector &b, float t)
    {
        a += (b - a) * t;
    }
public:
    uint32_t gridPointsX = 0;
    uint32_t gridPointsY = 0;
    uint32_t gridPointsZ = 0;
    uint32_t gridPointsW = 1;
    QList<QColorVector> table;

    bool isEmpty() const { return table.isEmpty(); }

    QColorVector apply(const QColorVector &v) const
    {
        Q_ASSERT(table.size() == qsizetype(gridPointsX * gridPointsY * gridPointsZ * gridPointsW));
        QColorVector frac;
        const float x = std::clamp(v.x, 0.0f, 1.0f) * (gridPointsX - 1);
        const float y = std::clamp(v.y, 0.0f, 1.0f) * (gridPointsY - 1);
        const float z = std::clamp(v.z, 0.0f, 1.0f) * (gridPointsZ - 1);
        const float w = std::clamp(v.w, 0.0f, 1.0f) * (gridPointsW - 1);
        const uint32_t lox = static_cast<uint32_t>(std::floor(x));
        const uint32_t hix = std::min(lox + 1, gridPointsX - 1);
        const uint32_t loy = static_cast<uint32_t>(std::floor(y));
        const uint32_t hiy = std::min(loy + 1, gridPointsY - 1);
        const uint32_t loz = static_cast<uint32_t>(std::floor(z));
        const uint32_t hiz = std::min(loz + 1, gridPointsZ - 1);
        const uint32_t low = static_cast<uint32_t>(std::floor(w));
        const uint32_t hiw = std::min(low + 1, gridPointsW - 1);
        frac.x = x - static_cast<float>(lox);
        frac.y = y - static_cast<float>(loy);
        frac.z = z - static_cast<float>(loz);
        frac.w = w - static_cast<float>(low);
        if (gridPointsW > 1) {
            auto index = [&](qsizetype x, qsizetype y, qsizetype z, qsizetype w) -> qsizetype {
                return x * gridPointsW * gridPointsZ * gridPointsY
                     + y * gridPointsW * gridPointsZ
                     + z * gridPointsW
                     + w;
            };
            QColorVector tmp[8];
            // interpolate over w
            tmp[0] = interpolate(table[index(lox, loy, loz, low)],
                                 table[index(lox, loy, loz, hiw)], frac.w);
            tmp[1] = interpolate(table[index(lox, loy, hiz, low)],
                                 table[index(lox, loy, hiz, hiw)], frac.w);
            tmp[2] = interpolate(table[index(lox, hiy, loz, low)],
                                 table[index(lox, hiy, loz, hiw)], frac.w);
            tmp[3] = interpolate(table[index(lox, hiy, hiz, low)],
                                 table[index(lox, hiy, hiz, hiw)], frac.w);
            tmp[4] = interpolate(table[index(hix, loy, loz, low)],
                                 table[index(hix, loy, loz, hiw)], frac.w);
            tmp[5] = interpolate(table[index(hix, loy, hiz, low)],
                                 table[index(hix, loy, hiz, hiw)], frac.w);
            tmp[6] = interpolate(table[index(hix, hiy, loz, low)],
                                 table[index(hix, hiy, loz, hiw)], frac.w);
            tmp[7] = interpolate(table[index(hix, hiy, hiz, low)],
                                 table[index(hix, hiy, hiz, hiw)], frac.w);
            // interpolate over z
            for (int i = 0; i < 4; ++i)
                interpolateIn(tmp[i * 2], tmp[i * 2 + 1], frac.z);
            // interpolate over y
            for (int i = 0; i < 2; ++i)
                interpolateIn(tmp[i * 4], tmp[i * 4 + 2], frac.y);
            // interpolate over x
            interpolateIn(tmp[0], tmp[4], frac.x);
            return tmp[0];
        }
        auto index = [&](qsizetype x, qsizetype y, qsizetype z) -> qsizetype {
            return x * gridPointsZ * gridPointsY
                 + y * gridPointsZ
                 + z;
        };
        QColorVector tmp[8] = {
                table[index(lox, loy, loz)],
                table[index(lox, loy, hiz)],
                table[index(lox, hiy, loz)],
                table[index(lox, hiy, hiz)],
                table[index(hix, loy, loz)],
                table[index(hix, loy, hiz)],
                table[index(hix, hiy, loz)],
                table[index(hix, hiy, hiz)]
        };
        // interpolate over z
        for (int i = 0; i < 4; ++i)
            interpolateIn(tmp[i * 2], tmp[i * 2 + 1], frac.z);
        // interpolate over y
        for (int i = 0; i < 2; ++i)
            interpolateIn(tmp[i * 4], tmp[i * 4 + 2], frac.y);
        // interpolate over x
        interpolateIn(tmp[0], tmp[4], frac.x);
        return tmp[0];
    }
};

QT_END_NAMESPACE

#endif // QCOLORCLUT_H
