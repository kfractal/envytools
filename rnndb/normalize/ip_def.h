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

#include <QTextStream>

using namespace std;

class gpu_family_t;

class gpuid_t {
	set<string> _hwref_files;
	size_t _ordering_index;

public:
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
    gpuid_t(const ctor_t &c) : _hwref_files(), _d(c) { }
    ~gpuid_t(){ }
};

typedef set <gpuid_t *> gpu_set_t;
typedef list<gpuid_t *> gpu_list_t;

std::ostream &operator<<(std::ostream &os, const gpu_set_t &gpus);
std::ostream &operator<<(std::ostream &os, const gpu_list_t &gpus);

QTextStream &operator<<(QTextStream &os, const gpu_set_t &gpus);
QTextStream &operator<<(QTextStream &os, const gpu_list_t &gpus);


class gpu_family_t {
    string    _name;
    gpu_set_t _gpus;
public:
	const string &name() { return _name; }
    gpu_family_t(string n) : _name(n) { }
    ~gpu_family_t() { }

    void insert   (gpuid_t *g) { _gpus.insert(g); }
	const gpu_set_t &gpus() { return _gpus; }

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

class def_t;
class group_def_t;
class reg_def_t;
class word_def_t;
class scope_def_t;
class offset_def_t;
class field_def_t;
class const_def_t;
class deleted_reg_def_t;
class deleted_word_def_t;
class deleted_offset_def_t;
class deleted_scope_def_t;

typedef set<def_t*> def_set_t;
typedef set<group_def_t *>  group_def_set_t;
typedef set<reg_def_t *>    reg_def_set_t;
typedef set<field_def_t *>  field_def_set_t;
typedef set<const_def_t *>  const_def_set_t;

typedef set<deleted_reg_def_t *>    deleted_reg_def_set_t;
typedef set<offset_def_t *> offset_def_set_t;
typedef set<deleted_offset_def_t *> deleted_offset_def_set_t;
typedef set<word_def_t *>   word_def_set_t;
typedef set<deleted_word_def_t *>   deleted_word_def_set_t;
typedef set<scope_def_t *>  scope_def_set_t;
typedef set<deleted_scope_def_t *>  deleted_scope_def_set_t;


class def_tree_t {
public:
	gpu_set_t   gpus;

	// if "gpus" is a single gpu then the sets below are overkill.
	// but when we merge gpu def trees then sets (i.e. not bare def_t *'s) are required.

	map<string, def_set_t>        defs_by_name;

	map<string, group_def_set_t>  groups_by_name;
	vector<group_def_t *>         groups;

	//	map<uint64_t, reg_def_set_t>  regs_by_val;
	map<string,   reg_def_set_t>  regs_by_name;

	
	map<string, offset_def_set_t> offsets_by_name;
	map<string, word_def_set_t>   words_by_name;
	map<string, scope_def_set_t>  scopes_by_name;

	map<string, const_def_set_t>  constants_by_name;
	map<string, field_def_set_t>  fields_by_name;

	map<string, deleted_reg_def_set_t>    deleted_regs_by_name;
	map<string, deleted_offset_def_set_t> deleted_offsets_by_name;
	map<string, deleted_word_def_set_t>   deleted_words_by_name;
	map<string, deleted_scope_def_set_t>  deleted_scopes_by_name;

	def_tree_t() { }
	def_tree_t(vector<def_tree_t *> &);
	def_tree_t(def_tree_t *);
	bool operator ==(const def_tree_t &t);

	friend std::ostream& operator<< (std::ostream& os, const def_tree_t& tree);
	friend QTextStream& operator<< (QTextStream& os, const def_tree_t& tree);

	group_def_t  *instance_group (ip_whitelist::group_t  *g);
	offset_def_t *instance_offset(ip_whitelist::offset_t *o);
	word_def_t   *instance_word  (ip_whitelist::word_t   *w);
	reg_def_t    *instance_reg   (ip_whitelist::reg_t    *r);
	scope_def_t  *instance_scope (ip_whitelist::scope_t  *s);
	field_def_t  *instance_field (ip_whitelist::field_t  *f);
	const_def_t  *instance_const (ip_whitelist::constant_t *c);

	def_t *map_to_new(def_t *orig_def, map< def_t *, def_t * > &def_correlates);

};

class def_t {
public:
	gpu_set_t def_gpus;
	def_tree_t *tree;
	string symbol;
	def_t(def_tree_t *t, const string &s, const gpu_set_t &gpus) : def_gpus(gpus), tree(t), symbol(s) { }
	bool operator ==(const def_t &o);
	virtual def_t *clone(def_tree_t *t) { return new def_t(t, symbol, def_gpus); }
	friend std::ostream& operator<< (std::ostream& os, const def_t& d) {
		os << d.def_gpus;
		os << d.symbol;
		return os;
	}
	friend QTextStream& operator<< (QTextStream& os, const def_t& d) {
		os << "def " << d.symbol.c_str() << " { gpus=[" << d.def_gpus <<"] " << endl;
		return os;
	}

};

class const_def_t : public def_t {
public:
	int64_t val;
	void set_val(int64_t v) { val = v; }
	const_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, int64_t val = 0) : def_t(t, s, g), val(val) { }

	bool operator ==(const const_def_t &o);

	virtual def_t *clone(def_tree_t *t) { return new const_def_t(t, symbol, def_gpus, val); }
	friend std::ostream& operator<< (std::ostream& os, const const_def_t& d) {
		os << d.def_gpus;
		os << d.symbol;
		os << d.val;
		return os;
	}
	friend QTextStream& operator<< (QTextStream& os, const const_def_t& d) {
		os << "const def " << d.symbol.c_str() << " { val=" << d.val << " gpus=[" << d.def_gpus << "]" << endl;
		return os;
	}
};

class field_def_t : public def_t {
public:
	size_t high;
	size_t low;
	void set_field(size_t h, size_t l) { high = h; low = l; }
	field_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, 
				pair<size_t, size_t> hl = make_pair(0,0)) : def_t(t,s, g), high(hl.first), low(hl.second) { }

	map<string, set<const_def_t *>> constants_by_name;

	bool operator ==(const field_def_t &o);
	virtual def_t *clone(def_tree_t *t) { return new field_def_t(t, symbol, def_gpus, make_pair(high, low)); }

 	friend std::ostream& operator<< (std::ostream& os, const field_def_t& d) {
		os << d.def_gpus;
		os << d.symbol;
		os << d.high << ":" << d.low;
		return os;
	}
 	friend QTextStream& operator<< (QTextStream& os, const field_def_t& d) {
		os << "field def " << d.symbol.c_str() << " { val=" << d.high << ":" << d.low << " gpus=[" <<d.def_gpus << "]" << endl;
		return os;
	}

};

class reg_def_t : public def_t {
public:
	uint64_t val;
	void set_val(uint64_t v) { val = v; }
	reg_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t val = 0) :
		def_t(t,s, g), val(val) { }

	map<string, field_def_set_t>    fields_by_name;
	map<string, set<const_def_t *>> constants_by_name;

	bool operator ==(const reg_def_t &o);

	virtual def_t *clone(def_tree_t *t) { return new reg_def_t(t, symbol, def_gpus, val); }

 	friend QTextStream& operator<< (QTextStream& os, const reg_def_t& d) {
		os << "reg def " << d.symbol.c_str() << " val=" << d.val <<
			" gpus=[" << d.def_gpus << "]" << endl;
		return os;
	}

};

class deleted_reg_def_t : public reg_def_t {
public:
	deleted_reg_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t val = 0) :
		reg_def_t(t,s,g,val) { }
	virtual def_t *clone(def_tree_t *t) { return new deleted_reg_def_t(t, symbol, def_gpus, val); }

	bool operator ==(const deleted_reg_def_t &o);
};

class offset_def_t : public reg_def_t {
public:
	offset_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v = 0) : reg_def_t(t,s,g,v) { }
	virtual def_t *clone(def_tree_t *t) { return new offset_def_t(t, symbol, def_gpus, val); }
};

class deleted_offset_def_t : public offset_def_t {
public:
	deleted_offset_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v = 0) :
		offset_def_t(t,s,g,v) { }
	virtual def_t *clone(def_tree_t *t){ return new deleted_offset_def_t(t, symbol, def_gpus, val); }
};

class word_def_t : public reg_def_t {
public:
	word_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v = 0) : reg_def_t(t,s,g, v) { }
	bool operator ==(const word_def_t &o);
	virtual def_t *clone(def_tree_t *t){ return new word_def_t(t, symbol, def_gpus, val); }
};

class deleted_word_def_t : public word_def_t {
public:
	deleted_word_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v) : word_def_t(t,s,g,v) { }
	virtual def_t *clone(def_tree_t *t){ return new deleted_word_def_t(t, symbol, def_gpus, val); }
	bool operator ==(const deleted_word_def_t &o);

};

class scope_def_t : public reg_def_t {
public:
	scope_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v=0) : reg_def_t(t,s,g,v) { }
	virtual def_t *clone(def_tree_t *t){ return new scope_def_t(t, symbol, def_gpus, val); }
	bool operator ==(const scope_def_t &o);
};

class deleted_scope_def_t : public scope_def_t {
public:
	deleted_scope_def_t(def_tree_t *t, const string &s, const gpu_set_t &g, uint64_t v = 0) :
		scope_def_t(t,s,g,v) { }
	virtual def_t *clone(def_tree_t *t){ return new deleted_scope_def_t(t, symbol, def_gpus, val); }
	bool operator ==(const deleted_scope_def_t &o);
};


class group_def_t : public def_t {
public:
	group_def_t(def_tree_t *t, const string &g, const gpu_set_t &gg) : def_t(t,g,gg) { }
	virtual def_t *clone(def_tree_t *t) { return new group_def_t(t, symbol, def_gpus); }

	//	vector<reg_def_t *> regs;
	map<string, reg_def_set_t > regs_by_name;

	// vector<offset_def_t *> offsets;
	map<string, offset_def_set_t > offsets_by_name;

	//	vector<word_def_t *> words;
	map<string, word_def_set_t > words_by_name;

	// vector<scope_def_t *> scopes;
	map<string, scope_def_set_t > scopes_by_name;

	//	vector<field_def_t *> fields;
	map<string, field_def_set_t > fields_by_name;

	// vector<const_def_t *> constants;
	map<string, const_def_set_t > constants_by_name;
	bool operator ==(const group_def_t &o);

	//	vector<deleted_reg_def_t *> deleted_regs;
	map<string, deleted_reg_def_set_t > deleted_regs_by_name;

	//	vector<deleted_offset_def_t *> deleted_offsets;
	map<string, deleted_offset_def_set_t > deleted_offsets_by_name;

	//	vector<deleted_word_def_t *> deleted_words;
	map<string, deleted_word_def_set_t > deleted_words_by_name;

	//	vector<deleted_scope_def_t *> deleted_scopes;
	map<string, deleted_scope_def_set_t > deleted_scopes_by_name;

	//	void coalesce(group_def_t *with);

 	friend std::ostream& operator<< (std::ostream& os, const group_def_t& d);
 	friend QTextStream& operator<< (QTextStream& os, const group_def_t& d);
};

vector<def_tree_t *> read_ip_defs();
//defn_index_t *get_defn_index();
//defn_set_t   *get_defns();


extern list<gpuid_t*> target_gpus;
extern map<string, gpuid_t *> target_gpus_by_name;
extern vector<gpuid_t*> target_gpus_by_ordering;



