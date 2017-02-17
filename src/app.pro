
TEMPLATE = app
TARGET = karuta-server

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QT -= core gui

MODULES = core network util

SOURCES = main.cpp
HEADERS = global.h

for (module, MODULES) {
	INCLUDEPATH += $$module
	SOURCES += $$files($$module/*.cpp)
	HEADERS += $$files($$module/*.h)
}
