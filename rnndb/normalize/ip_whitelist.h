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

namespace ip_whitelist {

extern std::vector<std::string> whitelist_gpus;

void init();

void emit_groups_gk20a();
void emit_groups_gm20b();

struct constant_t;
struct field_t;
struct reg_t;
struct group_t;

struct constant_t {
	struct constant_spec_t {
		const char *def;
		const char *name;
		//		uint32_t    value;
		const char *emit;
	};

	std::string def;
	std::string name;
	//	uint32_t value;
	std::string emit;
	field_t *field;
    reg_t *reg;
    group_t *group;
	constant_t(const constant_spec_t &cs) {
		def = cs.def ? cs.def : "";
		name = cs.name ? cs.name : "";
		//		value = cs.value;
		emit = cs.emit ? cs.emit : "";
        group = 0; reg = 0; field = 0;
	}
};

#define C_SPEC(n, x, e) new constant_t((constant_t::constant_spec_t){ .name = #n, .def = #x, .emit = 0 })

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
		const char *emit;
	};

	std::string def;
	std::string name;
	uint32_t lowbit;
	uint32_t highbit;
	uint32_t shift;
	uint32_t mask;
	bool     indexed;
	uint32_t stride;
	uint32_t size;
	std::string emit;
	reg_t *reg;
    group_t *group;
	std::map<std::string, constant_t *> constants;
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
		emit = fs.emit ? fs.emit : "";
        group = 0; reg = 0;
	}
};


#define F_SPEC(n, f, e)  new field_t((field_t::field_spec_t){ .name = #n, .def = #f, .emit = e })
#define IF_SPEC(n, f, e) new field_t((field_t::field_spec_t){ .name = #n, .def = #f, .emit = e })
#define WF_SPEC(n, f, e) new field_t((field_t::field_spec_t){ .name = #n, .def = #f, .emit = e })
#define OF_SPEC(n, f, e) new field_t((field_t::field_spec_t){ .name = #n, .def = #f, .emit = e })

struct reg_t {
	struct reg_spec_t {
		const char *def;
		const char *name;
		uint32_t offset;
		bool indexed;
		uint32_t stride;
		uint32_t size;
		const char *emit;
	};

	std::string def;
	std::string name;
	uint32_t offset;
	bool indexed;
	uint32_t stride;
	uint32_t size;
	group_t *group;
	std::string emit;
	reg_t(const reg_spec_t &rs) {
		def = rs.def ? rs.def : "";
		name = rs.name ? rs.name : "";
		offset = rs.offset;
		indexed = rs.indexed;
		stride = rs.stride;
		size = rs.size;
		emit = rs.emit ? rs.emit : "";
	}
	std::map<std::string, field_t *>    fields;
    std::map<std::string, constant_t *> constants; // could be some

};


#define S_SPEC(n)     new reg_t((reg_t::reg_spec_t){.name = #n, .def = "", .emit = "",  .indexed = false })
#define R_SPEC(n, r)  new reg_t((reg_t::reg_spec_t){.name = #n, .def = #r, .emit = "r", .indexed = false })
#define IR_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name = #n, .def = #r, .emit = "r", .indexed = true  })

#define O_SPEC(n, o)  new reg_t((reg_t::reg_spec_t){.name = #n, .def = #o, .emit = "o", .indexed = false })
#define W_SPEC(n, w)  new reg_t((reg_t::reg_spec_t){.name = #n, .def = #w, .emit = "w", .indexed = false })
/*
 * The "D*" versions mark the register/word/offset as being deleted from the engine.
 * For those, any use by software is invalid.  So, no code will be emitted for it.
 * However, software may be using another engine's support to implmement support for *this* engine.
 * For that case, the db entry will be set to "offset ~0" to mark it invalid.
 */
#define DR_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name = #n, .def = #r, .emit = "rx"})
#define DO_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name = #n, .def = #r, .emit = "ox"})
#define DW_SPEC(n, r) new reg_t((reg_t::reg_spec_t){.name = #n, .def = #r, .emit = "wx"})

struct group_t {
	struct group_spec_t {
		const char *name;
	};
	std::string name;
	group_t(const group_spec_t &gs) {
		name = gs.name ? gs.name : "";
	}
    std::map<std::string, reg_t *>      regs;      // lots of these (includes offsets, words)
    std::map<std::string, field_t *>    fields;    // typically empty
    std::map<std::string, constant_t *> constants; // could be some top-level constants
};


#define G_SPEC(n) new group_t((group_t::group_spec_t){.name = #n})


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
	std::string name;
	group_t *group;
	reg_t *reg;
	field_t *field;
	constant_t *constant;
	symbol_t(const std::string &s) {
		name =s;
		group = 0; reg = 0; field = 0; constant = 0;
	}
};

#if 0
typedef std::map<std::string, constant_t *> constant_db_t;
typedef std::map<std::string, field_t *>    field_db_t;
typedef std::map<std::string, group_t *>    group_db_t;
typedef std::map<std::string, reg_t *>      reg_db_t;
typedef std::map<std::string, symbol_t *>   symbol_db_t;
typedef std::map<std::string, symbol_t *>::iterator symbol_db_iter_t;
#endif

std::map<std::string, group_t *>    * get_groups();
std::map<std::string, reg_t *>      * get_regs();
std::map<std::string, reg_t *>      * get_offsets();
std::map<std::string, reg_t *>      * get_words();
std::map<std::string, reg_t *>      * get_deleted();
std::map<std::string, field_t *>    * get_fields();
std::map<std::string, constant_t *> * get_constants();

extern std::map<std::string, bool>         symbol_whitelist;
extern std::map<std::string, reg_t *>      register_whitelist;
extern std::map<std::string, field_t *>    field_whitelist;
extern std::map<std::string, constant_t *> constant_whitelist;

extern std::multimap<std::string, ip_whitelist::group_t *>    chip_groups;
extern std::multimap<std::string, ip_whitelist::reg_t *>      chip_regs;
extern std::multimap<std::string, ip_whitelist::field_t *>    chip_fields;
extern std::multimap<std::string, ip_whitelist::constant_t *> chip_constants;


typedef std::pair <std::multimap<std::string,ip_whitelist::group_t*>::iterator,
				   std::multimap<std::string,ip_whitelist::group_t*>::iterator>    group_mm_it_t;
typedef std::pair <std::multimap<std::string,ip_whitelist::reg_t*>::iterator,
				   std::multimap<std::string,ip_whitelist::reg_t*>::iterator>      reg_mm_it_t;
typedef std::pair <std::multimap<std::string,ip_whitelist::field_t*>::iterator,
				   std::multimap<std::string,ip_whitelist::field_t*>::iterator>    field_mm_it_t;
typedef std::pair <std::multimap<std::string,ip_whitelist::constant_t*>::iterator,
				   std::multimap<std::string,ip_whitelist::constant_t*>::iterator> constant_mm_it_t;


//typedef std::multimap<std::string, ip_whitelist::group_t *>::iterator    group_mm_it_t;
//typedef std::multimap<std::string, ip_whitelist::reg_t *>::iterator      reg_mm_it_t;
//typedef std::multimap<std::string, ip_whitelist::field_t *>::iterator    field_mm_it_t;
//typedef std::multimap<std::string, ip_whitelist::constant_t *>::iterator constant_mm_it_t;

#if 0
std::list<std::string> get_group_registers(string &group);
std::list<std::string> get_register_fields(string &symbol);
std::list<std::string> get_field_constants(string &symbol);
#endif

}
