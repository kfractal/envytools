TARGET = normalize_rnndb

CONFIG += c++11
CONFIG += qt thread console
CONFIG += debug
QT += xmlpatterns

macx {
	CONFIG -= app_bundle
}

SOURCES += main.cpp
HEADERS += main.h
