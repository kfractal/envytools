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
#include <cassert>

using namespace std;

#include "ip_whitelist.h"

namespace ip_whitelist {

map<string, bool>         symbol_whitelist;

map<string, group_t*>     group_whitelist;
map<string, reg_t *>      register_whitelist;
map<string, field_t *>    field_whitelist;
map<string, constant_t *> constant_whitelist;

map<string, group_t*>     deleted_group_whitelist;
map<string, reg_t *>      deleted_register_whitelist;
map<string, field_t *>    deleted_field_whitelist;
map<string, constant_t *> deleted_constant_whitelist;



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
	if ( wl_gpu == "gk20a" ) {
		ip_whitelist::emit_groups_gk20a();
	} else {
		ip_whitelist::emit_groups_gm20b();
	}

	map<string, ip_whitelist::group_t *>    * groups    = ip_whitelist::get_groups();
	map<string, ip_whitelist::reg_t *>      * regs      = ip_whitelist::get_regs();
	map<string, ip_whitelist::field_t *>    * fields    = ip_whitelist::get_fields();
	map<string, ip_whitelist::constant_t *> * constants = ip_whitelist::get_constants();

	map<string, ip_whitelist::group_t *>    * deleted_groups    = ip_whitelist::get_deleted_groups();
	map<string, ip_whitelist::reg_t *>      * deleted_regs      = ip_whitelist::get_deleted_regs();
	map<string, ip_whitelist::field_t *>    * deleted_fields    = ip_whitelist::get_deleted_fields();
	map<string, ip_whitelist::constant_t *> * deleted_constants = ip_whitelist::get_deleted_constants();


	for (auto g : *groups)    chip_groups.   insert(g);
	for (auto r : *regs)      chip_regs.     insert(r);
	for (auto f : *fields)    chip_fields.   insert(f);
	for (auto c : *constants) chip_constants.insert(c);

	for (auto g : *deleted_groups)    chip_deleted_groups.   insert(g);
	for (auto r : *deleted_regs)      chip_deleted_regs.     insert(r);
	for (auto f : *deleted_fields)    chip_deleted_fields.   insert(f);
	for (auto c : *deleted_constants) chip_deleted_constants.insert(c);
}

multimap<string, ip_whitelist::group_t *>    chip_groups;
multimap<string, ip_whitelist::reg_t *>      chip_regs;
multimap<string, ip_whitelist::field_t *>    chip_fields;
multimap<string, ip_whitelist::constant_t *> chip_constants;

multimap<string, ip_whitelist::group_t *>    chip_deleted_groups;
multimap<string, ip_whitelist::reg_t *>      chip_deleted_regs;
multimap<string, ip_whitelist::field_t *>    chip_deleted_fields;
multimap<string, ip_whitelist::constant_t *> chip_deleted_constants;


// current scoping state
static void * at_scope = 0;

//static symbol_t   *S;

static group_t    *G;
static reg_t      *R;
static field_t    *F;
static constant_t *C;

static group_t    *DG;
static reg_t      *DR;
static field_t    *DF;
static constant_t *DC;

static map<string, group_t *>    * groups    = new map<string, group_t *>   ();
static map<string, reg_t *>      * regs      = new map<string, reg_t *>     ();
static map<string, field_t *>    * fields    = new map<string, field_t *>   ();
static map<string, constant_t *> * constants = new map<string, constant_t *>();

static map<string, group_t *>    * deleted_groups    = new map<string, group_t *>   ();
static map<string, reg_t *>      * deleted_regs      = new map<string, reg_t *>     ();
static map<string, field_t *>    * deleted_fields    = new map<string, field_t *>   ();
static map<string, constant_t *> * deleted_constants = new map<string, constant_t *>();


void begin_group(group_t *g)
{
	if (G || DG) {
		cerr << "must close " << (G?G:DG)->name <<
			" group before starting " << g->name <<
			" group" << endl;
		exit(1);
	}

	if ( g->removed ) {
		DG = g;
		(*deleted_groups)[g->def] = g;
		at_scope = DG;
	} else {
		G = g;
		(*groups)[g->def] = g;
		at_scope = G;
	}
	symbol_whitelist[g->def] = true;

	R = 0;	F = 0; C=0;
	DR = 0; DF = 0; DC = 0;
	return;
}


void end_group()
{
	at_scope = 0;
	G = 0; R = 0; F = 0; C = 0;
	DG = 0; DR = 0; DF = 0; DC = 0;
}


void end_register()
{
	at_scope = G;
	R = 0; F = 0; C = 0;
	DG = 0; DR = 0; DF = 0; DC = 0;
}

void end_scope()
{
	end_register();
}

void end_field()
{
	if ( DR || R ) {
		at_scope = R ? R : DR;
	} else if ( DG || G ) {
		at_scope = G ? G : DG;
	}
	DF = F = 0;
	DC = C = 0;
}

string emit_scope()
{
	// group is always in scope.  shortcuts may be taken elsewhere though
	string scope = G->name;
	if (R && R->name.size()) scope += "_" + R->name;
	else if (DR && DR->name.size()) scope += "_" + DR->name;
	return scope;
}


void emit_register(reg_t *r)
{
	if ( ! (r && r->def.size() ) )
		return;
	if ( ! (G || DG) ) { // registers w/o enclosing group aren't allowed.
		return;
	}
	assert(!(G && DG));
	group_t *group = G ? G : DG;

	if ( r->removed ) {
		group->deleted_regs[r->def] = r;
		(*deleted_regs)[r->def] = r;
	} else {
		group->regs[r->def] = r;
		(*regs)[r->def] = r;
	}
	
	r->group = group;
	R = r;
	symbol_whitelist[r->def] = true;
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
	if (!f && f->def.size()) {
		return;
	}
	if (!(R || DR)) {
		return;
	}
	assert(!( R && DR));

	reg_t *r = R ? R : DR;

	if ( f->removed ) {
		r->deleted_fields[f->def] = f;
		(*deleted_fields)[f->def] = f;
	} else {
		r->fields[f->def] = f;
		(*fields)[f->def] = f;
	}

	f->reg = r;
	f->group = G ? G : DG;
	F = f;
	at_scope = F;
	symbol_whitelist[f->def] = true;
}

void emit_constant(constant_t *c)
{
	// constants can belong to any scope (from group down).
	if ( !c && c->def.size() )
		return;

	if (at_scope == G || at_scope == DG ) {
		group_t *g = G ? G : DG;
		if ( c->removed ) {
			g->deleted_constants[c->def] = c;
			(*deleted_constants)[c->def] = c;
		} else {
			g->constants[c->def] = c;
			(*constants)[c->def] = c;
		}
		c->group = g;
	} else if (at_scope == R || at_scope == DR ) {
		reg_t *r = R ? R : DR;
		if ( c->removed ) {
			r->deleted_constants[c->def] = c;
			(*deleted_constants)[c->def] = c;
		} else {
			r->constants[c->def] = c;
			(*constants)[c->def] = c;
		}
		c->reg = r;
	} else if (at_scope == F || at_scope== DF ) {
		field_t *f = F ? F : DF;
		if ( c->removed ){
			f->deleted_constants[c->def] = c;
			(*deleted_constants)[c->def] = c;
		} else {
			f->constants[c->def] = c;
			(*constants)[c->def] = c;
		}
		c->field = f;
	}
	symbol_whitelist[c->def] = true;
	C = c;
}

map<string, group_t *> *get_groups()
{
	return groups;
	groups = new map<string, group_t*>();
}

map<string, group_t *> *get_deleted_groups()
{
	return deleted_groups;
	deleted_groups = new map<string, group_t*>();
}


map<string, reg_t *> *get_regs()
{
	return regs;
	regs = new map<string, reg_t *>();
}

map<string, reg_t *> *get_deleted_regs()
{
	return deleted_regs;
	deleted_regs = new map<string, reg_t *>();
}


map<string, field_t *> *get_fields()
{
	return fields;
	fields = new map<string, field_t *>();
}

map<string, field_t *> *get_deleted_fields()
{
	return deleted_fields;
	deleted_fields = new map<string, field_t *>();
}


map<string, constant_t *> *get_constants()
{
	return constants;
	constants = new map<string, constant_t *>();
}

map<string, constant_t *> *get_deleted_constants()
{
	return deleted_constants;
	deleted_constants = new map<string, constant_t *>();
}


} // ip_whitelist namespace
