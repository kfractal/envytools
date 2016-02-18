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

map<string, bool>         symbol_whitelist;
map<string, reg_t *>      register_whitelist;
map<string, offset_t *>   offset_whitelist;
map<string, word_t *>     word_whitelist;
map<string, scope_t *>    scope_whitelist;

map<string, deleted_reg_t *>      deleted_register_whitelist;
map<string, deleted_offset_t *>   deleted_offset_whitelist;
map<string, deleted_word_t *>     deleted_word_whitelist;
map<string, deleted_scope_t *>    deleted_scope_whitelist;

map<string, field_t *>    field_whitelist;
map<string, constant_t *> constant_whitelist;

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
	map<string, ip_whitelist::offset_t *>   * offsets   = ip_whitelist::get_offsets();
	map<string, ip_whitelist::word_t *>     * words     = ip_whitelist::get_words();
	map<string, ip_whitelist::scope_t *>    * scopes    = ip_whitelist::get_scopes();

	map<string, ip_whitelist::deleted_reg_t *>      * deleted_regs     = ip_whitelist::get_deleted_regs();
	map<string, ip_whitelist::deleted_offset_t *>   * deleted_offsets  = ip_whitelist::get_deleted_offsets();
	map<string, ip_whitelist::deleted_word_t *>     * deleted_words    = ip_whitelist::get_deleted_words();
	map<string, ip_whitelist::deleted_scope_t *>    * deleted_scopes   = ip_whitelist::get_deleted_scopes();



	map<string, ip_whitelist::field_t *>    * fields    = ip_whitelist::get_fields();
	map<string, ip_whitelist::constant_t *> * constants = ip_whitelist::get_constants();

	for (auto g : *groups)    chip_groups.   insert(g);
	for (auto r : *regs)      chip_regs.     insert(r);
	for (auto o : *offsets)   chip_offsets.  insert(o);
	for (auto w : *words)     chip_words.    insert(w);

	for (auto dr : *deleted_regs)    chip_deleted_regs.    insert(dr);
	for (auto dw : *deleted_words)   chip_deleted_words.   insert(dw);
	for (auto d_o : *deleted_offsets) chip_deleted_offsets. insert(d_o);
	for (auto dr : *deleted_scopes)  chip_deleted_scopes.  insert(dr);

	for (auto f : *fields)    chip_fields.   insert(f);
	for (auto c : *constants) chip_constants.insert(c);
}

multimap<string, ip_whitelist::group_t *>    chip_groups;
multimap<string, ip_whitelist::reg_t *>      chip_regs;
multimap<string, ip_whitelist::word_t *>     chip_words;
multimap<string, ip_whitelist::offset_t *>   chip_offsets;
multimap<string, ip_whitelist::scope_t *>    chip_scopes;

multimap<string, ip_whitelist::deleted_reg_t *>      chip_deleted_regs;
multimap<string, ip_whitelist::deleted_word_t *>     chip_deleted_words;
multimap<string, ip_whitelist::deleted_offset_t *>   chip_deleted_offsets;
multimap<string, ip_whitelist::deleted_scope_t *>    chip_deleted_scopes;


multimap<string, ip_whitelist::field_t *>    chip_fields;
multimap<string, ip_whitelist::constant_t *> chip_constants;


// current scoping state
static void * at_scope = 0;
static group_t    *G;
static reg_t      *R;
static word_t      *W;
static offset_t    *O;
static scope_t    *SC;

static deleted_reg_t      *DR;
static deleted_word_t      *DW;
static deleted_offset_t    *DO;
static deleted_scope_t    *DSC;


static field_t    *F;
static constant_t *C;
static symbol_t   *S;

static map<string, group_t *>    * groups    = new map<string, group_t *>   ();
static map<string, reg_t *>      * regs      = new map<string, reg_t *>     ();
static map<string, offset_t *>   * offsets   = new map<string, offset_t *>  ();
static map<string, word_t *>     * words     = new map<string, word_t *>    ();
static map<string, scope_t *>     * scopes    = new map<string, scope_t *>    ();
static map<string, deleted_reg_t *> * deleted_regs   = new map<string, deleted_reg_t *>     ();
static map<string, deleted_offset_t *>   * deleted_offsets   = new map<string, deleted_offset_t *>  ();
static map<string, deleted_word_t *>     * deleted_words     = new map<string, deleted_word_t *>    ();
static map<string, deleted_scope_t *>     * deleted_scopes     = new map<string, deleted_scope_t *>    ();
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
	W = 0;
	SC = 0;
	O = 0;
	R = 0;
	F = 0;
	S = 0;
	DW = 0;
	DO = 0;
	DSC = 0;
	DR  = 0;
	return;
}


void end_group()
{
	at_scope = 0;
	G = 0; R = 0; F = 0; C = 0; S = 0;
	SC = 0; W = 0; O = 0;
	DSC = 0; DW = 0; DO = 0; DR = 0;
}


void end_register()
{
	at_scope = G;
	R = 0; F = 0; C = 0; S = 0;
	SC = 0; W = 0; O = 0;
	DSC = 0; DW = 0; DO = 0; DR = 0;
}

void end_scope()
{
	end_register();
}

void end_field()
{
	if ( W )
		at_scope = W;
	else if ( O )
		at_scope = O;
	else if ( SC )
		at_scope = SC;
	else if ( DR )
		at_scope = DR;
	else if ( DW )
		at_scope = DW;
	else if ( DO )
		at_scope = DO;
	else if ( DSC )
		at_scope = DSC;
	else if ( R )
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

	offset_t *offset = dynamic_cast<offset_t*>(r);
	word_t   *word   = dynamic_cast<word_t*>(r);
	scope_t  *scope  = dynamic_cast<scope_t*>(r);

	deleted_reg_t    *deleted_reg    = dynamic_cast<deleted_reg_t*>(r);
	deleted_offset_t *deleted_offset = dynamic_cast<deleted_offset_t*>(r);
	deleted_word_t   *deleted_word   = dynamic_cast<deleted_word_t*>(r);
	deleted_scope_t  *deleted_scope  = dynamic_cast<deleted_scope_t*>(r);

	DR = deleted_reg;
	DO = deleted_offset;
	DW = deleted_word;
	DSC = deleted_scope;

	O = offset;
	W = word;
	SC = scope;

	bool emit_r = false, emit_w = false, emit_o = false, emit_x = false;
	if ( R->emit.size() ) {
		emit_r   = R->emit.find_first_of("r") != string::npos;
		emit_w   = R->emit.find_first_of("w") != string::npos;
		emit_o   = R->emit.find_first_of("o") != string::npos;
		emit_x   = R->emit.find_first_of("x") != string::npos;
	}

	if (R->def.size()) {
		symbol_whitelist[R->def] = true;
		if ( O ) {
			offset_whitelist[O->def] = O;
		} else if ( W ) {
			word_whitelist[W->def] = W;
		} else if ( SC ) {
			scope_whitelist[SC->def] = SC;
		} else if ( DR ) {
			deleted_register_whitelist[DR->def] = DR;
		} else if ( DW ) {
			deleted_word_whitelist[DW->def] = DW;
		} else if ( DO ) {
			deleted_offset_whitelist[DO->def] = DO;
		} else if ( DSC ) { 
			deleted_scope_whitelist[DSC->def] = DSC;
		} else if ( R ) {
			register_whitelist[R->def] = R;
		}
	}

	R->group = G;
	G->regs[ R->def ] = R;

	if (O) {
		(*offsets)[R->def] = O;
		at_scope = O;
	} else if (W) {
		(*words)[R->def] = W;
		at_scope = W;
	} else if (SC) {
		(*scopes)[R->def] = SC;
		at_scope = SC;
	} else if (DR) {
		(*deleted_regs)[R->def] = DR;
		at_scope = DR;
	} else if (DW) {
		(*deleted_words)[R->def] = DW;
		at_scope = DW;
	} else if (DO) {
		(*deleted_offsets)[R->def] = DO;
		at_scope = DO;
	} else if (DSC) {
		(*deleted_scopes)[R->def] = DSC;
		at_scope = DSC;
	} else if (R) {
		(*regs)[R->def] = R;
		at_scope = R;
	} 

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
	if (F->def.size()) {
		symbol_whitelist[F->def] = true;
		field_whitelist[F->def] = f;
	}

	if ( O ) {
		O->fields[F->def] = F;
		F->reg = O;
	} else if ( W ) {
		W->fields[F->def] = F;
		F->reg = W;
	} else if ( SC) {
		SC->fields[F->def] = F;
		F->reg = SC;
	} else if ( R ) {
		R->fields[F->def] = F;
		F->reg = R;
	} else if ( DO ) {
		DO->fields[F->def] = F;
		F->reg = DO;
	} else if ( DW ) {
		DW->fields[F->def] = F;
		F->reg = DW;
	} else if ( DSC) {
		DSC->fields[F->def] = F;
		F->reg = DSC;
	} else if ( DR ) {
		DR->fields[F->def] = F;
		F->reg = DR;
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
	if ( C->def.size() ) {
		symbol_whitelist[C->def] = true;
		constant_whitelist[C->def] = c;
	}

	if ( at_scope == G ) {
		G->constants[C->def] = C;
		C->group = G;
	} else if (at_scope == O ) {
		O->constants[C->def] = C;
		C->group = G;
		C->reg = O;
	} else if (at_scope == W ) {
		W->constants[C->def] = C;
		C->group = G;
		C->reg = W;
	} else if (at_scope == SC ) {
		SC->constants[C->def] = C;
		C->group = G;
		C->reg = SC;
	} else if (at_scope == R ) {
		R->constants[C->def] = C;
		C->group = G;
		C->reg = R;
	} else if (at_scope == DO ) {
		DO->constants[C->def] = C;
		C->group = G;
		C->reg = DO;
	} else if (at_scope == DW ) {
		DW->constants[C->def] = C;
		C->group = G;
		C->reg = DW;
	} else if (at_scope == DSC ) {
		DSC->constants[C->def] = C;
		C->group = G;
		C->reg = DSC;
	} else if (at_scope == DR ) {
		DR->constants[C->def] = C;
		C->group = G;
		C->reg = DR;
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
map<string, deleted_reg_t *> *get_deleted_regs()
{
	return deleted_regs;
	deleted_regs = new map<string, deleted_reg_t *>();
}
map<string, word_t *> *get_words()
{
	return words;
	words = new map<string, word_t *>();
}
map<string, scope_t *> *get_scopes()
{
	return scopes;
	scopes = new map<string, scope_t *>();
}
map<string, offset_t *> *get_offsets()
{
	return offsets;
	offsets = new map<string, offset_t *>();
}
map<string, deleted_word_t *> *get_deleted_words()
{
	return deleted_words;
	deleted_words = new map<string, deleted_word_t *>();
}
map<string, deleted_offset_t *> *get_deleted_offsets()
{
	return deleted_offsets;
	deleted_offsets = new map<string, deleted_offset_t *>();
}
map<string, deleted_scope_t *> *get_deleted_scopes()
{
	return deleted_scopes;
	deleted_scopes = new map<string, deleted_scope_t *>();
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
