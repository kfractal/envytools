TARGET = normalize_rnndb

DEFINES += NV_IPDB_GEN

# the expr lexer generates a ton of these...
QMAKE_CXXFLAGS += -Wno-deprecated-register

linux {
	CONFIG += USE_QT_REGEX
	DEFINES += USE_QT_REGEX
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

QMAKE_LEXFLAGS += --c++  --outfile lex.expr.c
#--header=expr_lex.h
LEXSOURCES += expr.l
#LIBS += -lfl
#SOURCES += expr.cpp
#expr.cpp : expr.l
# flex++ --c++ --outfile=FILE --yyclass=NAME --header-file=FILE expr.l
# 



