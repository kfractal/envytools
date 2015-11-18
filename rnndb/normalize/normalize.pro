TARGET = normalize_rnndb

CONFIG += c++11
CONFIG += qt thread console
CONFIG += debug

macx {
	CONFIG -= app_bundle
}

#HEADERS += cmdline.h
#HEADERS += qtcmd.h
SOURCES += main.cpp
HEADERS += main.h
#SOURCES += main_app.cpp
#HEADERS += ssh2_cmd.h
#SOURCES += ssh2_cmd.c

