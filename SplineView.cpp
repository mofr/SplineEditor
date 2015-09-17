#include "SplineView.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>
#include <QLabel>

SplineView::SplineView(QWidget *parent) :
    QWidget(parent)
{
    setMouseTracking(true);
}

void SplineView::undo()
{
    history.undo(spline);
    hotIndex = -1;
    deselect();
    update();
}

void SplineView::redo()
{
    history.redo(spline);
    hotIndex = -1;
    deselect();
    update();
}

const Spline & SplineView::getSpline() const
{
    return spline;
}

void SplineView::setSpline(const Spline &spline)
{
    this->spline = spline;
    history.clear();
    hotIndex = -1;
    deselect();
    update();
}

void SplineView::updateSelectedKnot(Spline::Knot knot)
{
    if(selectedIndex != -1)
    {
        history.push(spline);
        spline.replace(selectedIndex, knot);
        update();
    }
}

void SplineView::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.fillRect(rect(), backgroundColor);

    //grid
    painter.setPen(gridColor);
    painter.drawLine(rect().center().x(), 0, rect().center().x(), height());
    painter.drawLine(0, rect().center().y(), width(), rect().center().y());

    painter.translate(rect().center());
    painter.setRenderHint(QPainter::Antialiasing);

    //spline
    QPen pen(splineColor);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.drawPolyline(spline.getCurve());

    //spline knots
    painter.setBrush(splineColor);
    for(const Spline::Knot & knot : spline.getKnots())
    {
        painter.drawEllipse(knot, 3, 3);
    }

    //hot knot
    if(hotIndex != -1)
    {
        painter.setBrush(Qt::white);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(spline.getKnot(hotIndex), 4, 4);
    }

    //selected knot
    if(selectedIndex != -1)
    {
        painter.setBrush(Qt::white);
        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawEllipse(spline.getKnot(selectedIndex), 6, 6);
    }

    //new knot draft
    if(insertIndex != -1)
    {
        pen.setWidth(1);
        pen.setStyle(Qt::DashLine);
        painter.setBrush(Qt::transparent);
        painter.setPen(pen);

        if(insertIndex < spline.size())
        {
            Spline::Knot knot = spline.getKnot(insertIndex);
            painter.drawLine(knot, insertPos);

            if(insertIndex - 1 >= 0)
            {
                knot = spline.getKnot(insertIndex - 1);
                painter.drawLine(knot, insertPos);
            }
        }

        painter.drawEllipse(insertPos, 6, 6);
    }
}

void SplineView::mouseMoveEvent(QMouseEvent *event)
{
    setFocus();

    QPoint pos = event->pos() - rect().center();

    if(movingKnot && hotIndex != -1)
    {
        Spline::Knot knot = spline.getKnot(hotIndex);
        knot.setX(pos.x());
        knot.setY(pos.y());
        spline.replace(hotIndex, knot);
        emit knotSelected(knot);
    }
    else
    {
        qreal distance;
        hotIndex = spline.findClosest(pos, distance);
        if(distance > HOT_KNOT_RADIUS)
        {
            hotIndex = -1;
        }
    }

    if(event->buttons() & Qt::LeftButton && hotIndex != -1)
    {
        if(!movingKnot)
        {
            history.push(spline);
            movingKnot = true;
        }
    }

    if(insertIndex != -1)
    {
        updateInsertState();
    }

    update();
}

void SplineView::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton && insertIndex != -1)
    {
        history.push(spline);
        spline.insert(insertIndex, Spline::Knot(insertPos));
        deselect();
        update();
    }
    else
    if(selectedIndex != -1 && hotIndex == -1)
    {
        deselect();
    }
    else
    if(event->buttons() & Qt::RightButton && hotIndex != -1)
    {
        history.push(spline);
        spline.remove(hotIndex);
        hotIndex = -1;
        update();
    }
    else
    if(event->buttons() & Qt::LeftButton && hotIndex != -1)
    {
        selectedIndex = hotIndex;
        emit knotSelected(spline.getKnot(selectedIndex));
        update();
    }
}

void SplineView::mouseReleaseEvent(QMouseEvent *)
{
    movingKnot = false;
}

void SplineView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        deselect();
    }
    else
    if(event->key() == Qt::Key_Shift)
    {
        updateInsertState();
        update();
    }
}

void SplineView::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Shift)
    {
        insertIndex = -1;
        update();
    }
}

void SplineView::deselect()
{
    if(selectedIndex == -1)
    {
        return;
    }
    selectedIndex = -1;
    emit knotDeselected();
    update();
}

void SplineView::updateInsertState()
{
    insertPos = mapFromGlobal(QCursor::pos()) - rect().center();
    if(spline.size() == 0)
    {
        insertIndex = 0;
    }
    else
    {
        qreal distance;
        insertIndex = spline.findClosest(insertPos, distance);
    }
}
