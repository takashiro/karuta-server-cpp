
TEMPLATE = app
TARGET = unittest

CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QT -= core gui

SRCDIR = $$dirname(PWD)/src
MODULES = core network util

INCLUDEPATH = $$SRCDIR
for (module, MODULES): INCLUDEPATH += $$SRCDIR/$$module

SOURCES = $$files(*.cpp)
HEADERS = $$files(*.h)
