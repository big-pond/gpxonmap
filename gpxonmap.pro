#-------------------------------------------------
#
# Project created by QtCreator 2023-02-06T16:29:31
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gpxonmap
TEMPLATE = app
RC_FILE = gpxonmap.rc

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    colorrect.cpp \
    curtrkpos.cpp \
    datawdg.cpp \
    def.cpp \
    dtdelegate.cpp \
    ellipsoid.cpp \
    gcdelegate.cpp \
    gcrecalc.cpp \
    gcrecalcs.cpp \
    geocoordedit.cpp \
    geopoint.cpp \
    gpointwdg.cpp \
    gpx.cpp \
    gpxmodel.cpp \
    gpxreader.cpp \
    hidcoldlg.cpp \
    levelwdg.cpp \
    ltime.cpp \
    mapdescription.cpp \
    mapinfodlg.cpp \
    mapscene.cpp \
    mapview.cpp \
    mapwidget.cpp \
    movetodlg.cpp \
    pointeditdlg.cpp \
    projection.cpp \
    reportwdg.cpp \
    rubberband.cpp \
    scalewdg.cpp \
    tilereader.cpp \
    topntfrom.cpp \
    trackmodel.cpp \
    trackpoint.cpp \
    trackpropdlg.cpp \
    util.cpp \
    waypoint.cpp \
    wpteditdlg.cpp \
    wptmodel.cpp \
    wptmrk.cpp \
    settingsdlg.cpp \
    weatherreader.cpp \
    weaterdlg.cpp

HEADERS += \
    mainwindow.h \
    colorrect.h \
    curtrkpos.h \
    datawdg.h \
    def.h \
    dtdelegate.h \
    ellipsoid.h \
    gcdelegate.h \
    gcrecalc.h \
    gcrecalcs.h \
    geocoordedit.h \
    geopoint.h \
    gpointwdg.h \
    gpx.h \
    gpxmodel.h \
    gpxreader.h \
    hidcoldlg.h \
    levelwdg.h \
    ltime.h \
    mapdescription.h \
    mapinfodlg.h \
    mapscene.h \
    mapview.h \
    mapwidget.h \
    movetodlg.h \
    pointeditdlg.h \
    projection.h \
    reportwdg.h \
    rubberband.h \
    scalewdg.h \
    tilereader.h \
    topntfrom.h \
    trackmodel.h \
    trackpoint.h \
    trackpropdlg.h \
    util.h \
    waypoint.h \
    wpteditdlg.h \
    wptmodel.h \
    wptmrk.h \
    settingsdlg.h \
    weatherreader.h \
    weaterdlg.h

FORMS += \
    mainwindow.ui \
    datawdg.ui \
    settingsdlg.ui

RESOURCES += \
    gpxonmap.qrc
