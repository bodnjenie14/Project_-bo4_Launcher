// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QMARGINS_H
#define QMARGINS_H

#include <QtCore/qcompare.h>
#include <QtCore/qnamespace.h>

#include <QtCore/q20type_traits.h>
#include <QtCore/q23utility.h>

QT_BEGIN_NAMESPACE

QT_ENABLE_P0846_SEMANTICS_FOR(get)

class QMarginsF;

/*****************************************************************************
  QMargins class
 *****************************************************************************/

class QMargins
{
public:
    constexpr QMargins() noexcept;
    constexpr QMargins(int left, int top, int right, int bottom) noexcept;

    constexpr bool isNull() const noexcept;

    constexpr int left() const noexcept;
    constexpr int top() const noexcept;
    constexpr int right() const noexcept;
    constexpr int bottom() const noexcept;

    constexpr void setLeft(int left) noexcept;
    constexpr void setTop(int top) noexcept;
    constexpr void setRight(int right) noexcept;
    constexpr void setBottom(int bottom) noexcept;

    constexpr QMargins &operator+=(const QMargins &margins) noexcept;
    constexpr QMargins &operator-=(const QMargins &margins) noexcept;
    constexpr QMargins &operator+=(int) noexcept;
    constexpr QMargins &operator-=(int) noexcept;
    constexpr QMargins &operator*=(int) noexcept;
    constexpr QMargins &operator/=(int);
    constexpr QMargins &operator*=(qreal) noexcept;
    constexpr QMargins &operator/=(qreal);

    [[nodiscard]] constexpr inline QMarginsF toMarginsF() const noexcept;

private:
    int m_left;
    int m_top;
    int m_right;
    int m_bottom;

    friend constexpr bool comparesEqual(const QMargins &lhs, const QMargins &rhs) noexcept
    {
        return lhs.m_left == rhs.m_left
                && lhs.m_top == rhs.m_top
                && lhs.m_right == rhs.m_right
                && lhs.m_bottom == rhs.m_bottom;
    }
    Q_DECLARE_EQUALITY_COMPARABLE_LITERAL_TYPE(QMargins)

    template <std::size_t I,
              typename M,
              std::enable_if_t<(I < 4), bool> = true,
              std::enable_if_t<std::is_same_v<q20::remove_cvref_t<M>, QMargins>, bool> = true>
    friend constexpr decltype(auto) get(M &&m) noexcept
    {
        if constexpr (I == 0)
            return q23::forward_like<M>(m.m_left);
        else if constexpr (I == 1)
            return q23::forward_like<M>(m.m_top);
        else if constexpr (I == 2)
            return q23::forward_like<M>(m.m_right);
        else if constexpr (I == 3)
            return q23::forward_like<M>(m.m_bottom);
    }
};

Q_DECLARE_TYPEINFO(QMargins, Q_RELOCATABLE_TYPE);

/*****************************************************************************
  QMargins stream functions
 *****************************************************************************/
#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QMargins &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QMargins &);
#endif

/*****************************************************************************
  QMargins inline functions
 *****************************************************************************/

constexpr inline QMargins::QMargins() noexcept : m_left(0), m_top(0), m_right(0), m_bottom(0) {}

constexpr inline QMargins::QMargins(int aleft, int atop, int aright, int abottom) noexcept
    : m_left(aleft), m_top(atop), m_right(aright), m_bottom(abottom) {}

constexpr inline bool QMargins::isNull() const noexcept
{ return m_left==0 && m_top==0 && m_right==0 && m_bottom==0; }

constexpr inline int QMargins::left() const noexcept
{ return m_left; }

constexpr inline int QMargins::top() const noexcept
{ return m_top; }

constexpr inline int QMargins::right() const noexcept
{ return m_right; }

constexpr inline int QMargins::bottom() const noexcept
{ return m_bottom; }


constexpr inline void QMargins::setLeft(int aleft) noexcept
{ m_left = aleft; }

constexpr inline void QMargins::setTop(int atop) noexcept
{ m_top = atop; }

constexpr inline void QMargins::setRight(int aright) noexcept
{ m_right = aright; }

constexpr inline void QMargins::setBottom(int abottom) noexcept
{ m_bottom = abottom; }

constexpr inline QMargins operator+(const QMargins &m1, const QMargins &m2) noexcept
{
    return QMargins(m1.left() + m2.left(), m1.top() + m2.top(),
                    m1.right() + m2.right(), m1.bottom() + m2.bottom());
}

constexpr inline QMargins operator-(const QMargins &m1, const QMargins &m2) noexcept
{
    return QMargins(m1.left() - m2.left(), m1.top() - m2.top(),
                    m1.right() - m2.right(), m1.bottom() - m2.bottom());
}

constexpr inline QMargins operator+(const QMargins &lhs, int rhs) noexcept
{
    return QMargins(lhs.left() + rhs, lhs.top() + rhs,
                    lhs.right() + rhs, lhs.bottom() + rhs);
}

constexpr inline QMargins operator+(int lhs, const QMargins &rhs) noexcept
{
    return QMargins(rhs.left() + lhs, rhs.top() + lhs,
                    rhs.right() + lhs, rhs.bottom() + lhs);
}

constexpr inline QMargins operator-(const QMargins &lhs, int rhs) noexcept
{
    return QMargins(lhs.left() - rhs, lhs.top() - rhs,
                    lhs.right() - rhs, lhs.bottom() - rhs);
}

constexpr inline QMargins operator*(const QMargins &margins, int factor) noexcept
{
    return QMargins(margins.left() * factor, margins.top() * factor,
                    margins.right() * factor, margins.bottom() * factor);
}

constexpr inline QMargins operator*(int factor, const QMargins &margins) noexcept
{
    return QMargins(margins.left() * factor, margins.top() * factor,
                    margins.right() * factor, margins.bottom() * factor);
}

constexpr inline QMargins operator*(const QMargins &margins, qreal factor) noexcept
{
    return QMargins(qRound(margins.left() * factor), qRound(margins.top() * factor),
                    qRound(margins.right() * factor), qRound(margins.bottom() * factor));
}

constexpr inline QMargins operator*(qreal factor, const QMargins &margins) noexcept
{
    return QMargins(qRound(margins.left() * factor), qRound(margins.top() * factor),
                    qRound(margins.right() * factor), qRound(margins.bottom() * factor));
}

constexpr inline QMargins operator/(const QMargins &margins, int divisor)
{
    return QMargins(margins.left() / divisor, margins.top() / divisor,
                    margins.right() / divisor, margins.bottom() / divisor);
}

constexpr inline QMargins operator/(const QMargins &margins, qreal divisor)
{
    return QMargins(qRound(margins.left() / divisor), qRound(margins.top() / divisor),
                    qRound(margins.right() / divisor), qRound(margins.bottom() / divisor));
}

constexpr inline QMargins operator|(const QMargins &m1, const QMargins &m2) noexcept
{
    return QMargins(qMax(m1.left(), m2.left()), qMax(m1.top(), m2.top()),
                    qMax(m1.right(), m2.right()), qMax(m1.bottom(), m2.bottom()));
}

constexpr inline QMargins &QMargins::operator+=(const QMargins &margins) noexcept
{
    return *this = *this + margins;
}

constexpr inline QMargins &QMargins::operator-=(const QMargins &margins) noexcept
{
    return *this = *this - margins;
}

constexpr inline QMargins &QMargins::operator+=(int margin) noexcept
{
    m_left += margin;
    m_top += margin;
    m_right += margin;
    m_bottom += margin;
    return *this;
}

constexpr inline QMargins &QMargins::operator-=(int margin) noexcept
{
    m_left -= margin;
    m_top -= margin;
    m_right -= margin;
    m_bottom -= margin;
    return *this;
}

constexpr inline QMargins &QMargins::operator*=(int factor) noexcept
{
    return *this = *this * factor;
}

constexpr inline QMargins &QMargins::operator/=(int divisor)
{
    return *this = *this / divisor;
}

constexpr inline QMargins &QMargins::operator*=(qreal factor) noexcept
{
    return *this = *this * factor;
}

constexpr inline QMargins &QMargins::operator/=(qreal divisor)
{
    return *this = *this / divisor;
}

constexpr inline QMargins operator+(const QMargins &margins) noexcept
{
    return margins;
}

constexpr inline QMargins operator-(const QMargins &margins) noexcept
{
    return QMargins(-margins.left(), -margins.top(), -margins.right(), -margins.bottom());
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QMargins &);
#endif

/*****************************************************************************
  QMarginsF class
 *****************************************************************************/

class QMarginsF
{
public:
    constexpr QMarginsF() noexcept;
    constexpr QMarginsF(qreal left, qreal top, qreal right, qreal bottom) noexcept;
    constexpr QMarginsF(const QMargins &margins) noexcept;

    constexpr bool isNull() const noexcept;

    constexpr qreal left() const noexcept;
    constexpr qreal top() const noexcept;
    constexpr qreal right() const noexcept;
    constexpr qreal bottom() const noexcept;

    constexpr void setLeft(qreal aleft) noexcept;
    constexpr void setTop(qreal atop) noexcept;
    constexpr void setRight(qreal aright) noexcept;
    constexpr void setBottom(qreal abottom) noexcept;

    constexpr QMarginsF &operator+=(const QMarginsF &margins) noexcept;
    constexpr QMarginsF &operator-=(const QMarginsF &margins) noexcept;
    constexpr QMarginsF &operator+=(qreal addend) noexcept;
    constexpr QMarginsF &operator-=(qreal subtrahend) noexcept;
    constexpr QMarginsF &operator*=(qreal factor) noexcept;
    constexpr QMarginsF &operator/=(qreal divisor);

    constexpr inline QMargins toMargins() const noexcept;

private:
    qreal m_left;
    qreal m_top;
    qreal m_right;
    qreal m_bottom;

    QT_WARNING_PUSH
    QT_WARNING_DISABLE_FLOAT_COMPARE
    friend constexpr bool qFuzzyCompare(const QMarginsF &lhs, const QMarginsF &rhs) noexcept
    {
        return ((!lhs.m_left || !rhs.m_left) ? qFuzzyIsNull(lhs.m_left - rhs.m_left)
                                             : qFuzzyCompare(lhs.m_left, rhs.m_left))
                && ((!lhs.m_top || !rhs.m_top) ? qFuzzyIsNull(lhs.m_top - rhs.m_top)
                                               : qFuzzyCompare(lhs.m_top, rhs.m_top))
                && ((!lhs.m_right || !rhs.m_right) ? qFuzzyIsNull(lhs.m_right - rhs.m_right)
                                                   : qFuzzyCompare(lhs.m_right, rhs.m_right))
                && ((!lhs.m_bottom || !rhs.m_bottom) ? qFuzzyIsNull(lhs.m_bottom - rhs.m_bottom)
                                                     : qFuzzyCompare(lhs.m_bottom, rhs.m_bottom));
    }
    QT_WARNING_POP
    friend constexpr bool qFuzzyIsNull(const QMarginsF &m) noexcept
    {
        return qFuzzyIsNull(m.m_left) && qFuzzyIsNull(m.m_top)
                && qFuzzyIsNull(m.m_right) && qFuzzyIsNull(m.m_bottom);
    }

    friend constexpr bool comparesEqual(const QMarginsF &lhs, const QMarginsF &rhs) noexcept
    {
        return qFuzzyCompare(lhs, rhs);
    }
    Q_DECLARE_EQUALITY_COMPARABLE_LITERAL_TYPE(QMarginsF)

    friend constexpr bool comparesEqual(const QMarginsF &lhs, const QMargins &rhs) noexcept
    { return comparesEqual(lhs, rhs.toMarginsF()); }
    Q_DECLARE_EQUALITY_COMPARABLE_LITERAL_TYPE(QMarginsF, QMargins)

    template <std::size_t I,
              typename M,
              std::enable_if_t<(I < 4), bool> = true,
              std::enable_if_t<std::is_same_v<q20::remove_cvref_t<M>, QMarginsF>, bool> = true>
    friend constexpr decltype(auto) get(M &&m) noexcept
    {
        if constexpr (I == 0)
            return q23::forward_like<M>(m.m_left);
        else if constexpr (I == 1)
            return q23::forward_like<M>(m.m_top);
        else if constexpr (I == 2)
            return q23::forward_like<M>(m.m_right);
        else if constexpr (I == 3)
            return q23::forward_like<M>(m.m_bottom);
    }
};

Q_DECLARE_TYPEINFO(QMarginsF, Q_RELOCATABLE_TYPE);

/*****************************************************************************
  QMarginsF stream functions
 *****************************************************************************/

#ifndef QT_NO_DATASTREAM
Q_CORE_EXPORT QDataStream &operator<<(QDataStream &, const QMarginsF &);
Q_CORE_EXPORT QDataStream &operator>>(QDataStream &, QMarginsF &);
#endif

/*****************************************************************************
  QMarginsF inline functions
 *****************************************************************************/

constexpr inline QMarginsF::QMarginsF() noexcept
    : m_left(0), m_top(0), m_right(0), m_bottom(0) {}

constexpr inline QMarginsF::QMarginsF(qreal aleft, qreal atop, qreal aright, qreal abottom) noexcept
    : m_left(aleft), m_top(atop), m_right(aright), m_bottom(abottom) {}

constexpr inline QMarginsF::QMarginsF(const QMargins &margins) noexcept
    : m_left(margins.left()), m_top(margins.top()), m_right(margins.right()), m_bottom(margins.bottom()) {}

constexpr inline bool QMarginsF::isNull() const noexcept
{ return qFuzzyIsNull(m_left) && qFuzzyIsNull(m_top) && qFuzzyIsNull(m_right) && qFuzzyIsNull(m_bottom); }

constexpr inline qreal QMarginsF::left() const noexcept
{ return m_left; }

constexpr inline qreal QMarginsF::top() const noexcept
{ return m_top; }

constexpr inline qreal QMarginsF::right() const noexcept
{ return m_right; }

constexpr inline qreal QMarginsF::bottom() const noexcept
{ return m_bottom; }


constexpr inline void QMarginsF::setLeft(qreal aleft) noexcept
{ m_left = aleft; }

constexpr inline void QMarginsF::setTop(qreal atop) noexcept
{ m_top = atop; }

constexpr inline void QMarginsF::setRight(qreal aright) noexcept
{ m_right = aright; }

constexpr inline void QMarginsF::setBottom(qreal abottom) noexcept
{ m_bottom = abottom; }

constexpr inline QMarginsF operator+(const QMarginsF &lhs, const QMarginsF &rhs) noexcept
{
    return QMarginsF(lhs.left() + rhs.left(), lhs.top() + rhs.top(),
                     lhs.right() + rhs.right(), lhs.bottom() + rhs.bottom());
}

constexpr inline QMarginsF operator-(const QMarginsF &lhs, const QMarginsF &rhs) noexcept
{
    return QMarginsF(lhs.left() - rhs.left(), lhs.top() - rhs.top(),
                     lhs.right() - rhs.right(), lhs.bottom() - rhs.bottom());
}

constexpr inline QMarginsF operator+(const QMarginsF &lhs, qreal rhs) noexcept
{
    return QMarginsF(lhs.left() + rhs, lhs.top() + rhs,
                     lhs.right() + rhs, lhs.bottom() + rhs);
}

constexpr inline QMarginsF operator+(qreal lhs, const QMarginsF &rhs) noexcept
{
    return QMarginsF(rhs.left() + lhs, rhs.top() + lhs,
                     rhs.right() + lhs, rhs.bottom() + lhs);
}

constexpr inline QMarginsF operator-(const QMarginsF &lhs, qreal rhs) noexcept
{
    return QMarginsF(lhs.left() - rhs, lhs.top() - rhs,
                     lhs.right() - rhs, lhs.bottom() - rhs);
}

constexpr inline QMarginsF operator*(const QMarginsF &lhs, qreal rhs) noexcept
{
    return QMarginsF(lhs.left() * rhs, lhs.top() * rhs,
                     lhs.right() * rhs, lhs.bottom() * rhs);
}

constexpr inline QMarginsF operator*(qreal lhs, const QMarginsF &rhs) noexcept
{
    return QMarginsF(rhs.left() * lhs, rhs.top() * lhs,
                     rhs.right() * lhs, rhs.bottom() * lhs);
}

constexpr inline QMarginsF operator/(const QMarginsF &lhs, qreal divisor)
{
    Q_ASSERT(divisor < 0 || divisor > 0);
    return QMarginsF(lhs.left() / divisor, lhs.top() / divisor,
                     lhs.right() / divisor, lhs.bottom() / divisor);
}

constexpr inline QMarginsF operator|(const QMarginsF &m1, const QMarginsF &m2) noexcept
{
    return QMarginsF(qMax(m1.left(), m2.left()), qMax(m1.top(), m2.top()),
                     qMax(m1.right(), m2.right()), qMax(m1.bottom(), m2.bottom()));
}

constexpr inline QMarginsF &QMarginsF::operator+=(const QMarginsF &margins) noexcept
{
    return *this = *this + margins;
}

constexpr inline QMarginsF &QMarginsF::operator-=(const QMarginsF &margins) noexcept
{
    return *this = *this - margins;
}

constexpr inline QMarginsF &QMarginsF::operator+=(qreal addend) noexcept
{
    m_left += addend;
    m_top += addend;
    m_right += addend;
    m_bottom += addend;
    return *this;
}

constexpr inline QMarginsF &QMarginsF::operator-=(qreal subtrahend) noexcept
{
    m_left -= subtrahend;
    m_top -= subtrahend;
    m_right -= subtrahend;
    m_bottom -= subtrahend;
    return *this;
}

constexpr inline QMarginsF &QMarginsF::operator*=(qreal factor) noexcept
{
    return *this = *this * factor;
}

constexpr inline QMarginsF &QMarginsF::operator/=(qreal divisor)
{
    return *this = *this / divisor;
}

constexpr inline QMarginsF operator+(const QMarginsF &margins) noexcept
{
    return margins;
}

constexpr inline QMarginsF operator-(const QMarginsF &margins) noexcept
{
    return QMarginsF(-margins.left(), -margins.top(), -margins.right(), -margins.bottom());
}

constexpr QMarginsF QMargins::toMarginsF() const noexcept { return *this; }

constexpr inline QMargins QMarginsF::toMargins() const noexcept
{
    return QMargins(qRound(m_left), qRound(m_top), qRound(m_right), qRound(m_bottom));
}

#ifndef QT_NO_DEBUG_STREAM
Q_CORE_EXPORT QDebug operator<<(QDebug, const QMarginsF &);
#endif

QT_END_NAMESPACE

/*****************************************************************************
  QMargins/QMarginsF tuple protocol
 *****************************************************************************/

namespace std {
    template <>
    class tuple_size<QT_PREPEND_NAMESPACE(QMargins)> : public integral_constant<size_t, 4> {};
    template <>
    class tuple_element<0, QT_PREPEND_NAMESPACE(QMargins)> { public: using type = int; };
    template <>
    class tuple_element<1, QT_PREPEND_NAMESPACE(QMargins)> { public: using type = int; };
    template <>
    class tuple_element<2, QT_PREPEND_NAMESPACE(QMargins)> { public: using type = int; };
    template <>
    class tuple_element<3, QT_PREPEND_NAMESPACE(QMargins)> { public: using type = int; };

    template <>
    class tuple_size<QT_PREPEND_NAMESPACE(QMarginsF)> : public integral_constant<size_t, 4> {};
    template <>
    class tuple_element<0, QT_PREPEND_NAMESPACE(QMarginsF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
    template <>
    class tuple_element<1, QT_PREPEND_NAMESPACE(QMarginsF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
    template <>
    class tuple_element<2, QT_PREPEND_NAMESPACE(QMarginsF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
    template <>
    class tuple_element<3, QT_PREPEND_NAMESPACE(QMarginsF)> { public: using type = QT_PREPEND_NAMESPACE(qreal); };
}

#endif // QMARGINS_H
