include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG += thread
CONFIG += qt

HEADERS += \
    ../app/engine.h \
    test_engine.h

SOURCES += \
    ../app/engine.cpp \
    test_engine.cpp \
    tst_engine.cpp

RESOURCES +=
