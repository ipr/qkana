#-------------------------------------------------
#
# Project created by QtCreator 2011-08-24T22:22:26
#
#-------------------------------------------------

QT       += core gui

TARGET = qkana
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    LanguageData.cpp

HEADERS  += mainwindow.h \
    LanguageData.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:/code/db-5.2.28/build_windows/

CONFIG (debug, debug|release) 
{
    LIBS += -LC:/code/db-5.2.28/build_windows/Win32/Debug/
    LIBS += -llibdb52d
} 

CONFIG (release, debug|release) 
{
    LIBS += -LC:/code/db-5.2.28/build_windows/Win32/Release/
    LIBS += -llibdb52
}

