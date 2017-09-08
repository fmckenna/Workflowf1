TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    OpenSeesPreprocessor.h

SOURCES += \
    mainPreprocessor.cpp \
    OpenSeesPreprocessor.cpp

win32 {
INCLUDEPATH += $$PWD\..\..\include
LIBS += -L$$PWD\..\..\lib -llibjansson-4
}
