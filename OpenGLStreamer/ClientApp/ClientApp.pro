include(../config.pri)

QT       += core websockets
QT       -= gui

TARGET  = ClientApp
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/OpenGLClient.cpp

HEADERS += \
    src/OpenGLClient.h
