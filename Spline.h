#pragma once

#include <QPointF>
#include <QVector>

class Spline
{
public:
    struct Knot : public QPointF
    {
        Knot(QPointF pos) :
            QPointF(pos)
        {}

        Knot(qreal x = 0.0f, qreal y = 0.0f) :
            QPointF(x, y)
        {}

        Knot(qreal x, qreal y, float t, float b, float c) :
            QPointF(x, y),
            t(t),
            b(b),
            c(c)
        {}

        float t = 0.0f; // tension
        float b = 0.0f; // bias
        float c = 0.0f; // continuity
    };

    Spline();

    void add(const Knot &knot);
    void insert(int i, const Knot &knot);
    void replace(int i, const Knot &knot);
    void remove(int i);

    int size() const;

    const Knot & getKnot(int index) const;
    const QVector<Knot> & getKnots() const;

    /*
     * @return closest knot index or -1 if spline is empty
     */
    int findClosest(QPointF pos, qreal & distance) const;

    QVector<QPointF> getCurve() const;

    static void interpolate(QVector<QPointF> & curve, const QVector<Knot> & knots);

private:
    QVector<Knot> knots;
    mutable QVector<QPointF> curve;
    mutable bool dirty = false;
};
