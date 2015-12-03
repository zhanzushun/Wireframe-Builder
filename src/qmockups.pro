#-------------------------------------------------
#
# Project created by QtCreator 2010-08-26T14:44:09
#
#-------------------------------------------------

QT       += core gui xml widgets

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0

TARGET = WireframeBuilder
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    document.cpp \
    commands.cpp \
    flowlayout.cpp \
    palette.cpp \
    itemdata.cpp

HEADERS  += mainwindow.hxx \
    document.hxx \
    commands.h \
    flowlayout.h \
    palette.hxx \
    itemdata.hxx

FORMS    += mainwindow.ui \
    palette.ui

RESOURCES += \
    qmockups.qrc
