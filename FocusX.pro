#-------------------------------------------------
#
# Project created by QtCreator 2015-12-25T11:20:00
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FocusX
TEMPLATE = app


SOURCES += main.cpp\
        focusx.cpp

HEADERS  += focusx.h

#CONFIG += release
#QMAKE_LFLAGS += -static

#LIBS += -lQt5Widgets
#LIBS += -lQt5Sql
#LIBS += -lQt5Gui
#LIBS += -lQt5Core

DISTFILES += \
    icon.rc

RC_FILE = \
    icon.rc
