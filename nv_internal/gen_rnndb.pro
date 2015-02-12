#
# Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
# insert GPL v2
#

TARGET = gen_rnndb

#
# note: uses c++11 purposefully in multiple places.
# and, tested with qt 5.3 only.  clang works great.
#
QMAKE_CXX=clang
CONFIG += c++11 debug
QT += xml sql

CONFIG += gk20a gm20b

NVIDIA_TEGRA       = /home/kadams/u/main/vendor/nvidia/tegra
CHIPS_A_PATH       = /home/kadams/u/sw/dev/gpu_drv/chips_a

IP_CHECK_PATH      = $$NVIDIA_TEGRA/core-private/tools/ip_check

INCLUDEPATH += $$CHIPS_A_PATH/drivers/common/inc/hwref/maxwell
INCLUDEPATH += $$CHIPS_A_PATH/drivers/common/inc/hwref/kepler

SOURCES += gk20a/gen_rnndb_gk20a.cpp
SOURCES += gm20b/gen_rnndb_gm20b.cpp

HEADERS += gen_rnndb.h
SOURCES += gen_rnndb.cpp

# gengetopt -i ../gen_rnndb.ggo  -F gen_rnndb_cmdline
#     TBD: generate qmake-foo to properly create these:
HEADERS += gen_rnndb_cmdline.h
SOURCES += gen_rnndb_cmdline.c

#
