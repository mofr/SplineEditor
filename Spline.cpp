#include "Spline.h"
#include <QLinkedList>

Spline::Spline()
{
}

void Spline::add(const Spline::Knot &knot)
{
    knots.push_back(knot);
    dirty = true;
}

void Spline::insert(int i, const Spline::Knot &knot)
{
    knots.insert(i, knot);
    dirty = true;
}

void Spline::replace(int i, const Spline::Knot &knot)
{
    if(i >= knots.size()) return;
    knots[i] = knot;
    dirty = true;
}

void Spline::remove(int i)
{
    knots.erase(knots.begin() + i);
    dirty = true;
}

int Spline::size() const
{
    return knots.size();
}

const Spline::Knot &Spline::getKnot(int i) const
{
    return knots[i];
}

const QVector<Spline::Knot> &Spline::getKnots() const
{
    return knots;
}

int Spline::findClosest(QPointF point, qreal & distance) const
{
    if(knots.isEmpty())
    {
        return -1;
    }

    int closestIndex = 0;
    distance = (point - knots[0]).manhattanLength();

    for(int i = 1; i < knots.size(); ++i)
    {
        const Knot & knot = knots.at(i);
        qreal newDistance = (point - knot).manhattanLength();
        if(newDistance < distance)
        {
            distance = newDistance;
            closestIndex = i;
        }
    }

    return closestIndex;
}

QVector<QPointF> Spline::getCurve() const
{
    if(dirty)
    {
        interpolate(curve, knots);
        dirty = false;
    }

    return curve;
}

void Spline::interpolate(QVector<QPointF> &curve, const QVector<Spline::Knot> &knots)
{
    curve.clear();

    if(knots.size() < 2)
    {
        return;
    }

    static const int subdivs = 30;
    static const qreal uStep = 1.0 / subdivs;

    curve.reserve((knots.size()-1)*subdivs + 1);

    for(int k = 0; k < knots.size() - 1; ++k)
    {
        const Knot & prev = k == 0 ? knots[k] : knots[k-1];
        const Knot & cur = knots[k];
        const Knot & next = knots[k+1];
        const Knot & next2 = k + 2 >= knots.size() ? knots[k+1] : knots[k+2];

        float t = cur.t;
        float b = cur.b;
        float c = cur.c;
        QPointF d0 = 0.5*(1-t)*((1+b)*(1-c)*(cur - prev) + (1-b)*(1+c)*(next - cur));
        t = next.t;
        b = next.b;
        c = next.c;
        QPointF d1 = 0.5*(1-t)*((1+b)*(1+c)*(next - cur) + (1-b)*(1-c)*(next2 - next));

        qreal u = 0.0;
        for(int subdiv = 0; subdiv < subdivs; ++subdiv)
        {
            qreal u2 = u*u;
            qreal u3 = u*u*u;
            curve.push_back((2*u3 - 3*u2 + 1) * cur
                    + (-2*u3 + 3*u2) * next
                    + (u3 - 2*u2 + u) * d0
                    + (u3 - u2) * d1);

            u += uStep;
        }
    }

    curve.push_back(knots.last());
}
