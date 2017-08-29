TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    HazusLossEstimator.cpp \
    Building.cpp \
    FragilityCurve.cpp \
    NormativeQty.cpp \
    Stat.cpp \
    include/inifile.cpp \
    include/tinyxml2.cpp \
    Component.cpp \
    NormativeQtyStr.cpp

HEADERS += \
    HazusLossEstimator.h \
    Building.h \
    FragilityCurve.h \
    NormativeQty.h \
    Stat.h \
    inifile.h \
    inifile.h \
    tinyxml2.h \
    Component.h \
    csv.h \
    NormativeQtyStr.h

DISTFILES += \
    README.md \
    data/settings.ini

unix {
INCLUDEPATH += /usr/local/include

LIBS += /usr/local/lib/libjansson.a
}

win32 {
INCLUDEPATH += $$PWD\..\..\include
LIBS += -L$$PWD\..\..\lib -llibjansson-4
}
