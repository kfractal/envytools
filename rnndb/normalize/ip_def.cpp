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

#include <unordered_map>
#include <fstream>

#include <QDir>
#include <QFileInfo>
#include <QFile>

#include "ip_whitelist.h"

#define yyFlexLexer exprFlexLexer
#include <FlexLexer.h>

#include "expr.h"
FooLexer* lexer = 0;

#include "ip_def.h"

using namespace std;

#ifndef USE_QT_REGEX
#include <regex>
using namespace std::regex_constants;
#else
#    include <QRegularExpression>
#endif

defn_set_t __defines;
defn_index_t __defn_index;

list<gpuid_t*> target_gpus;
map<string, gpuid_t*> target_gpus_by_name;
vector<gpuid_t*> target_gpus_by_ordering;


static gpu_family_t
	g8x_family     {"g8x"},
	tesla_family   {"tesla"},
	kepler_family  {"kepler"},
	fermi_family   {"fermi"},
	maxwell_family {"maxwell"},
	pascal_family  {"pascal"};

#if 0
static gpu_family_t *family_order[] {
	&g8x_family,
	&tesla_family,
	&fermi_family,
	&kepler_family,
	&maxwell_family,
	&pascal_family
};
#endif

// files which are of a different format (not registers/fields)
// or which otherwise aren't applicable.
map<string, bool> hwref_file_blacklist {
	{ "vpmSigNames.h" , true},
	{ "user_access_map.h", true},
	{ "user_access_map_gzip.h", true},
	{ "pm_signals.h", true },
	{ "kind_macros.h", true},
};

//
// master list of nouveau supported gpus grouped by family/arch, id.
//
static vector<gpuid_t::ctor_t> gpuid_table {

	{ "g80"     , 0x0080 , "${hwref}/nv50",  &g8x_family },
	{ "g84"     , 0x0084 , "${hwref}/g84",   &g8x_family },
	{ "g86"     , 0x0086 , "${hwref}/g86",   &g8x_family },
	{ "g92"     , 0x0092 , "${hwref}/g92",   &g8x_family },
	{ "g94"     , 0x0094 , "${hwref}/g94",   &g8x_family },
	{ "g96"     , 0x0096 , "${hwref}/g96",   &g8x_family },
	{ "g98"     , 0x0098 , "${hwref}/g98",   &g8x_family },

	{ "mcp77"   , 0x00aa , "${hwref}/igt206", &tesla_family },
	{ "mcp79"   , 0x00ac , "${hwref}/igt209", &tesla_family },
	{ "mcp89"   , 0x00af , "${hwref}/igt21a", &tesla_family },
	{ "g200"    , 0x00a0 , "${hwref}/gt200", &tesla_family },
	{ "gt215"   , 0x00a3 , "${hwref}/gt215", &tesla_family },
	{ "gt216"   , 0x00a5 , "${hwref}/gt216", &tesla_family },
	{ "gt218"   , 0x00a8 , "${hwref}/gt218", &tesla_family },


	{ "gf100"   , 0x00c0 , "${hwref}/fermi/gf100",  &fermi_family },
	{ "gf104"   , 0x00c4 , "${hwref}/fermi/gf104",  &fermi_family },
	{ "gf106"   , 0x00c3 , "${hwref}/fermi/gf106",  &fermi_family },
	{ "gf108"   , 0x00c1 , "${hwref}/fermi/gf108",  &fermi_family },
	{ "gf110"   , 0x00c8 , "${hwref}/fermi/gf100b", &fermi_family },
	{ "gf116"   , 0x00cf , "${hwref}/fermi/gf106b", &fermi_family },
	{ "gf114"   , 0x00ce , "${hwref}/fermi/gf104b", &fermi_family },
	{ "gf117"   , 0x00d7 , "${hwref}/fermi/gf117",  &fermi_family },
	{ "gf119"   , 0x00d9 , "${hwref}/fermi/gf119",  &fermi_family },

	{ "gk104"   , 0x00e4 , "${hwref}/kepler/gk104",  &kepler_family },
	{ "gk106"   , 0x00e6 , "${hwref}/kepler/gk106",  &kepler_family },
	{ "gk107"   , 0x00e7 , "${hwref}/kepler/gk107",  &kepler_family },
	{ "gk110"   , 0x00f0 , "${hwref}/kepler/gk110",  &kepler_family },
	{ "gk110b"  , 0x00f1 , "${hwref}/kepler/gk110b", &kepler_family },
	{ "gk208b"  , 0x0106 , "${hwref}/kepler/gk208s", &kepler_family },
	{ "gk208"   , 0x0108 , "${hwref}/kepler/gk208",  &kepler_family },
	{ "gk20a"   , 0x00ea , "${hwref}/kepler/gk20a",  &kepler_family },

	{ "gm107"   , 0x0117 , "${hwref}/maxwell/gm107", &maxwell_family },
	{ "gm108"   , 0x0118 , "${hwref}/maxwell/gm108", &maxwell_family },
	{ "gm204"   , 0x0124 , "${hwref}/maxwell/gm204", &maxwell_family },
	{ "gm206"   , 0x0126 , "${hwref}/maxwell/gm206", &maxwell_family },
	{ "gm20b"   , 0x012b , "${hwref}/maxwell/gm20b", &maxwell_family },

};

//
// setup for finding nvidia-internal hardware headers
//
struct repl_symbol_t {
#ifndef USE_QT_REGEX
	regex  match_expr; // set programmatically (i.e. don't bother in initializer)
#else
	QRegularExpression match_expr;
#endif
	string value;      // set in static initializer or at alloc/init
};

static unordered_map<string, repl_symbol_t> symbol_map {
	{ "sw_root" , { .value = { "/Volumes/too/t/sw" } } },
	{ "chips_a" , { .value = { "${sw_root}/dev/gpu_drv/chips_a" } } },
	{ "hwref"   , { .value = { "${chips_a}/drivers/common/inc/hwref" } } },
};

static void init_symbols()
{
	for (auto sym_i = symbol_map.begin(); sym_i != symbol_map.end(); sym_i++) {
		string repl = sym_i->first;
#ifndef USE_QT_REGEX
		sym_i->second.match_expr = regex( "\\$\\{" + repl + "\\}", extended );
#else
		sym_i->second.match_expr =
			QRegularExpression( QString::fromStdString("\\$\\{" + repl + "\\}"),
								QRegularExpression::ExtendedPatternSyntaxOption );
#endif
	}
}

//
// Given the set of symbols and their names replace occurences of
// ${sym_name} with the value for the named symbol.  Keep going until
// no more replacements are possible.
//
static string replace_symbol_refs(const string &_src)
{
	string result { _src };
	bool replaced;
	do {
		replaced = false;
		string new_result = result;

		for (auto sym_i = symbol_map.begin(); sym_i != symbol_map.end(); sym_i++) {
			repl_symbol_t &sym = sym_i->second;
#ifndef USE_QT_REGEX
			new_result = regex_replace (new_result, sym.match_expr, sym.value);
#else
			QString new_result_qstr = QString::fromStdString(new_result);
			new_result = new_result_qstr.replace(sym.match_expr,
												 QString::fromStdString(sym.value)).toStdString();
#endif
		}
		replaced |= (new_result != result);
		result = new_result;
	} while (replaced);

	return result;
}


static bool use_hwref_file(const string &fn) {
    return hwref_file_blacklist.find(fn) == hwref_file_blacklist.end();
}

static set<string> all_hwref_files;
static set<string> seed_hwref_files;


multimap<uint64_t, defn_t *> reg_val_index;


void gpuid_t::get_hwref_files()
{
	QFileInfoList fis;
	_d.hwref_dir = replace_symbol_refs(_d.hwref_dir);
	QDir hwref_dir(QString::fromStdString(_d.hwref_dir));
	if (!hwref_dir.exists()) {
		cerr << "error: hwref dir [" << _d.hwref_dir << "] missing.";
		exit(1);
		return; /*NOT reached */
	}
	QStringList nf { "*.h" };

	QFileInfoList fi_list = hwref_dir.entryInfoList(nf, QDir::Files | QDir::Readable, QDir::Name);
	for ( auto &fi: fi_list ) {
		string fn = fi.fileName().toStdString();
		auto found = find(ip_whitelist::whitelist_gpus.begin(), ip_whitelist::whitelist_gpus.end(), _d.name);
		if ( found != ip_whitelist::whitelist_gpus.end() ) {
			seed_hwref_files.insert(fn);
		}
		all_hwref_files.insert(fn);
		if ( use_hwref_file(fn) ) {
			_hwref_files.insert(fn);
		}
	}
}


static void init_gpuids()
{
	for (size_t v =0; v < gpuid_table.size(); v++ )
	{
		gpuid_t::ctor_t &gpu_ctor = gpuid_table[v];
		gpuid_t *new_gpu = new gpuid_t(gpu_ctor);
		new_gpu->set_ordering_index(v);
		new_gpu->get_hwref_files();
		target_gpus.push_back(new_gpu);
		target_gpus_by_name[new_gpu->name()] = new_gpu;
		target_gpus_by_ordering.push_back(new_gpu);
		if( ! new_gpu->family() ) {
			cerr << "error: all gpus must be assigned to a family [" << new_gpu->name() << "]" << endl;
			exit(1);
		}
		new_gpu->family()->insert(new_gpu);
	}
}

#define qstdstr(X) QString::fromStdString(X)
#define stdstr(X)  X.toStdString()

string FooLexer::simplified()
{
	stringstream r;
	for ( auto &t: _tokens ) {
		switch ( t._t ) {
			case token_type_e::keyword :
				r << "[" << t._s << "]";
				break;
			case token_type_e::hashop :
				r << "#";
				break;
			case token_type_e::hex_literal :
				r << "hex[" << t._s << "]";
				break;
			case token_type_e::dec_literal :
				r << "dec[" << t._s << "]";
				break;
			case token_type_e::identifier :
				r << "id[" << t._s << "]";
				break;
			case token_type_e::grouping :
				r << t._s;
				break;
			case token_type_e::squot_string :
				r << "'[" << t._s << "]";
				break;
			case token_type_e::dquot_string :
				r << "\"[" << t._s << "]";
				break;
			case token_type_e::lop :
			case token_type_e::uop :
			case token_type_e::mop :
				r << "op[" << t._s << "]";
				break;
			case token_type_e::whitespace :
				r << "[ ]";
				break;
			case token_type_e::comment :
				r << "comment [" << t._s << "]";
				break;
			case token_type_e::newline :
				r << endl;
				break;
			case token_type_e::unk :
			default:
				cerr << "error: unknown or unrecognized token/parse result:" << t._s;
				exit(1);
				break;
		}
	}
	return r.str();
}

// should be >= 32 otherwise bitfields show up in hex (e.g.: "0x10:0x11" )
static int hex_switchover_value = 32; // highest decimal is this

static string normalize_hex(string hex_in)
{
	uint64_t n; // ?  sufficient to avoid range problems?
	stringstream in(hex_in);
	in >> hex >> n;
	stringstream out;
	if ( n > uint64_t(hex_switchover_value) ) {
		out << hex << "0x" << n;
	} else {
		out << dec << n;
	}
	return out.str();
}

static string normalize_dec(string dec_in)
{
	uint64_t n; // ?  sufficient to avoid range problems?
	stringstream in(dec_in);
	in >> dec >> n;
	stringstream out;
	if ( n > uint64_t(hex_switchover_value) ) {
		out << hex << "0x" << n;
	} else {
		out << dec << n;
	}
	return out.str();
}

static string normalize_token(list<FooLexer::token_t>::iterator &t)
{
	if ( lexer->is_hex_literal(t) ) {
		return normalize_hex(lexer->token_str(t));
	} else if ( lexer->is_dec_literal(t) ) {
		return normalize_dec(lexer->token_str(t));
	}
	return lexer->token_str(t);
}

//
// dev_fb.h -> "fb"  dev_fb_addendum.h -> "fb_addendum" etc.
//
static string tag(const string &def_file_name)
{
	string foo = def_file_name;
	size_t f = foo.find("dev_");
	if ( f == string::npos ) {
		// handle a few specially
		if ( foo == "hwproject.h" ) {
			return "proj";
		}
		return "tagging_is_BORKED";
	}
	foo = foo.substr(f+4, -1);
	size_t h = foo.find(".h");

	if ( h == string::npos ) {
		return "tagging_is_WT";
	}
	return foo.substr(0, foo.length() - 2);
}


static void process_gpu_defs(gpuid_t *g, string def_file_name)
{
	QDir hwref_dir( qstdstr( g->hwref_dir() ) );
	if (!hwref_dir.exists()) {
		// already tested, but just to be sure...
		throw logic_error("no hwref dir?");
	}

	string full_path = hwref_dir.filePath( qstdstr(def_file_name)).toStdString();
	QFile def_file(hwref_dir.filePath( qstdstr(def_file_name)));

	QFileInfo fi(QString::fromStdString(full_path));
	if ( !fi.exists() ) {
		// typically ok.  some headers are new or name-changed per-chip.
		// but we scan over all possibilities, so...
		return;
	}

	ifstream ifs;
	ifs.open (full_path, ifstream::in);
	if ( ! (ifs.good() || ifs.eof() ) ) {
		cerr << "error: couldn't open " << full_path << " for reading." << endl;
		exit(1);
		return;
	}

	stringstream outstring;
	if ( lexer )
		delete lexer;
	lexer = new FooLexer(&ifs, &outstring);

	while (lexer->yylex() != 0) { }

	if ( outstring.str().size() ) {
		//???
		cerr << "#info lex[" << outstring.str() << "]" << endl;
	}
	ifs.close();

	// the file is now represented as a list of tokens.  line continuation
	// is handled by turning the continuation into whitespace.
	// we're interested here in token sequences which fit the pattern:
	// [][hashop][][keyword:define][][symbol][][...][nl]
	//
	
	auto t = lexer->begin();
	vector< list<FooLexer::token_t>::iterator > line_toks;
	while ( t != lexer->end() ) {
		string def_match, val_match, ident_match;
		line_toks.clear();
		
		auto l = t;
		while ( !lexer->is_newline(l) && (l != lexer->end()) ) {
			if ( ! lexer->is_comment(l) )
				line_toks.push_back(l);
			l++;
		}
		//
		// line_toks has no comments, and only covers a single line (incl continuations)
		// we care about a few patterns...
		// []* [hashop] []* [define] []+ [ident] ( []* [ident]? []* ) []+ ![]*
		// []* [hashop] []* [define] []+ [ident] []* ![]*
		//
		int hash = -1, define = -1, define_ident = -1, open = -1, ident = -1, close = -1;
		int cur = 0, end = line_toks.size();
		int matches = false;
		do {
			if ( cur == end ) break;
			if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end) break;
			if ( ! lexer->is_hashop(line_toks[cur]) ) break;
			hash = cur++; if ( cur == end ) break;
			if ( lexer->is_whitespace(line_toks[cur])) if (++cur == end) break;
			if ( !lexer->is_keyword(line_toks[cur], "define")) break;
			define = cur++; if ( cur == end ) break;
			if ( lexer->is_whitespace(line_toks[cur])) if (++cur == end) break;
			if ( !lexer->is_identifier(line_toks[cur])) break;
			define_ident = cur++; if ( cur == end ) break;
			// if '(' immediately follows the identifier, then it is a fn-like macro.
			// otherwise it's an object like macro.

			if ( lexer->is_grouping(line_toks[cur], "(" ) ) {
				open = cur++; if ( cur == end ) break;
				if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end ) break;
				if ( lexer->is_identifier(line_toks[cur]) )
					ident = cur++; if ( cur == end ) break;
				if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end ) break;
				if ( lexer->is_grouping(line_toks[cur], ")") ) {
					close = cur++; if ( cur == end ) break;
				} else break;
				// TBD: multiple vars...
			}

			def_match = lexer->token_str(line_toks[define_ident]);
			if ( ident != -1 ) ident_match = lexer->token_str(line_toks[ident]);
			matches = true;

			if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end ) break;

			for ( /*cur = cur*/; cur != end; cur++ ) {
				val_match += normalize_token(line_toks[cur]);
			}

		} while (0);


		//
		// leave this alone... or, break to this point...
		//
		if (l != lexer->end() )
			l++;
		t = l;

		if ( !matches ) {
			// cerr << "warning: mismatch on line [" <<
			continue;
		}

		string idstring;
		if ( ident_match.size() ) {
			// quick assumption check: one symbol: == "i";
#if 0
			//XXX need to add a way to handle multiple macro args!
			if ( ident_match != "i" && ident_match != "G" ) {
				cerr << "warning: not expecting anything other than \"i\" or"
					" \"G\" as macro argument." << endl;
				cerr << "warning : saw \"" << ident_match <<
					"\" in def'n for [" << def_match << "]" << endl;

			}
#endif
			idstring = "(" + ident_match + ")";
		}

		if ( ip_whitelist::symbol_whitelist.find(def_match) == ip_whitelist::symbol_whitelist.end() ) {
			continue;
		}

		// is it a register or a field or a constant? (theoretically could be multiple types?)
		// the whitelist has that information.
		bool is_reg = false, is_field = false, is_constant = false;
		if ( ip_whitelist::chip_regs.find(def_match) != ip_whitelist::chip_regs.end() ) {
			is_reg = true;
		} else if (ip_whitelist::chip_fields.find(def_match) != ip_whitelist::chip_fields.end()) {
			is_field = true;
		} else if (ip_whitelist::chip_constants.find(def_match) != ip_whitelist::chip_constants.end()) {
			is_constant = true;
		}

		//XXX is this correct?
		// now, add the (i) to the symbol if it should be.
		// () should match (i) and (i,j) as well since they would collide.
		def_match += idstring;

		defn_t *symbol_defn       = 0;
		//defn_index_t *gpu_defns_index = g->defns_index();
		defn_set_t *gpu_defns         = g->defines();

		defn_index_t *defns_index = &__defn_index;
		defn_set_t *defns = &__defines;

		defn_index_t::iterator find_defn = defns_index->find(def_match);
		if ( find_defn == defns_index->end() ) { // first (global) encounter with the symbol
			symbol_defn = (*defns_index)[def_match] = new defn_t(def_match);
			symbol_defn->is_reg      = is_reg;
			symbol_defn->is_field    = is_field;
			symbol_defn->is_constant = is_constant;
			defns->insert(symbol_defn);
		} else {
			symbol_defn = find_defn->second;
		}
		gpu_defns->insert(symbol_defn);


		defn_val_index_t::iterator find_val = symbol_defn->val_index.find(val_match);
		defn_val_t *defn_val = 0;
		if ( find_val == symbol_defn->val_index.end() ) { // first encounter with this value (for the symbol)
			defn_val = symbol_defn->val_index[val_match] = new defn_val_t(val_match);
			symbol_defn->vals.insert(defn_val);

			if ( symbol_defn->is_reg ) {
				try {
					uint32_t vnum = stoull (val_match, nullptr, 0);
					reg_val_index.insert(make_pair( vnum, symbol_defn ));
				} catch ( invalid_argument w) {
					// bogus val... might want to complain here?
				}
			}
		} else {
			defn_val = find_val->second;
		}
		defn_val->gpus.insert(g);
	}
}

static void calculate_equiv_classes(defn_set_t *defs,  string def_file_name );
defn_set_t *read_ip_defs()
{
	init_symbols();
	init_gpuids();

	ip_whitelist::init();

	list<string> *potential_files = new list<string>();
	for ( auto &fn : seed_hwref_files ) {
		if ( use_hwref_file(fn) ) {
			potential_files->push_back(fn);
		}
	}

	for ( auto &fn : *potential_files ) {
		if ( hwref_file_blacklist.find(fn) != hwref_file_blacklist.end())
			continue;
		for ( auto g = target_gpus.begin(); g != target_gpus.end(); g++ ) {
			process_gpu_defs(*g, fn);
		}
	}

	for ( auto def : __defines ) {
		defn_set_t defs;  defs.insert(def);
		calculate_equiv_classes(&defs, "uh");
	}

	return &__defines;
}


void FooLexer::push_hex_literal(const char *s) {
	_tokens.push_back(token_t(token_type_e::hex_literal, string(s)));
}
void FooLexer::push_dec_literal(const char *s) {
	_tokens.push_back(token_t(token_type_e::dec_literal, string(s)));
}
void FooLexer::push_identifier(const char *s) {
	_tokens.push_back(token_t(token_type_e::identifier, string(s)));
}
void FooLexer::push_grouping(const char *s) {
	_tokens.push_back(token_t(token_type_e::grouping, string(s)));
}
void FooLexer::push_lop(const char *s) {
	_tokens.push_back(token_t(token_type_e::lop, string(s)));
}
void FooLexer::push_uop(const char *s) {
	_tokens.push_back(token_t(token_type_e::uop, string(s)));
}
void FooLexer::push_mop(const char *s) {
	_tokens.push_back(token_t(token_type_e::mop, string(s)));
}
void FooLexer::push_unk(const char *s) {
	_tokens.push_back(token_t(token_type_e::unk, string(s)));
}
void FooLexer::push_hashop() {
	_tokens.push_back(token_t(token_type_e::hashop, string("#")));
}
void FooLexer::push_keyword(const char *s) {
	_tokens.push_back(token_t(token_type_e::keyword, string(s)));
}
void FooLexer::push_whitespace() {
	_tokens.push_back(token_t(token_type_e::whitespace, string()));
}
void FooLexer::push_comment(const char *) {
	_tokens.push_back(token_t(token_type_e::comment, string()));
}

void FooLexer::push_dquot_string(const char *) {
	_tokens.push_back(token_t(token_type_e::dquot_string, string()));
}
void FooLexer::push_squot_string(const char *){
	_tokens.push_back(token_t(token_type_e::squot_string, string()));
}
void FooLexer::push_newline() {
	_tokens.push_back(token_t(token_type_e::newline, string()));
}

//
// the comparison here is lexical, post cast-to-string form.
// only the set of gpus and families is considered.  the equiv
// class name and family order are not part of the comparison.
//
bool gpu_equiv_class_t::operator ==(const gpu_equiv_class_t &o) const
{
	return ! ((*this < o) || (o < *this));
}
bool gpu_equiv_class_t::operator <(const gpu_equiv_class_t &o) const
{
	string gpus_str, ogpus_str;
	string families_str, ofamilies_str;
	set<string> gpus, ogpus;
	set<string> families, ofamilies;

	for ( auto &f : _families)  families.insert(f->name());
	for ( auto &f : families)	families_str += f;
	for ( auto &f : o._families) ofamilies.insert(f->name());
	for ( auto &f : ofamilies)	ofamilies_str += f;

	for ( auto &g : _gpus)   gpus.insert (g->name());
	for ( auto &g : gpus)    gpus_str += g;
	for ( auto &g : o._gpus) ogpus.insert(g->name());
	for ( auto &g : ogpus)   ogpus_str += g;

	if ( families_str < ofamilies_str )
		return true;

	if ( families_str == ofamilies_str ) {
		if ( gpus_str < ogpus_str)
			return true;
	}
	return false;
}

void gpu_equiv_class_t::insert(gpuid_t *gpu)
{
    if ( (_gpus.find(gpu) != _gpus.end()) ||
		 (_families.find(gpu->family()) != _families.end()) )
		return;

	// if adding this gpu would cause its family to become
	// fully represented, then remove the other gpus in the
	// same family and add the family instead.
	gpu_set_t result_gpus = _gpus;
	result_gpus.insert(gpu);

	if ( gpu->family()->fully_represented_in(result_gpus) ) {
		_families.insert(gpu->family());
		for ( auto &gr : gpu->family()->gpus()) {
			_gpus.erase(gr);
		}
	} else {
		_gpus.insert(gpu);
	}
}


//
// calculate gpu equivalence classes implicitly defined by the set of def'ns...
// this isn't terribly useful unless only a single (or small # of) engine(s) is
// being analyzed at a time.
//
static void calculate_equiv_classes(defn_set_t *defs,  string def_file_name ) {
	gpu_equiv_class_t::snapshot_reset();
	for ( auto &defn : (*defs) ) {
		for ( auto &val : defn->vals ) {
			gpu_equiv_class_t tmp_eq_class;
			for ( auto &g : val->gpus ) { tmp_eq_class.insert(g); }
			val->eq_class = tmp_eq_class.snap(tag(def_file_name));
		}
	}
}

vector<gpu_equiv_class_t *> gpu_equiv_class_t::_snapshot;
void gpu_equiv_class_t::snapshot_reset() { 	_snapshot.clear(); }

gpu_equiv_class_t *gpu_equiv_class_t::snap(const string &/*tag*/)
{
	for ( size_t i = 0; i < _snapshot.size(); i++) {
		if ( *this == *(_snapshot[i]) ) {
			return _snapshot[i];
		}
	}
#if 0
	int order = _snapshot.size();
	stringstream refstr; refstr << "__nv_" << tag << "_compat_" << order;
	stringstream defstr; defstr << "#define __nv_" << tag << "_compat_" << order << "(x)  ((x)!=(x)\\";
	for ( auto &family : _families ) {	defstr << "\n\t\t|| " << family->ref() << "(x)\\";	}
	for ( auto &gpu : _gpus ) {	defstr << "\n\t\t|| ((x)==" << gpu->ref() << ")\\";	}
	defstr << "\n\t)" << endl;
	_def = defstr.str();
	_ref = refstr.str();
#endif
	gpu_equiv_class_t *snapped = new gpu_equiv_class_t(*this);
	_snapshot.push_back(snapped);
	return snapped;
}
