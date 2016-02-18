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

#include <set>
#include <string>
#include <sstream>

#include "ip_whitelist.h"

using namespace std;

class defn_t;
typedef set<defn_t *> defn_set_t;
typedef map<string/*symbol*/, defn_t *> defn_index_t;

class gpu_family_t;
class gpu_equiv_class_t;

class gpuid_t {
	set<string> _hwref_files;
	defn_set_t   _defines;
	defn_index_t _defn_index;
	size_t _ordering_index;
public:
	defn_set_t *defines() { return &_defines; }
	defn_index_t *defns_index() { return &_defn_index; }
    void get_hwref_files();
    struct ctor_t {
        string   name;
        uint16_t id;
        string   hwref_dir;
		gpu_family_t *family;
    } _d;

    const string &       name()        const { return _d.name;      }
          uint16_t       id()          const { return _d.id;        }
    const string &       hwref_dir()   const { return _d.hwref_dir; }
    const set<string> &  hwref_files() const { return _hwref_files; }
          gpu_family_t * family()      const { return _d.family;    }

	size_t ordering_index() const { return _ordering_index; }
	void set_ordering_index(size_t i) { _ordering_index = i; }

	const string ref() const { return "__nv_" + _d.name +"__"; } 

    gpuid_t(const ctor_t &c) : _hwref_files(), _d(c) { }
    ~gpuid_t(){ }
};

typedef set <gpuid_t *> gpu_set_t;
typedef list<gpuid_t *> gpu_list_t;

class gpu_family_t {
    string    _name;
    gpu_set_t _gpus;
public:
	const string &name() { return _name; }
    gpu_family_t(string n) : _name(n) { }
    ~gpu_family_t() { }

    void insert   (gpuid_t *g) { _gpus.insert(g); }
	const gpu_set_t &gpus() { return _gpus; }

    string ref() { return "__nv_gpu_is_" + _name; }
    string def() {
		string ret;
		if ( _gpus.size()) {
			ret = "#define __nv_gpu_is_" + _name + "(x) ((x)!=(x)\\";
			std::set<string> sorted_gpus;
			for ( auto &g : _gpus )
				sorted_gpus.insert(g->ref());

			for ( auto &g : sorted_gpus )
				ret += "\n\t\t|| ((x)==" + g + ")\\";
			ret += "\n\t)\n";
		}
		return ret;
    }
    bool fully_represented_in(const gpu_set_t &compare) {
        for ( auto &member : _gpus ) {
            if ( compare.find(member) == compare.end() ) {
                return false;
            }
        }
        return true;
    }
};

struct evaluation_result_t {
	bool field;
	union {
		struct {
			int64_t low;
			int64_t high;
		} field;
		int64_t val;
	} result;
};


// defn+ -> val+ -> gpus+

struct defn_val_t {
    string    val;
    gpu_set_t gpus;
	gpu_equiv_class_t *eq_class;
	defn_t *defn;
	int line_nr; // earliest line_nr found
	vector<string> idents;
	bool evaluated;
	evaluation_result_t evaluation;
	defn_val_t(const string &s) : val(s), eq_class(0), evaluated(false) { }
private:
	defn_val_t();
};

typedef set<defn_val_t *> defn_val_set_t;
typedef map<string/*val*/, defn_val_t *> defn_val_index_t;
typedef set<ip_whitelist::group_t *>    group_set_t;
typedef set<ip_whitelist::reg_t *>      register_set_t;
typedef set<ip_whitelist::field_t *>    field_set_t;
typedef set<ip_whitelist::constant_t *> constant_set_t;

class defn_t {
public:
	string           symbol;
	defn_val_set_t   vals;
	defn_val_index_t val_index;
	//	int line_nr; // earliest line_nr found
	defn_t(const string &s) : symbol(s) , is_reg(0), is_field(0), is_constant(0) { }

	/* elements of the whitelist info this symbol plays a part in. */
	group_set_t    groups;
	register_set_t regs;
	field_set_t    fields;
	constant_set_t constants;
	bool is_reg; 
	bool is_field; 
	bool is_constant;
	bool operator ==(const defn_t &o);
private:
	defn_t(){ }
};

class def_t;
class group_def_t;
class reg_def_t;
class word_def_t;
class scope_def_t;
class offset_def_t;
class field_def_t;
class const_def_t;

class def_tree_t {
public:
	set<gpuid_t *>   gpus;
	vector<def_t *>  defs;

	map<string, group_def_t *>  groups_by_name;
	vector<group_def_t *>       groups;

	map<uint64_t, reg_def_t *>  regs_by_val;
	map<string, reg_def_t *>    regs_by_name;

	map<string, const_def_t *> constants_by_name;
	map<string, field_def_t *> fields_by_name;

	map<string, offset_def_t *> offsets_by_name;
	map<string, word_def_t *>   words_by_name;
	map<string, scope_def_t *>  scopes_by_name;

	map<string, reg_def_t *>    deleted_by_name;

	def_tree_t() { }
	bool operator ==(const def_tree_t &t);
};

class def_t {
public:
	def_tree_t *tree;
	string symbol;
	def_t(def_tree_t *t, const string &s) : tree(t), symbol(s) { }
	bool operator ==(const def_t &o);
};

class const_def_t : public def_t {
public:
	int64_t val;
	const_def_t(def_tree_t *t, const string &s) : def_t(t, s), val(0) { }
	bool operator ==(const const_def_t &o);
};

class field_def_t : public def_t {
public:
	size_t high;
	size_t low;
	field_def_t(def_tree_t *t, const string &s) : def_t(t,s), high(0), low(0) { }

	vector<const_def_t *>      constants;
	map<string, const_def_t *> constants_by_name;

	bool operator ==(const field_def_t &o);
};

class reg_def_t : public def_t {
public:
	uint64_t val;
	reg_def_t(def_tree_t *t, const string &s) : def_t(t,s), val(0) { }

	vector<field_def_t *>        fields;
	map<string, field_def_t*>    fields_by_name;

	vector<const_def_t *>      constants;
	map<string, const_def_t *> constants_by_name;

	bool operator ==(const reg_def_t &o);
};

class offset_def_t : public reg_def_t {
public:
	offset_def_t(def_tree_t *t, const string &s) : reg_def_t(t,s) { }
	bool operator ==(const offset_def_t &o);
};

class word_def_t : public reg_def_t {
public:
	word_def_t(def_tree_t *t, const string &s) : reg_def_t(t,s) { }
	bool operator ==(const word_def_t &o);
};

class scope_def_t : public reg_def_t {
public:
	scope_def_t(def_tree_t *t, const string &s) : reg_def_t(t,s) { }
	bool operator ==(const scope_def_t &o);
};


class group_def_t : public def_t {
public:
	group_def_t(def_tree_t *t, const string &g) : def_t(t,g) { }
	vector<reg_def_t *> regs;
	map<string, reg_def_t *> regs_by_name;

	vector<offset_def_t *> offsets;
	map<string, offset_def_t *> offsets_by_name;

	vector<word_def_t *> words;
	map<string, word_def_t *> words_by_name;

	vector<scope_def_t *> scopes;
	map<string, scope_def_t *> scopes_by_name;

	vector<field_def_t *> fields;
	map<string, field_def_t *> fields_by_name;

	vector<const_def_t *> constants;
	map<string, const_def_t *> constants_by_name;
	bool operator ==(const group_def_t &o);

};

class gpu_equiv_class_t {
    set<gpu_family_t *> _families;
    gpu_set_t           _gpus;
	string _def;
	string _ref;
	//int _def_order;

	static vector<gpu_equiv_class_t *> _snapshot;
	static gpu_equiv_class_t *find(const gpu_equiv_class_t &o);
public:
    gpu_equiv_class_t(const gpu_equiv_class_t &o)  {
		_families = o._families;
		_gpus = o._gpus;
		_def = o._def;
		_ref = o._ref;
		//_def_order = o._def_order;
	}
    gpu_equiv_class_t() : _families(), _gpus() { }
    ~gpu_equiv_class_t() { }
    void insert(gpuid_t *);
	const set<gpu_family_t *> & families() const { return _families; }
	const set<gpuid_t *>      & gpus()     const { return _gpus; }
	bool operator <(const gpu_equiv_class_t & o) const;
	bool operator ==(const gpu_equiv_class_t & o) const;
	const string &ref() const { return _ref; }
	const string &def() const { return _def; }
	//int order() const { return _def_order; }

	gpu_equiv_class_t * snap(const string &tag);
	static void snapshot_reset();
	static int snapshot_size() { return _snapshot.size();}
	static gpu_equiv_class_t * snapshot_at(int i) { return _snapshot.at(i); }
};

void read_ip_defs();
defn_index_t *get_defn_index();
defn_set_t   *get_defns();


extern list<gpuid_t*> target_gpus;
extern map<string, gpuid_t *> target_gpus_by_name;
extern vector<gpuid_t*> target_gpus_by_ordering;

extern multimap<uint64_t, defn_val_t *> reg_val_index;
extern multimap<string, defn_t *> field_val_index;
extern multimap<string, defn_t *> constant_index;
extern defn_index_t __register_index;
extern defn_index_t __field_index;
extern defn_index_t __constant_index;

extern vector<def_tree_t *> gpu_def_trees;
