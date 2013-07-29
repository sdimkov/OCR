#-------------------------------------------------
#
# Project created by QtCreator 2012-09-19T18:38:59
#
#-------------------------------------------------

QT       += core

TARGET = ocr
TEMPLATE = lib

DEFINES += OCR_LIBRARY

SOURCES += \
    lib/main.cpp \
    lib/ocr.cpp \
    lib/language.cpp \
    lib/image.cpp

HEADERS  += \
    lib/ocr.h \
    lib/language.h \
    lib/image.h

QMAKE_CXXFLAGS += -std=c++0x
