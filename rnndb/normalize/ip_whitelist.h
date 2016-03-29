// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <cstdint>
#include <string>
#include <map>
#include <vector>

using namespace std;

namespace ip_whitelist {

extern vector<string> whitelist_gpus;

void init();

void emit_groups_gk20a();
void emit_groups_gm20b();

struct group_t;
struct reg_t;
struct field_t;
struct constant_t;

struct constant_t {
	struct constant_spec_t {
		const char *def;
		const char *name;
		const char *emit;
		bool removed;
	};

	string def;
	string name;
	string emit;
	field_t *field;
    reg_t *reg;
    group_t *group;
	bool removed;
	constant_t(const constant_spec_t &cs) {
		def = cs.def ? cs.def : "";
		name = cs.name ? cs.name : "";
		emit = cs.emit ? cs.emit : "";
        group = 0; reg = 0; field = 0;
		removed = cs.removed;
	}
};

#define C_SPEC(n, x, e) new constant_t((constant_t::constant_spec_t){.name=#n,.def=#x,.emit=0,.removed=0})

//
// fields represent bit-extents within a register/offset/word
//
struct field_t {
	struct field_spec_t {
		const char *def;
		const char *name;
		uint32_t lowbit;
		uint32_t highbit;
		uint32_t shift;
		uint32_t mask;
		bool     indexed;
		uint32_t stride;
		uint32_t size;
		bool removed;
		//		const char *emit;
	};

	string def;
	string name;
	uint32_t lowbit;
	uint32_t highbit;
	uint32_t shift;
	uint32_t mask;
	bool     indexed;
	uint32_t stride;
	uint32_t size;
	bool removed;
	reg_t *reg;
    group_t *group;
	map<string, constant_t *> constants;
	map<string, constant_t *> deleted_constants;
	field_t(const field_spec_t &fs) {
		def = fs.def ? fs.def : "";
		name = fs.name ? fs.name : "";
		lowbit = fs.lowbit;
		highbit = fs.highbit;
		shift = fs.shift;
		mask = fs.mask;
		indexed = fs.indexed;
		stride= fs.stride;
		size = fs.size;
		//		emit = fs.emit ? fs.emit : "";
        group = 0; reg = 0;
		removed = fs.removed;
	}
};


#define F_SPEC(n, f, e)  new field_t((field_t::field_spec_t){.name=#n,.def=#f,.removed=0})
#define IF_SPEC(n, f, e) new field_t((field_t::field_spec_t){.name=#n,.def=#f,.removed=0})
#define WF_SPEC(n, f, e) new field_t((field_t::field_spec_t){.name=#n,.def=#f,.removed=0})
#define OF_SPEC(n, f, e) new field_t((field_t::field_spec_t){.name=#n,.def=#f,.removed=0})

//
// reg_t is used for registers of course, but also for offsets within
// memory objects (e.g. instance memory) and command streams (e.g.: 3d
// class methods).
//
struct reg_t {
	struct reg_spec_t {
		const char *def;
		const char *name;
		uint32_t offset;
		bool indexed;
		uint32_t stride;
		uint32_t size;
		bool removed;
		bool skip_symbol;
	};

	string def;
	string name;
	uint32_t offset;
	bool indexed;
	uint32_t stride;
	uint32_t size;
	group_t *group;
	bool removed;
	bool skip_symbol;
	reg_t(const reg_spec_t &rs) {
		def = rs.def ? rs.def : "";
		name = rs.name ? rs.name : "";
		indexed = rs.indexed;
		stride = rs.stride;
		size = rs.size;
		removed = rs.removed;
		skip_symbol = rs.skip_symbol;
	}
	virtual ~reg_t(){ }
	map<string, field_t *>    fields;
	map<string, field_t *>    deleted_fields;
    map<string, constant_t *> constants; // could be some
    map<string, constant_t *> deleted_constants; // could be some
};

#define S_SPEC(n, d)  new reg_t((reg_t::reg_spec_t){.name=#n,.def=#d,.indexed=0,.removed=0,.skip_symbol=1})
#define R_SPEC(n, r)  new reg_t((reg_t::reg_spec_t){.name=#n,.def=#r,.indexed=0,.removed=0,.skip_symbol=0})
#define IR_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name=#n,.def=#r,.indexed=1,.removed=0,.skip_symbol=0})
#define O_SPEC(n, o)  new reg_t((reg_t::reg_spec_t){.name=#n,.def=#o,.indexed=0,.removed=0,.skip_symbol=0})
#define W_SPEC(n, w)  new reg_t((reg_t::reg_spec_t){.name=#n,.def=#w,.indexed=0,.removed=0,.skip_symbol=0})

/*
 * The "D*" versions mark the register/word/offset as being deleted from the engine.
 * For those, any use by software is invalid.  So, no code will be emitted for it.
 * However, software may be using another engine's support to implmement support for *this* engine.
 * For that case, the db entry will be set to "offset ~0" to mark it invalid.
 */
#define DR_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name=#n,.def=#r,.removed=1,.skip_symbol=1})
#define DO_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name=#n,.def=#r,.removed=1,.skip_symbol=1})
#define DW_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name=#n,.def=#r,.removed=1,.skip_symbol=1})
#define DS_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name=#n,.def="",.removed=1,.skip_symbol=1})

//
// groups are register domains, specific classes (3d/2d, etc) and
// memory object sets (ramin, etc).  generally a group represents
// a specific scope within which registers/offsets belonging to
// it can't collide.
//
enum class group_type_e { register_domain, memory_object, command_class };
struct group_t {
	struct group_spec_t {
		const char *name;
		const char *def;
		group_type_e type;
		bool removed;
		bool skip_symbol;
	};
	bool removed;
	string name;
	string def;
	bool skip_symbol;
	group_t(const group_spec_t &gs) {
		name = gs.name ? gs.name : "";
		def = gs.def?gs.def:"";
		removed = gs.removed;
		skip_symbol = gs.skip_symbol;
	}
    map<string, reg_t *>      regs;      // lots of these (includes offsets, words)
    map<string, field_t *>    fields;    // typically empty
    map<string, constant_t *> constants; // could be some top-level constants

    map<string, reg_t *>      deleted_regs;
    map<string, field_t *>    deleted_fields;    // typically empty
    map<string, constant_t *> deleted_constants; // could be some top-level constants
};


#define G_SPEC(n, d) new group_t((group_t::group_spec_t){.name=#n,.def=#d,.removed=false,.skip_symbol=true})


void begin_group(group_t *);
void begin_scope(reg_t *);
void end_scope();
void emit_register(reg_t *);
void delete_register(reg_t *);
void end_register();
void emit_offset(reg_t *);
void emit_field(field_t *);
void end_field();
void emit_constant(constant_t *);
void end_group();

struct symbol_t {
	string name;
	group_t *group;
	reg_t *reg;
	field_t *field;
	constant_t *constant;
	symbol_t(const string &s) {
		name =s;
		group = 0; reg = 0; field = 0; constant = 0;
	}
};

map<string, group_t *>    * get_groups();
map<string, reg_t *>      * get_regs();
map<string, field_t *>    * get_fields();
map<string, constant_t *> * get_constants();

map<string, group_t *>    * get_deleted_groups();
map<string, reg_t *>      * get_deleted_regs();
map<string, field_t *>    * get_deleted_fields();
map<string, constant_t *> * get_deleted_constants();

extern map<string, bool>         symbol_whitelist;
extern map<string, reg_t *>      register_whitelist;
extern map<string, field_t *>    field_whitelist;
extern map<string, constant_t *> constant_whitelist;

extern map<string, reg_t *>      deleted_register_whitelist;
extern map<string, field_t *>    deleted_field_whitelist;
extern map<string, constant_t *> deleted_constant_whitelist;

extern multimap<string, ip_whitelist::group_t *>    chip_groups;
extern multimap<string, ip_whitelist::reg_t *>      chip_regs;
extern multimap<string, ip_whitelist::field_t *>    chip_fields;
extern multimap<string, ip_whitelist::constant_t *> chip_constants;

extern multimap<string, ip_whitelist::group_t *>    chip_deleted_groups;
extern multimap<string, ip_whitelist::reg_t *>      chip_deleted_regs;
extern multimap<string, ip_whitelist::field_t *>    chip_deleted_fields;
extern multimap<string, ip_whitelist::constant_t *> chip_deleted_constants;

} // ip_whitelist namespace

typedef multimap<string, ip_whitelist::group_t *>::iterator    wl_groups_iterator_t;
typedef multimap<string, ip_whitelist::reg_t *>::iterator      wl_regs_iterator_t;
typedef multimap<string, ip_whitelist::field_t *>::iterator    wl_fields_iterator_t;
typedef multimap<string, ip_whitelist::constant_t *>::iterator wl_constants_iterator_t;

typedef multimap<string, ip_whitelist::group_t *>::iterator    wl_deleted_groups_iterator_t;
typedef multimap<string, ip_whitelist::reg_t *>::iterator      wl_deleted_regs_iterator_t;
typedef multimap<string, ip_whitelist::field_t *>::iterator    wl_deleted_fields_iterator_t;
typedef multimap<string, ip_whitelist::constant_t *>::iterator wl_deleted_constants_iterator_t;


