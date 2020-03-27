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
    widget/mainwindow.cpp \
    widget/palletewindow.cpp \
    widget/palletetablewidget.cpp \
    widget/grpconfigdialog.cpp \
    component/grp.cpp \
    component/mapping.cpp \
    component/remapping.cpp \
    component/wpe.cpp \
    component/colorcycling.cpp \
    command/createframecommand.cpp \
    component/grpframe.cpp \
    component/loader.cpp \
    command/deleteframecommand.cpp \
    command/swapframecommand.cpp \
    command/dragframecommand.cpp \
    component/doublefacedrect.cpp

HEADERS  += globals.h \
    widget/mainwindow.h \
    widget/palletewindow.h \
    widget/palletetablewidget.h \
    widget/grpconfigdialog.h \
    component/grp.h \
    component/mapping.h \
    component/remapping.h \
    component/wpe.h \
    component/colorcycling.h \
    command/createframecommand.h \
    component/grpframe.h \
    component/loader.h \
    command/deleteframecommand.h \
    command/swapframecommand.h \
    command/dragframecommand.h \
    component/doublefacedrect.h

FORMS    += form/mainwindow.ui \
    form/palletewindow.ui \
    form/grpconfigdialog.ui

copydata.commands = $(COPY_DIR) $$shell_quote($$shell_path($$PWD/../data)) \
        $$shell_quote($$shell_path($${OUT_PWD}/data))
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata
