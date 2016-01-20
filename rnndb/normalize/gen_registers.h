// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
/*
 *  >insert nvidia open source copyright<
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

void emit_groups_gk20a();
void emit_groups_gm20b();

struct field_spec_t;
struct group_spec_t;
struct reg_spec_t;

struct constant_spec_t {
	const char *def;
	const char *name;
	uint32_t    value;
	const char *emit;
	field_spec_t *fs;
};

#define C_SPEC(n, x, e) (constant_spec_t){ .name = #n, .def = #x, .emit = 0 }

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
	reg_spec_t *rs;
};

#define F_SPEC(n, f, e) (field_spec_t) { .name = #n, .def = #f, .emit = e }
#define IF_SPEC(n, f, e) (field_spec_t){ .name = #n, .def = #f, .emit = e }
#define WF_SPEC(n, f, e) (field_spec_t){ .name = #n, .def = #f, .emit = e }
#define OF_SPEC(n, f, e) (field_spec_t){ .name = #n, .def = #f, .emit = e }

struct reg_spec_t {
	const char *def;
	const char *name;
	uint32_t offset;
	bool indexed;
	uint32_t stride;
	uint32_t size;
	group_spec_t *gs;
	const char *emit;
};

#define S_SPEC(n)     (reg_spec_t){.name = #n, .def = "",  .emit = "", .indexed = false }
#define R_SPEC(n, r)  (reg_spec_t){.name = #n, .def = #r, .emit = "r", .indexed = false }
#define IR_SPEC(n, r) (reg_spec_t){.name = #n, .def = #r, .emit = "r", .indexed = true  }

#define O_SPEC(n, o)  (reg_spec_t){.name = #n, .def = #o, .emit = "o", .indexed = false }
#define W_SPEC(n, w)  (reg_spec_t){.name = #n, .def = #w, .emit = "w", .indexed = false }
/*
 * The "D*" versions mark the register/word/offset as being deleted from the engine.
 * For those, any use by software is invalid.  So, no code will be emitted for it.
 * However, software may be using another engine's support to implmement support for *this* engine.
 * For that case, the db entry will be set to "offset ~0" to mark it invalid.
 */
#define DR_SPEC(n, r) (reg_spec_t){.name = #n, .def = #r, .emit = "rx"}
#define DO_SPEC(n, r) (reg_spec_t){.name = #n, .def = #r, .emit = "ox"}
#define DW_SPEC(n, r) (reg_spec_t){.name = #n, .def = #r, .emit = "wx"}

struct group_spec_t {
	FILE *f;
	const char *name;
};


#define G_SPEC(n) (group_spec_t){.name = #n}


void begin_group(group_spec_t gs);
void begin_scope(reg_spec_t rs);
void end_scope();
void emit_register(reg_spec_t rs);
void delete_register(reg_spec_t rs);
void end_register();
void emit_offset(reg_spec_t rs);
void emit_field(field_spec_t fs);
void end_field();
void emit_constant(constant_spec_t cs);
void end_group();

#include <string>
#include <map>

struct reg_t;
struct field_t;
struct constant_t;

struct group_t {
	std::string name;
	group_t(group_spec_t gs) {
		//std::cout << "ctor group " << gs.name << "\n";
		name = gs.name ? gs.name : "";
	}
    std::map<std::string, reg_t *>      regs;      // lots of these (includes offsets, words)
    std::map<std::string, field_t *>    fields;    // typically empty
    std::map<std::string, constant_t *> constants; // could be some top-level constants
};

struct reg_t {
	std::string def;
	std::string name;
	uint32_t offset;
	bool indexed;
	uint32_t stride;
	uint32_t size;
	group_spec_t *gs;
	group_t *group;
	std::string emit;
	reg_t(reg_spec_t rs) {
		//std::cout << "ctor reg " << rs.name << "\n";
		def = rs.def ? rs.def : "";
		name = rs.name ? rs.name : "";
		offset = rs.offset;
		indexed = rs.indexed;
		stride = rs.stride;
		size = rs.size;
		gs = rs.gs;
		emit = rs.emit ? rs.emit : "";
	}
	std::map<std::string, field_t *>    fields;
    std::map<std::string, constant_t *> constants; // could be some

};

struct field_t {
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
	reg_spec_t *rs;
	reg_t *reg;
    group_t *group;
	std::map<std::string, constant_t *> constants;
	field_t(field_spec_t fs) {
		//std::cout << "ctor field " << fs.name << "\n";
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
		rs = fs.rs;
        group = 0; reg = 0;
	}
};

struct constant_t {
	std::string def;
	std::string name;
	uint32_t value;
	std::string emit;
	field_spec_t *fs;
	field_t *field;
    reg_t *reg;
    group_t *group;
	constant_t(constant_spec_t cs) {
		def = cs.def ? cs.def : "";
		name = cs.name ? cs.name : "";
		value = cs.value;
		emit = cs.emit ? cs.emit : "";
		fs = cs.fs;
        group = 0; reg = 0; field = 0;
	}
};


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

typedef std::map<std::string, constant_t *> constant_db_t;
typedef std::map<std::string, field_t *>    field_db_t;
typedef std::map<std::string, group_t *>    group_db_t;
typedef std::map<std::string, reg_t *>      reg_db_t;
typedef std::map<std::string, symbol_t *>   symbol_db_t;
typedef std::map<std::string, symbol_t *>::iterator symbol_db_iter_t;

std::map<std::string, group_t *> *get_groups();
std::map<std::string, reg_t *> *get_regs();
std::map<std::string, field_t *> *get_fields();
std::map<std::string, constant_t *> *get_constants();

