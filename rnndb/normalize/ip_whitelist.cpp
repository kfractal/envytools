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

#include <iostream>
#include <string>
#include <map>

using namespace std;

#include "ip_whitelist.h"

namespace ip_whitelist {

map<string, bool> symbol_whitelist;
vector<string> whitelist_gpus{ "gk20a", "gm20b" };
void emit_groups_gk20a();
void emit_groups_gm20b();


static void init_gpu_symbol_whitelist(const string &wl_gpu);

void init()
{
	for ( auto wi : whitelist_gpus ) {
		init_gpu_symbol_whitelist(wi);
	}
}

static void init_gpu_symbol_whitelist(const string &wl_gpu)
{
	if ( wl_gpu == "gk20a" ){
		ip_whitelist::emit_groups_gk20a();
	} else {
		ip_whitelist::emit_groups_gm20b();
	}
	map<string, ip_whitelist::group_t *>    * groups    = ip_whitelist::get_groups();
	map<string, ip_whitelist::reg_t *>      * regs      = ip_whitelist::get_regs();
	map<string, ip_whitelist::field_t *>    * fields    = ip_whitelist::get_fields();
	map<string, ip_whitelist::constant_t *> * constants = ip_whitelist::get_constants();

	for (auto g : *groups)    chip_groups.   insert(g);
	for (auto r : *regs)      chip_regs.     insert(r);
	for (auto f : *fields)    chip_fields.   insert(f);
	for (auto c : *constants) chip_constants.insert(c);
}

multimap<string, ip_whitelist::group_t *>    chip_groups;
multimap<string, ip_whitelist::reg_t *>      chip_regs;
multimap<string, ip_whitelist::field_t *>    chip_fields;
multimap<string, ip_whitelist::constant_t *> chip_constants;


// current scoping state
static void * at_scope = 0;
static group_t    *G;
static reg_t      *R;
static field_t    *F;
static constant_t *C;
static symbol_t   *S;

static map<string, group_t *>    * groups    = new map<string, group_t *>   ();
static map<string, reg_t *>      * regs      = new map<string, reg_t *>     ();
static map<string, reg_t *>      * deleted   = new map<string, reg_t *>     ();
static map<string, reg_t *>      * offsets   = new map<string, reg_t *>     ();
static map<string, reg_t *>      * words     = new map<string, reg_t *>     ();
static map<string, field_t *>    * fields    = new map<string, field_t *>   ();
static map<string, constant_t *> * constants = new map<string, constant_t *>();

void begin_group(group_t *g)
{
	if (G) {
		cerr << "must close " << G->name <<
			" group before starting " << g->name <<
			" group" << endl;
		exit(1);
	}

	G = g;

	(*groups)[G->name] = G;
	at_scope = G;
	R = 0;
	F = 0;
	S = 0;
	return;
}


void end_group()
{
	at_scope = 0;
	G = 0; R = 0; F = 0; C = 0; S = 0;
}


void end_register()
{
	at_scope = G;
	R = 0; F = 0; C = 0; S = 0;
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
}

string emit_scope()
{
	// group is always in scope.  shortcuts may be taken elsewhere though
	string scope = G->name;
	if (R && R->name.size()) scope += "_" + R->name;
	return scope;
}


void emit_register(reg_t *r)
{
	R = r;

	bool emit_r = false, emit_w = false, emit_o = false, emit_x = false;
	if ( R->emit.size() ) {
		emit_r   = R->emit.find_first_of("r") != string::npos;
		emit_w   = R->emit.find_first_of("w") != string::npos;
		emit_o   = R->emit.find_first_of("o") != string::npos;
		emit_x   = R->emit.find_first_of("x") != string::npos;
	}

	if (R->def.size())
		symbol_whitelist[R->def] = true;

	R->group = G;
	G->regs[ R->def ] = R;
	at_scope = R;

	if ( emit_r )
		(*regs)[R->def] = R;
	if ( emit_w )
		(*words)[R->def] = R;
	if ( emit_o )
		(*offsets)[R->def] = R;
	if ( emit_x )
		(*deleted)[R->def] = R;
}

void emit_offset(reg_t *r)
{
	emit_register(r);
}

void delete_register(reg_t *r)
{
	// the spec holds the info about deleting...
	emit_register(r);
}


/* begin scope is just setting up a register which won't be emitted */
void begin_scope(reg_t *r)
{
	emit_register(r);
}

void emit_field(field_t *f)
{
	F = f;
	if (F->def.size())
		symbol_whitelist[F->def] = true;

	if ( R ) {
		R->fields[F->def] = F;
		F->reg = R;
	} else if (G) {
		G->fields[F->def] = F;
		F->group = G;
	} else { 
		cerr << "wtf1";
		exit(1);
	}
	at_scope = F;

	(*fields)[F->def] = F;
}

void emit_constant(constant_t *c)
{
	C = c;
	if ( C->def.size() )
		symbol_whitelist[C->def] = true;

	if ( at_scope == G ) {
		G->constants[C->def] = C;
		C->group = G;
	} else if (at_scope == R ) {
		R->constants[C->def] = C;
		C->group = G;
		C->reg = R;
	} else if (at_scope == F ) {
		F->constants[C->def] = C;
		C->group = G;
		C->reg = R;
		C->field = F;
	} else {
		cerr << "what scope is [" << C->name << "] at?\n";
		exit(1);
	}
	(*constants)[C->def] = C;
}


map<string, group_t *> *get_groups()
{
	return groups;
	groups = new map<string, group_t*>();
}
map<string, reg_t *> *get_regs()
{
	return regs;
	regs = new map<string, reg_t *>();
}
map<string, reg_t *> *get_deleted()
{
	return deleted;
	deleted = new map<string, reg_t *>();
}
map<string, reg_t *> *get_words()
{
	return words;
	words = new map<string, reg_t *>();
}
map<string, reg_t *> *get_offsets()
{
	return offsets;
	offsets = new map<string, reg_t *>();
}
map<string, field_t *> *get_fields()
{
	return fields;
	fields = new map<string, field_t *>();
}
map<string, constant_t *> *get_constants()
{
	return constants;
	constants = new map<string, constant_t *>();
}


}
