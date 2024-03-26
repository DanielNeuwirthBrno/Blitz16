#-------------------------------------------------
#
# Project created by QtCreator 2016-01-14T22:37:51
#
#-------------------------------------------------

QT       += core
QT       -= gui

TARGET = Blitz16

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

LIBS     += -lpdcurses

SOURCES  += main.cpp \
            board.cpp \
            airplane.cpp \
            base.cpp \
            game.cpp \
            draw.cpp \
            settings.cpp

HEADERS  += airplane.h \
            board.h \
            base.h \
            game.h \
            settings.h

OTHER_FILES += picture.txt
