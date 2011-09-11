#-------------------------------------------------
#
# Project created by QtCreator 2011-08-18T11:57:41
#
#-------------------------------------------------

QT       += core gui

QT       += network
QT       += xml
QT       += phonon


TARGET = YoutubeViewer
TEMPLATE = app


SOURCES += src/image.cpp \
    src/InfoManager.cpp \
    src/PhononYoutubePlayer.cpp \
    src/VideoStreamer.cpp \
    src/VideoController.cpp
SOURCES += src/Info.cpp
SOURCES += src/ListElement.cpp
SOURCES += src/Log.cpp
SOURCES += src/main.cpp
SOURCES += src/MainWindow.cpp
SOURCES += src/Parser3.cpp
SOURCES += src/SettingManager.cpp
SOURCES += src/ThumbnailsHandler2.cpp

HEADERS  += include/image.h \
    include/InfoManager.h \
    include/PhononYoutubePlayer.h \
    include/VideoStreamer.h \
    include/ColorsFuncs.h \
    include/KeyList.h \
    include/VideoController.h
HEADERS  += include/Info.h
HEADERS  += include/ListElement.h
HEADERS  += include/Log.h
HEADERS  += include/MainWindow.h
HEADERS  += include/Parser3.h
HEADERS  += include/SettingManager.h
HEADERS  += include/ThumbnailsHandler2.h

FORMS    += ui/mainwindow.ui


INCLUDEPATH += include/

RESOURCES += rc.qrc

OTHER_FILES += \
    Resources/YD_Icon.png \
    Resources/Prev.png \
    Resources/Play.png \
    Resources/Pause.png \
    Resources/Next.png \
    Resources/image-missing.jpg
