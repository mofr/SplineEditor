#pragma once

#include "Spline.h"
#include "History.h"

#include <QWidget>
#include <QColor>

class SplineView : public QWidget
{
    Q_OBJECT
public:
    explicit SplineView(QWidget *parent = 0);

public slots:
    void undo();
    void redo();

    const Spline & getSpline() const;
    void setSpline(const Spline & spline);

    void updateSelectedKnot(Spline::Knot knot);

signals:
    void knotSelected(Spline::Knot knot);
    void knotDeselected();

protected:
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

private:
    void deselect();
    void updateInsertState();

private:
    Spline spline;
    History history;

    bool movingKnot = false;
    int hotIndex = -1; // none
    int selectedIndex = -1; // none
    int insertIndex = -1; // none
    QPointF insertPos;

    QColor backgroundColor = QColor(32, 32, 32);
    QColor gridColor = QColor(128, 128, 128, 64);
    QColor splineColor = QColor(182, 219, 73);

    static const int HOT_KNOT_RADIUS = 10;
};

