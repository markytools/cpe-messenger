#-------------------------------------------------
#
# Project created by QtCreator 2018-06-16T22:23:16
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       += multimedia

android {
    QT += quick androidextras
    ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources


OTHER_FILES += \
    android-sources/src/org/qtproject/example/notification/NotificationClient.java \
    android-sources/AndroidManifest.xml
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CPEMessenger
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


SOURCES += \
        main.cpp \
    buddies.cpp \
    cpemessenger.cpp \
    groupnotifications.cpp \
    groupselection.cpp \
    jackpots.cpp \
    jackpotshome.cpp \
    jackpotsroom.cpp \
    joingroup.cpp \
    login.cpp \
    messengerhome.cpp \
    myfiles.cpp \
    newgroup.cpp \
    passwordrecovery.cpp \
    qwidgetmanager.cpp \
    register.cpp \
    registersuccess.cpp \
    uploadfile.cpp \
    userinfo.cpp \
    utils.cpp \
    settings.cpp

HEADERS += \
    buddies.h \
    cpemessenger.h \
    groupnotifications.h \
    groupselection.h \
    jackpots.h \
    jackpotshome.h \
    jackpotsroom.h \
    joingroup.h \
    login.h \
    messengerhome.h \
    myfiles.h \
    newgroup.h \
    passwordrecovery.h \
    qwidgetmanager.h \
    register.h \
    registersuccess.h \
    uploadfile.h \
    userinfo.h \
    utils.h \
    settings.h

FORMS += \
        mainwindow.ui \
    buddies.ui \
    cpemessenger.ui \
    groupnotifications.ui \
    groupselection.ui \
    jackpots.ui \
    jackpotshome.ui \
    jackpotsroom.ui \
    joingroup.ui \
    login.ui \
    mainwindow.ui \
    messengerhome.ui \
    myfiles.ui \
    newgroup.ui \
    passwordrecovery.ui \
    register.ui \
    registersuccess.ui \
    uploadfile.ui

CONFIG += mobility
CONFIG += c++11
MOBILITY =

RESOURCES += \
    resources.qrc \
    resources.qrc
