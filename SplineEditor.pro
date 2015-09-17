#-------------------------------------------------
#
# Project created by QtCreator 2015-09-02T15:56:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SplineEditor
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11 -Werror=return-type

SOURCES += main.cpp\
        MainWindow.cpp \
    History.cpp \
    Spline.cpp \
    SplineView.cpp

HEADERS  += MainWindow.h \
    History.h \
    Spline.h \
    SplineView.h

FORMS    += MainWindow.ui
