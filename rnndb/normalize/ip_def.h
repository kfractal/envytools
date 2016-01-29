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

// defn+ -> val+ -> gpus+

struct defn_val_t {
    string    val;
    gpu_set_t gpus;
	gpu_equiv_class_t *eq_class;
	defn_t *defn;
	int line_nr; // earliest line_nr found
	defn_val_t(const string &s) : val(s), eq_class(0) { }
private:
	defn_val_t();
};
typedef set<defn_val_t *> defn_val_set_t;
typedef map<string/*val*/, defn_val_t *> defn_val_index_t;

class defn_t {
public:
	string           symbol;
	defn_val_set_t   vals;
	defn_val_index_t val_index;
	int line_nr; // earliest line_nr found
	defn_t(const string &s) : symbol(s), is_reg(0), is_field(0), is_constant(0) { }
	bool is_reg;
	bool is_field;
	bool is_constant;

private:
	defn_t(){ }
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

defn_set_t *read_ip_defs();

extern list<gpuid_t*> target_gpus;
extern map<string, gpuid_t *> target_gpus_by_name;
extern vector<gpuid_t*> target_gpus_by_ordering;

extern multimap<uint64_t, defn_t *> reg_val_index;
