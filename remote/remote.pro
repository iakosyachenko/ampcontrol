#-------------------------------------------------
#
# Project created by QtCreator 2016-08-04T10:19:01
#
#-------------------------------------------------

QT += core gui

!android {
    QT += serialport
}

android {
    QT += bluetooth
    CONFIG += mobility
    DEFINES += BLUETOOTH
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ampcontrol-m32-remote
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    btitem.cpp

HEADERS += \
    mainwindow.h \
    defines.h \
    btitem.h

FORMS += \
    mainwindow.ui \
    btitem.ui

!android {
    SOURCES += \
        setupdialog.cpp

    HEADERS += \
        setupdialog.h

    FORMS += \
        setupdialog.ui
}

android {
    SOURCES += \
        btsetupdialog.cpp

    HEADERS += \
        btsetupdialog.h

    FORMS += \
        btsetupdialog.ui
}

RESOURCES += \
    res.qrc \
    ts.qrc

TRANSLATIONS += \
    ts/remote_ru.ts

tr.commands = lupdate \"$$_PRO_FILE_\" && lrelease \"$$_PRO_FILE_\"

PRE_TARGETDEPS += tr
QMAKE_EXTRA_TARGETS += tr

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
