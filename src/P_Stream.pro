#-------------------------------------------------
#
# Project created by QtCreator 2011-04-11T19:48:10
#
#-------------------------------------------------

QT       += core gui\
            sql

TARGET = P_Stream
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    pstream.cpp \
    frame.cpp

HEADERS  += mainwindow.h \
    pstream.h \
    frame.h

FORMS    += mainwindow.ui \
    frame.ui

RESOURCES += \
    res.qrc

LIBS += /usr/lib/libpcap.so.1.1.1
