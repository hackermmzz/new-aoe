#-------------------------------------------------
#
# Project created by QtCreator 2023-12-07T19:37:39
#
#-------------------------------------------------

QT       += \
        core gui \
        multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = newAOE
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWidget.cpp \
    GameWidget.cpp \
    Coordinate.cpp \
    GlobalVariate.cpp \
    Map.cpp \
    Block.cpp \
    MoveObject.cpp \
    Building.cpp \
    Resource.cpp \
    Human.cpp \
    Animal.cpp \
    StaticRes.cpp \
    Player.cpp \
    Core.cpp \
    Farmer.cpp

HEADERS += \
        MainWidget.h \
    GameWidget.h \
    config.h \
    Coordinate.h \
    GlobalVariate.h \
    Map.h \
    Block.h \
    MoveObject.h \
    Building.h \
    Resource.h \
    Human.h \
    Animal.h \
    StaticRes.h \
    Player.h \
    Core.h \
    Farmer.h

FORMS += \
        MainWidget.ui \
    GameWidget.ui


