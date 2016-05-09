#
# Copyright (c) 2015-2016, NVIDIA CORPORATION. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

TARGET = normalize_rnndb

DEFINES += NV_IPDB_GEN

# the expr lexer generates a ton of these...
QMAKE_CXXFLAGS += -Wno-deprecated-register

linux {
	CONFIG += USE_QT_REGEX
	DEFINES += USE_QT_REGEX
	QMAKE_CXX = clang++
}

CONFIG += no_keywords /* because emit is used in gen_registers_**/
CONFIG += c++11
CONFIG += qt thread console
CONFIG += debug
QT += xmlpatterns

macx {
	CONFIG -= app_bundle
}

SOURCES += main.cpp
HEADERS += main.h

HEADERS += ip_def.h
SOURCES += ip_def.cpp

HEADERS += ip_whitelist.h
SOURCES += ip_whitelist.cpp

SOURCES += gk20a/gen_registers_gk20a.cpp
SOURCES += gm20b/gen_registers_gm20b.cpp

# kind of hacked to fit into qmake's flex rule (assumes .c)

QMAKE_LEXFLAGS += --c++  --outfile lex.expr.c --prefix expr
#--header=expr_lex.h
LEXSOURCES += expr.l
#LIBS += -lfl
#SOURCES += expr.cpp
#expr.cpp : expr.l
# flex++ --c++ --outfile=FILE --yyclass=NAME --header-file=FILE expr.l
# 



