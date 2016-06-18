#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T11:34:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GrpEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    globals.cpp \
    palletewindow.cpp \
    wpe.cpp \
    mapping.cpp \
    data.cpp \
    grp.cpp \
    remapping.cpp \
    colorcycling.cpp \
    palletetablewidget.cpp

HEADERS  += mainwindow.h \
    globals.h \
    palletewindow.h \
    wpe.h \
    mapping.h \
    data.h \
    grp.h \
    remapping.h \
    colorcycling.h \
    palletetablewidget.h

FORMS    += mainwindow.ui \
    palletewindow.ui
