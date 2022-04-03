QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    engine.cpp \
    log.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    engine.h \
    log.h \
    mainwindow.h

FORMS += \
    mainwindow.ui
