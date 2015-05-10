#-------------------------------------------------
#
# Project created by QtCreator 2014-08-22T17:50:16
#
#-------------------------------------------------

QT       += core gui

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libusb
#win32: LIBS += -llibusb

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RadioController
TEMPLATE = app


SOURCES += main.cxx\
        mainwindow.cxx \
    opendevice.cxx \
    rtttlplayer.cxx

HEADERS  += mainwindow.hxx \
    opendevice.h \
    rtttl_notes.hxx \
    rtttlplayer.hxx

FORMS    += mainwindow.ui

win32: LIBS += -LC:/Users/akos/Downloads/libusb-win32-bin-1.2.6.0/libusb-win32-bin-1.2.6.0/lib/dynamic/gcc/ -lusb

INCLUDEPATH += C:/Users/akos/Downloads/libusb-win32-bin-1.2.6.0/libusb-win32-bin-1.2.6.0/include
DEPENDPATH += C:/Users/akos/Downloads/libusb-win32-bin-1.2.6.0/libusb-win32-bin-1.2.6.0/include
