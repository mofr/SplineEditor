#include "History.h"

void History::push(const Spline &spline)
{
    backStack.push(spline);
    forwardStack.clear();
}

void History::redo(Spline &spline)
{
    if(forwardStack.size() > 0)
    {
        backStack.push(spline);
        spline = forwardStack.top();
        forwardStack.pop();
    }
}

void History::undo(Spline &spline)
{
    if(backStack.size() > 0)
    {
        forwardStack.push(spline);
        spline = backStack.top();
        backStack.pop();
    }
}

void History::clear()
{
    backStack.clear();
    forwardStack.clear();
}
