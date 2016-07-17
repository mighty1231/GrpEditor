#-------------------------------------------------
#
# Project created by QtCreator 2016-06-10T11:34:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GrpEditor
TEMPLATE = app


SOURCES += main.cpp \
    data.cpp \
    widget/mainwindow.cpp \
    widget/palletewindow.cpp \
    widget/palletetablewidget.cpp \
    widget/grpconfigdialog.cpp \
    comp/grp.cpp \
    comp/mapping.cpp \
    comp/remapping.cpp \
    comp/wpe.cpp \
    comp/colorcycling.cpp

HEADERS  += globals.h \
    data.h \
    widget/mainwindow.h \
    widget/palletewindow.h \
    widget/palletetablewidget.h \
    widget/grpconfigdialog.h \
    comp/grp.h \
    comp/mapping.h \
    comp/remapping.h \
    comp/wpe.h \
    comp/colorcycling.h

FORMS    += form/mainwindow.ui \
    form/palletewindow.ui \
    form/grpconfigdialog.ui
