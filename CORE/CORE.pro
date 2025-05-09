#-------------------------------------------------
#
# Project created by QtCreator 2024-11-30T23:47:36
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CORE
DESTDIR = ../../bin
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

CONFIG += c++11

SOURCES += \
    chageattr.cpp \
        main.cpp \
        mainwindow.cpp \
        mygraphicsview.cpp \
        mywidget.cpp \
        share/cpp/map.pb.cc \


HEADERS += \
    chageattr.h \
        mainwindow.h \
        mygraphicsview.h \
        mywidget.h \
        share/cpp/map.pb.h \


FORMS += \
        chageattr.ui \
        mainwindow.ui \
        mywidget.ui

INCLUDEPATH +=  \
    share/cpp \

LIBS += \
    -L/usr/local/lib \
    -lprotobuf \

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    share/auto.sh \
    share/map.proto
