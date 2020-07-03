#-------------------------------------------------
#
# Project created by QtCreator 2018-12-14T14:26:19
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SignalSave2
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        maindialog.cpp \
    myo/myo.cpp \
    mcu/packets.cpp \
    math/recognizer.cpp \
    mcu/mcubion.cpp

HEADERS  += maindialog.h \
    myo/myo.h \
    include/myo/cxx/detail/ThrowOnError.hpp \
    include/myo/cxx/impl/Hub_impl.hpp \
    include/myo/cxx/impl/Myo_impl.hpp \
    include/myo/cxx/impl/Pose_impl.hpp \
    include/myo/cxx/DeviceListener.hpp \
    include/myo/cxx/Hub.hpp \
    include/myo/cxx/Myo.hpp \
    include/myo/cxx/Pose.hpp \
    include/myo/cxx/Quaternion.hpp \
    include/myo/cxx/Vector3.hpp \
    include/myo/libmyo/detail/visibility.h \
    include/myo/libmyo.h \
    include/myo/myo.hpp \
    mcu/packets.h \
    math/recognizer.h \
    mcu/mcubion.h

RC_ICONS += images/bion.ico

RESOURCES+= SignalSave2.qrc

INCLUDEPATH += include

LIBS += -L$${PWD}/lib/ -lmyo32

