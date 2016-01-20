// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
/*
 *  >insert nvidia open source copyright<
 */

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <iostream>

using namespace std;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>
#include <regex.h>

#include "gen_registers.h"
static char inline_name[128]; /* scratch */
static int inline_name_size = 128;

std::map<std::string, bool> symbol_whitelist { };

static void * at_scope = 0;

static group_spec_t    g;
static reg_spec_t      r;
static field_spec_t    f;
static constant_spec_t c;
static reg_spec_t      r_null;
static field_spec_t    f_null;

static group_t    *G;
static reg_t      *R;
static field_t    *F;
static constant_t *C;
static symbol_t   *S;


static std::map<std::string, group_t *> *groups = new std::map<std::string, group_t *>();
static std::map<std::string, reg_t *> *regs = new std::map<std::string, reg_t *>();
static std::map<std::string, field_t *> *fields = new std::map<std::string, field_t *>();
static std::map<std::string, constant_t *> *constants = new std::map<std::string, constant_t *>();

void begin_group(group_spec_t gs)
{
	if (g.f) {
		fprintf(stderr, "must close %s group before starting %s group\n",
				g.name, gs.name);
		exit(1);
	}

	g = gs;

	(*groups)[gs.name] = G = new group_t(g);
	at_scope = G;
	R = 0;
	F = 0;
	S = 0;
	r = r_null;
	f = f_null;

	return;
}


void end_group()
{
	at_scope = 0;
	G = 0; R = 0; F = 0; C = 0; S = 0;
	g.name = 0;
}


void end_register()
{
	at_scope = G;
	R = 0; F = 0; C = 0; S = 0;
	r = r_null;
	f = f_null;
}

void end_scope()
{
	end_register();
}

void end_field()
{
	if ( R )
		at_scope = R;
	else 
		at_scope = G;
	F = 0; C = 0; S = 0;
	f = f_null;
}

std::string emit_scope()
{
	// group is always in scope.  shortcuts may be taken elsewhere though
	std::string scope = std::string(g.name);
	if (r.name && strcmp(r.name, "")) scope += "_" + std::string(r.name);
	//symbol_whitelist[scope] = true;
	return scope;
}


void emit_register(reg_spec_t rs)
{
	const char *to_emit = rs.emit;
	bool emit_r = false, emit_w = false, emit_o = false, emit_x = false;

	if ( to_emit && strlen(to_emit) ) {
		emit_r   = !!strstr(to_emit, "r");
		emit_w   = !!strstr(to_emit, "w");
		emit_o   = !!strstr(to_emit, "o");
		emit_x   = !!strstr(to_emit, "x");
	}

	if (rs.def)
		symbol_whitelist[rs.def] = true;

	r = rs;
	f = f_null;

	R = new reg_t(rs);
	R->group = G;
	std::string name = emit_scope();
	G->regs[ rs.def ] = R;
	at_scope = R;
	R->indexed = rs.indexed;

	if ( emit_r )
		(*regs)[rs.def] = R;

}

void emit_offset(reg_spec_t rs)
{
	emit_register(rs);
}

void delete_register(reg_spec_t rs)
{
	// the spec holds the info about deleting...
	emit_register(rs);
}


/* begin scope is just setting up a register which won't be emitted */
void begin_scope(reg_spec_t rs)
{
	emit_register(rs);
}



void emit_field(field_spec_t fs)
{
	f = fs;
	if (fs.def)
		symbol_whitelist[fs.def] = true;

	std::string reg_name = emit_scope();
	std::string field_name = reg_name + "_"+ std::string(f.name);


	bool emit_size, emit_mask, emit_place, emit_get, emit_word, emit_offset;
	const char *to_emit = fs.emit;


	F = new field_t(f);
	if ( R ) {
		R->fields[fs.def] = F;
		F->reg = R;
	} else if (G) {
		G->fields[fs.def] = F;
		F->group = G;
	} else { 
		cerr << "wtf1";
		exit(1);
	}
	at_scope = F;

	(*fields)[fs.def] = F;
}


void emit_constant(constant_spec_t cs)
{

	if ( cs.def )
		symbol_whitelist[cs.def] = true;

	bool emit_place, emit_get;
	const char *to_emit = cs.emit;
	c = cs;

	std::string scope = emit_scope();
	std::string name = scope + "_" + c.name;

	C = new constant_t(cs);

	if ( at_scope == G ) {
		G->constants[cs.def] = C;
		C->group = G;
	} else if (at_scope == R ) {
		R->constants[cs.def] = C;
		C->group = G;
		C->reg = R;
	} else if (at_scope == F ) {
		F->constants[cs.def] = C;
		C->group = G;
		C->reg = R;
		C->field = F;
	} else {
		std::cerr << "what scope is [" << name << "] at?\n";
		exit(1);
	}
	(*constants)[cs.def] = C;
}


std::map<std::string, group_t *> *get_groups()
{
	return groups;
	groups = new std::map<std::string, group_t*>();
}
std::map<std::string, reg_t *> *get_regs()
{
	return regs;
	regs = new std::map<std::string, reg_t *>();
}
std::map<std::string, field_t *> *get_fields()
{
	return fields;
	fields = new std::map<std::string, field_t *>();
}
std::map<std::string, constant_t *> *get_constants()
{
	return constants;
	constants = new std::map<std::string, constant_t *>();
}


