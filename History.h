#pragma once

#include "Spline.h"
#include <QStack>

class History
{
public:
    void push(const Spline & spline);
    void redo(Spline & spline);
    void undo(Spline & spline);
    void clear();

private:
    QStack<Spline> backStack;
    QStack<Spline> forwardStack;
};
