// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
//
// Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
// insert GPL v2
//
#pragma once

#include <string>
#include <map>

#include <QString>
#include <QDomDocument>
#include <QXmlStreamWriter>
#include <QFile>

#include "nv_drf.h"

struct field_spec;

struct constant_spec {
    const char *def;
    const char *name;
    uint32_t    value;
    field_spec *fs;
};

#define C_SPEC(n, x, e) (constant_spec){ .name = #n, .value = x, .def = #x }

struct register_spec;
struct field_spec {
    const char *def;
    const char *name;
    uint32_t lowbit;
    uint32_t highbit;
    uint32_t shift;
    uint32_t mask;
    bool     indexed;
    uint32_t stride;
    uint32_t size;
    register_spec *rs;
};

#define F_SPEC(n, f, e) (field_spec){ .name = #n, .indexed = false, \
            .lowbit = NV_FIELD_LOWBIT(f), .highbit = NV_FIELD_HIGHBIT(f), \
            .shift = NV_FIELD_SHIFT(f),  .mask = NV_FIELD_MASK(f), .def = #f}

#define IF_SPEC(n, f, e) (field_spec){ .name = #n, .indexed = true, \
            .lowbit = NV_FIELD_LOWBIT(f(0)), .highbit = NV_FIELD_HIGHBIT(f(0)), \
            .shift = NV_FIELD_SHIFT(f(0)),  .mask = NV_FIELD_MASK(f(0)), \
            .stride = NV_FIELD_SHIFT(f(1)) - NV_FIELD_SHIFT(f(0)), .def = #f}


#define WF_SPEC(n, f, e) (field_spec){ .name = #n, .indexed = false, \
            .lowbit = NV_FIELD_LOWBIT(f), .highbit = NV_FIELD_HIGHBIT(f), \
            .shift = NV_FIELD_SHIFT(f),  .mask = NV_FIELD_MASK(f)/*?*/, .def = #f}

#define OF_SPEC(n, f, e) (field_spec){ .name = #n, .indexed = false,  \
            .lowbit = NV_FIELD_LOWBIT(f), .highbit = NV_FIELD_HIGHBIT(f), \
            .shift = NV_FIELD_SHIFT(f),  .mask = NV_FIELD_MASK(f)/*?*/, .def = #f}

struct group_spec;
struct register_spec {
    const char *def;
    const char *name;
    int64_t offset;
    bool indexed;
    uint32_t stride;
    uint32_t size;
    group_spec *gs;
};

#define S_SPEC(n, d)    (register_spec){.name = #n, .offset = (uint32_t)~0,  .indexed = false, .def = #d }
#define R_SPEC(n, r) (register_spec){.name = #n, .offset = (r), .indexed = false, .def = #r}
#define IR_SPEC(n, r) (register_spec){.name = #n, .offset = r(0), \
            .indexed = true, .stride = r(1) - r(0),  .size = r##__SIZE_1, .def = #r}

#define O_SPEC(n, o)  (register_spec){.name = #n, .offset = (o), .indexed = false, .def = #o}
#define W_SPEC(n, w)  (register_spec){.name = #n, .offset = (w), .indexed = false, .def = #w}
/*
 * The "D*" versions mark the register/word/offset as being deleted from the engine.
 * For those, any use by software is invalid.  So, no code will be emitted for it.
 * However, software may be using another engine's support to implmement support for *this* engine.
 * For that case, the db entry will be set to "offset ~0" to mark it invalid.
 */
#define DR_SPEC(n, r) (register_spec){.name = #n, .offset = 0, .indexed = false, .def = #r}
#define DO_SPEC(n, r) (register_spec){.name = #n, .offset = 0,  .indexed = false, .def = #r}
#define DW_SPEC(n, r) (register_spec){.name = #n, .offset = 0,  .indexed = false, .def = #r}

struct group_spec {
    const char *name;
    const char *def;
	int64_t base;
	int64_t extent;
};


#define G_SPEC(g, d) (struct group_spec){.name = #g, .def = #d, .base = 1 ? d, .extent = 0 ? d }
#define GAUX_SPEC(g, d) (struct group_spec){.name = #g, .def = #d, .base = 0, .extent = 0 }

void begin_group(const group_spec &gs);
void begin_scope(const register_spec &rs);
void end_scope(void);
void emit_register(const register_spec &rs);
void delete_register(const register_spec &rs);
void end_register(void);
void emit_offset(const register_spec &rs);
void emit_field(const field_spec &fs);
void end_field(void);
void emit_constant(const constant_spec &cs);
void end_group(void);

struct group;
struct reg;
struct field;
struct constant;


struct group {
    QXmlStreamWriter stream;
	QDomDocument doc;
    QFile file;
    std::string name;
    std::map<std::string, constant *> constants;
    std::string def;
	uint64_t base;
	uint64_t extent;
    group(group_spec gs) {
		name = gs.def;
		// std::cout << "ctor group " << gs.name << "\n";
		name = gs.name ? gs.name : "";
		def = gs.def;
		base = gs.base;
		extent = gs.extent;
    }
};

struct reg { /* can't use "register" here. it's a keyword */
    std::string def;
    std::string name;
    uint32_t offset;
    bool indexed;
    uint32_t stride;
    uint32_t size;
    group_spec *gs;
    group *g;
    std::map<std::string, constant *> constants;
    reg(register_spec rs) {
        //std::cout << "ctor reg " << rs.name << "\n";
        def     = rs.def ? rs.def : "";
        name    = rs.name ? rs.name : "";
        offset  = rs.offset;
        indexed = rs.indexed;
        stride  = rs.stride;
        size    = rs.size;
        gs      = rs.gs;
    }
    std::map<std::string, field *> fields;
};

struct field {
    std::string def;
    std::string name;
    uint32_t lowbit;
    uint32_t highbit;
    uint32_t shift;
    uint32_t mask;
    bool     indexed;
    uint32_t stride;
    uint32_t size;
    register_spec *rs;
    reg *r;
    std::map<std::string, constant *> constants;
    field(field_spec fs) {
        //std::cout << "ctor field " << fs.name << "\n";
        def     = fs.def ? fs.def : "";
        name    = fs.name ? fs.name : "";
        lowbit  = fs.lowbit;
        highbit = fs.highbit;
        shift   = fs.shift;
        mask    = fs.mask;
        indexed = fs.indexed;
        stride  = fs.stride;
        size    = fs.size;
        rs      = fs.rs;
    }
};

struct constant {
    std::string def;
    std::string name;
    uint32_t value;
    field_spec *fs;
    field *f;
    constant(constant_spec cs) {
        def   = cs.def ? cs.def : "";
        name  = cs.name ? cs.name : "";
        value = cs.value;
        fs    = cs.fs;
    }
};


struct symbol {
    std::string name;
    group *    G;
    reg *      R;
    field *    F;
    constant * C;
    symbol(const std::string &s) {
        name = s;
        G = 0; R=0; F=0; C=0;
    }
};

typedef std::map<std::string, constant *> constant_db;
typedef std::map<std::string, field *>    field_db;
typedef std::map<std::string, group *>    group_db;
typedef std::map<std::string, reg *>      register_db;
typedef std::map<std::string, symbol *>   symbol_db;
typedef std::map<std::string, symbol *>::iterator symbol_db_iter;

typedef std::list<std::string> def_lines;

extern void emit_groups_gk20a();
extern void emit_groups_gm20b();
