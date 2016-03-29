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
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <queue>

#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QDataStream>
#include <QString>
#include <QMap>

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


list<gpuid_t*> target_gpus;
map<string, gpuid_t*> target_gpus_by_name;
vector<gpuid_t*> target_gpus_by_ordering;


static gpu_family_t
	legacy_family {"legacy"},
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
	{ "legacy"  , 0x0000 , "", &legacy_family },

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
			case token_type_e::comma :
				r << ",";
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
#if 0
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
#endif
//
// evaluation here is sending a test program off to be compiled
// and then run.  i.e. it's expensive. so, the results are cached
// for later runs.
//
typedef QMap<string, evaluation_result_t> eval_result_map_t;

static eval_result_map_t prior_evaluations;
int prior_evaluation_hits = 0;
int prior_evaluation_misses = 0;


static eval_result_map_t::iterator
evaluate_str(const string &def_match, const string &val_match,
			 bool field, size_t num_eval_args, const string &idstring)
{
	eval_result_map_t::iterator pef = prior_evaluations.find(val_match);

	if ( pef != prior_evaluations.end() ) {
		prior_evaluation_hits++;
		return pef;
	}
	prior_evaluation_misses++;
	// hmm.
	string compile_me = "clang++ -xc++ -o foo2 foo2.cpp";
	string run_me = "./foo2";
	string evaluate_me;
	evaluate_me += "#include <iostream>\n";
	evaluate_me += "#include <string>\n";

	if ( !field ) {
		evaluate_me += "#define " + def_match + idstring + " " + val_match + "\n";
		if ( !num_eval_args ) {
			evaluate_me += "int64_t evaluate_me(){ return " + def_match + ";}\n";
		} else if (num_eval_args == 1) {
			evaluate_me += "int64_t evaluate_me(){ return " + def_match + "(0);}\n";
		} else if (num_eval_args == 2) {
			evaluate_me += "int64_t evaluate_me(){ return " + def_match + "(0,0);}\n";
		} else {
			throw runtime_error("how many eval args?");
		}
		evaluate_me += "int main(int argc, char **argv) {"
			" std::cout << evaluate_me() << std::endl; std::cout.flush(); "
			"return 0;}\n";
	} else {
		evaluate_me += "#define " + def_match + idstring + " " + val_match + "\n";
		if ( !num_eval_args ) {
			evaluate_me += "int64_t evaluate_low() { return (0)?" + def_match + ";}\n";
			evaluate_me += "int64_t evaluate_high(){ return (1)?" + def_match + ";}\n";
		} else if (num_eval_args == 1) {
			evaluate_me += "int64_t evaluate_low() { return (0)?" + def_match + "(0);}\n";
			evaluate_me += "int64_t evaluate_high(){ return (1)?" + def_match + "(0);}\n";
		} else if (num_eval_args == 2) {
			evaluate_me += "int64_t evaluate_low() { return (0)?" + def_match + "(0,0);}\n";
			evaluate_me += "int64_t evaluate_high(){ return (1)?" + def_match + "(0,0);}\n";
		} else {
			throw runtime_error("how many field eval args?");
		}
		evaluate_me += "int main(int argc, char **argv) {"
			" std::cout << evaluate_low() << std::endl <<"
			" evaluate_high() << std::endl; std::cout.flush(); "
			"return 0; }\n";
	}

	ofstream progf("foo2.cpp");
	progf << evaluate_me;
	progf.close();
	string result; 
	bool evaluated = false;
	vector<int64_t> vals;
	try {
		int rc = system(compile_me.c_str());
		if ( rc ) {
			cerr << "error: evaluation program for " << def_match <<
				" failed to compile" << endl;
			throw runtime_error("compilation rc wasn't zero");
		}
		FILE *eval_out = popen(run_me.c_str(), "r");
		if ( !eval_out ) {
			cerr << "error: evaluation program for " << def_match <<
				" didn't run propely" << endl;
			throw runtime_error("busted evaluation program?");
		}
		int c;
		do {
			c = fgetc (eval_out);
			if ( c != EOF ) result += (char)c;
		} while (c != EOF);
		if ( ferror(eval_out) ) {
			perror("error:");
			throw runtime_error("error reading stream");
		}
		pclose (eval_out); // careful, pclose is required.
		evaluated = true;
	} catch ( runtime_error w ) {
	}

	if ( evaluated ){
		stringstream ss(result);
		do {
			int64_t val;
			ss >> val;
			if (!ss.fail()) vals.push_back(val);
		} while (!ss.fail());

		if ( vals.size() == 2 ) {
			prior_evaluations[val_match].field = true;
			prior_evaluations[val_match].result.field.low  = vals[0];
			prior_evaluations[val_match].result.field.high = vals[1];
		} else if (vals.size() == 1) {
			prior_evaluations[val_match].field = false;
			prior_evaluations[val_match].result.val = vals[0];
		} else {
			// warning or something?
		}
	}
	return prior_evaluations.find(val_match);
}

QDataStream & operator<< (QDataStream& stream, const evaluation_result_t &r)
{
	stream << r.field;
	if ( r.field ) {
		stream << r.result.field.low;
		stream << r.result.field.high;
	} else {
		stream << r.result.val;
	}
	return stream;
}

QDataStream & operator>> (QDataStream& stream, evaluation_result_t &r)
{
	stream >> r.field;
	if ( r.field ) {
		stream >> r.result.field.low;
		stream >> r.result.field.high;
	} else {
		stream >> r.result.val;
	}
	return stream;
}


//
// the "instance" functions populate the paths described by the whitelist def hierarchy.
//
// during this phase of tree creation there is only *one possible member* for the
// "*_by_name" sets, because there is only one gpu under consideration.  so any element
// present there already is what we're after.
//

group_def_t *def_tree_t::instance_group(ip_whitelist::group_t *group)
{
	string name = group->name;
	assert(groups_by_name[name].size() <= 1);
	auto group_by_name = groups_by_name[name].begin();
	group_def_t *group_def;
	if ( group_by_name == groups_by_name[name].end() ) {
		group_def = new group_def_t(this, name, gpus);
		defs_by_name[name].insert(group_def);
		groups_by_name[name].insert(group_def);
		groups.push_back(group_def);
	} else {
		group_def = *group_by_name;
	}
	return group_def;
}



reg_def_t *def_tree_t::instance_reg(ip_whitelist::reg_t *reg)
{
	string name = reg->def;
	if ( !name.size() ) {
		throw runtime_error("null reg length [" + reg->def + "]");
	}

	auto reg_by_name = regs_by_name[name].begin();
	reg_def_t *reg_def;
	if ( reg_by_name == regs_by_name[name].end() ) {
		reg_def = new reg_def_t(this, reg->def, gpus);
		defs_by_name[name].insert(reg_def);
		regs_by_name[name].insert(reg_def);
		if ( reg->group ) {
			group_def_t *parent_group = instance_group(reg->group);
			parent_group->regs_by_name[name].insert(reg_def);
		} else {
			throw domain_error("parentless reg" + reg->name);
		}
	} else {
		reg_def = *reg_by_name;
	}
	return reg_def;
}

field_def_t *def_tree_t::instance_field(ip_whitelist::field_t *field)
{
	string name = field->def;
	auto field_by_name = fields_by_name[name].begin();
	field_def_t *field_def;
	if ( field_by_name == fields_by_name[name].end() ) {
		field_def = new field_def_t(this, field->def, gpus);
		defs_by_name[name].insert(field_def);
		fields_by_name[name].insert(field_def);
		if ( field->reg ) {
				reg_def_t *parent_reg = instance_reg(field->reg);
				parent_reg->fields_by_name[name].insert(field_def);
		} else if (field->group) {
			group_def_t *parent_group = instance_group(field->group);
			parent_group->fields_by_name[name].insert(field_def);
		} else {
			throw domain_error("parentless field" + field->name);
		}
	} else {
		field_def = *field_by_name;
	}
	return field_def;
}

const_def_t *def_tree_t::instance_const(ip_whitelist::constant_t *constant)
{
	string name = constant->def;
	auto constant_by_name = constants_by_name[name].begin();
	const_def_t *const_def;
	if ( constant_by_name == constants_by_name[name].end() ) {
		const_def = new const_def_t(this, constant->def, gpus);
		defs_by_name[name].insert(const_def);
		constants_by_name[name].insert(const_def);
		if ( constant->field ) {
			field_def_t *parent_field = instance_field(constant->field);
			parent_field->constants_by_name[name].insert(const_def);
		} else if ( constant->reg ) {
			reg_def_t *parent_reg = instance_reg(constant->reg);
			parent_reg->constants_by_name[name].insert(const_def);
		} else if ( constant->group) {
			group_def_t *parent_group = instance_group(constant->group);
			parent_group->constants_by_name[name].insert(const_def);
		} else {
			throw domain_error("parentless constant" + constant->name);
		}
	} else {
		const_def = *constant_by_name;
	}
	return const_def;
}

def_tree_t * process_gpu_defs(gpuid_t *g, string def_file_name)
{
	def_tree_t *def_tree = new def_tree_t();
	def_tree->gpus.insert(g);

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
		return def_tree;
	}

	ifstream ifs;
	ifs.open (full_path, ifstream::in);
	if ( ! (ifs.good() || ifs.eof() ) ) {
		cerr << "error: couldn't open " << full_path << " for reading." << endl;
		exit(1);
		return def_tree;
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
		vector<string> idents;
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
		// []* [hashop] []* [define] []+ [ident] ( []* [ident]? [,[]* [ident]]+ []* ) []+ ![]*
		// []* [hashop] []* [define] []+ [ident] []* ![]*
		//
		int hash = -1, define = -1, define_ident = -1, open = -1, /*ident = -1, */close = -1;
		int cur = 0, end = line_toks.size();
		int matches = false;
		vector<int> ident;
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

			next_arg_ident: // for each new argument to the definition...

				if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end ) break;

				if ( lexer->is_identifier(line_toks[cur]) )
					ident.push_back(cur++); if ( cur == end ) break;

				if ( lexer->is_whitespace(line_toks[cur]) ) if ( ++cur == end ) break;
				//
				// a macro definition can't have any other grouping inside the argument
				// declaration list.  so, though generally dodgy, looking for ","
				// and the ending ")" here is ok.
				//
				if ( lexer->is_grouping(line_toks[cur], ")") ) {
					close = cur++; if ( cur == end ) break;
				} else if ( lexer->is_comma(line_toks[cur]) ) {
					if ( ++cur == end ) break;
					goto next_arg_ident;
				} else break;
			}

			def_match = lexer->token_str(line_toks[define_ident]);
			if ( ident.size() ) {
				ident_match = lexer->token_str(line_toks[ident[0]]);
				idents.push_back(lexer->token_str(line_toks[ident[0]]));

				for ( size_t ii = 1; ii < ident.size(); ii++) {
					ident_match += "," + lexer->token_str(line_toks[ident[ii]]);
					idents.push_back(lexer->token_str(line_toks[ident[ii]]));
				}
			}
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
			// ??? cerr << "warning: mismatch on line [" <<
			// happens when we encounter anything other than
			// #define ...
			continue;
		}

		string idstring;
		if ( ident_match.size() ) {
			idstring = "(" + ident_match + ")";
		}

		//
		// !!! this is where non-whitelisted symbols are rejected... !!!
		//
		// XXX/TBD: search within each group because the top-level namespace
		// shouldn't be used as it causes collisions.  that, or include
		// the namespace (group::register::...) within the symbol name.
		//
		auto find_symbol = ip_whitelist::symbol_whitelist.find(def_match);
		if ( find_symbol == ip_whitelist::symbol_whitelist.end() ) {
			continue;
		}

		//
		// is it a register or a field or a constant?  and at what scope?
		// the whitelist has that information.  but it is buried inside the
		// group definitions (if at all).
		//
		auto find_wl_group    = ip_whitelist::chip_groups.find   (def_match);
		auto find_wl_reg      = ip_whitelist::chip_regs.find     (def_match);
		auto find_wl_field    = ip_whitelist::chip_fields.find   (def_match);
		auto find_wl_constant = ip_whitelist::chip_constants.find(def_match);

		auto find_wl_deleted_group    = ip_whitelist::chip_deleted_groups.find   (def_match);
		auto find_wl_deleted_reg      = ip_whitelist::chip_deleted_regs.find     (def_match);
		auto find_wl_deleted_field    = ip_whitelist::chip_deleted_fields.find   (def_match);
		auto find_wl_deleted_constant = ip_whitelist::chip_deleted_constants.find(def_match);

		bool is_group    = find_wl_group    != ip_whitelist::chip_groups.end();
		bool is_reg      = find_wl_reg      != ip_whitelist::chip_regs.end();
		bool is_field    = find_wl_field    != ip_whitelist::chip_fields.end();
		bool is_constant = find_wl_constant != ip_whitelist::chip_constants.end();

		bool is_deleted_group    = find_wl_deleted_group    != ip_whitelist::chip_deleted_groups.end();
		bool is_deleted_reg      = find_wl_deleted_reg      != ip_whitelist::chip_deleted_regs.end();
		bool is_deleted_field    = find_wl_deleted_field    != ip_whitelist::chip_deleted_fields.end();
		bool is_deleted_constant = find_wl_deleted_constant != ip_whitelist::chip_deleted_constants.end();

		int category_matches =
			int(is_group) + int(is_deleted_group) +
			int(is_reg) + 	int(is_deleted_reg)   + 
			int(is_field) + int(is_deleted_field) +
			int(is_constant) + int(is_deleted_constant);

		if ( category_matches > 1 ) {
			// the only acceptable way to handle this occurs if one of the
			// matches is a delete... so, one whitelist among them has it
			// deleted and another likely is a (to-be) deprecated/unused reg.
			bool ok = (category_matches == 2) &&
				( ( is_group && is_deleted_group) ||
				  ( is_reg && is_deleted_reg ) ||
				  ( is_field && is_deleted_field ) ||
				  ( is_constant && is_deleted_constant ));
			if ( !ok ) {
				throw domain_error("symbol matches more than one category in the whitelist:" + def_match);
			}
		}

		bool skip_symbol = false;
		if ( is_group ) {
			skip_symbol = find_wl_group->second->skip_symbol;
		} else if (is_reg) {
			skip_symbol = find_wl_reg->second->skip_symbol;
		} else if (is_field) {
			// skip_symbol = find_wl_field->second->skip_symbol;
		} else if (is_constant) {
			// skip_symbol = find_wl_constant->second->skip_symbol;
		}

		if ( skip_symbol ) {
			continue;
		}

		eval_result_map_t::iterator pef;
		evaluation_result_t val;
		if ( !(is_group || is_deleted_group)) {
			try {
				pef = evaluate_str(def_match, val_match, is_field, idents.size(), idstring);
			} catch(...) {
				continue;
			}
			val = *pef;
		} else {
			// groups don't necessarily have anything other than symbolic value...
			val.field = 0;
			val.result.val = -1;
		}

		if ( is_reg ) {
			reg_def_t *reg_def = def_tree->instance_reg(find_wl_reg->second);
			reg_def->set_val(val.result.val);
		} else if (is_deleted_reg) {
			reg_def_t *reg_def = def_tree->instance_reg(find_wl_deleted_reg->second);
			reg_def->set_val(val.result.val);
		} else if ( is_field ) {
			ip_whitelist::field_t *field = find_wl_field->second;
			field_def_t *field_def = def_tree->instance_field(field);
			field_def->set_field(val.result.field.high, val.result.field.low);
		} else if ( is_deleted_field ) {
			ip_whitelist::field_t *field = find_wl_deleted_field->second;
			field_def_t *field_def = def_tree->instance_field(field);
			field_def->set_field(val.result.field.high, val.result.field.low);
		} else if ( is_constant ) {
			ip_whitelist::constant_t *constant = find_wl_constant->second;
			const_def_t *const_def = def_tree->instance_const(constant);
			const_def->set_val(val.result.val);
		} else if ( is_deleted_constant) {
			ip_whitelist::constant_t *constant = find_wl_deleted_constant->second;
			const_def_t *const_def = def_tree->instance_const(constant);
			const_def->set_val(val.result.val);
		} else if (is_group) {
			ip_whitelist::group_t *group = find_wl_group->second;
			/*group_def_t *group_def =*/ def_tree->instance_group(group);
		} else if ( is_deleted_group ) {
			ip_whitelist::group_t *group = find_wl_deleted_group->second;
			/*group_def_t *group_def =*/ def_tree->instance_group(group);
		} else {
			throw domain_error("unknown symbol type for" + def_match);
		}
	}
	return def_tree;
}


// datastream torque converter
QDataStream & operator<< (QDataStream& stream, const string &s)
{
	stream << QString::fromStdString(s);
	return stream;
}
QDataStream & operator>> (QDataStream& stream, string &s)
{
	QString t;
	stream >> t;
	s = t.toStdString();
	return stream;
}

void init_evaluation_cache()
{
	QFile file("eval.dat");
	if (!file.open(QIODevice::ReadOnly)) {
		return;
	}
	QDataStream in(&file);
	in >> prior_evaluations;
}

void save_evaluation_cache()
{
	QFile file("eval.dat");
	if (!file.open(QIODevice::WriteOnly)) {
		return;
	}
	
	QDataStream out(&file);
	out << prior_evaluations;
}

vector<def_tree_t *> read_ip_defs()
{
	vector<def_tree_t *> gpu_def_trees;

	init_evaluation_cache();
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
			def_tree_t *t = process_gpu_defs(*g, fn);
			gpu_def_trees.push_back(t);
		}
	}

	save_evaluation_cache();
	return gpu_def_trees;
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
void FooLexer::push_comma() {
	_tokens.push_back(token_t(token_type_e::comma, string()));
}


//
// construct a new aggregate tree from a vector of per-gpu trees.
//
def_tree_t::def_tree_t(vector<def_tree_t *> &gpu_def_trees)
{
	for ( auto gpu_def_tree : gpu_def_trees ) {

		gpus.insert( gpu_def_tree->gpus.begin(), gpu_def_tree->gpus.end() );

		// the def tree at this point only contains top-level
		// defs for the groups.  flatten everything out and get
		// those to bubble up to the "defs_by_name" level.
#if 0
		queue<def_t *> expansion_q;
		for ( auto group : gpu_def_tree->groups ) {
			expansion_q.push(group);
		}
		while ( expansion_q.size() ) {
			def_t *expand_def = expansion_q.front();
			expansion_q.pop();
			string name = expand_def->symbol;
			if ( group_def_t *expand_group = dynamic_cast<group_def_t*>(expand_def) ) {
				expand_group->enqueue_children(expansion_q);
				defs_by_name[name].insert(expand_group);
				groups_by_name[name].insert(expand_group);
			} else if ( reg_def_t *expand_reg = dynamic_cast<reg_def_t*>(expand_def) ) {
				expand_reg->enqueue_children(expansion_q);
				defs_by_name[name].insert(expand_reg);
				regs_by_name[name].insert(expand_reg);
			} else if ( field_def_t *expand_field = dynamic_cast<field_def_t*>(expand_def) ) {
				expand_field->enqueue_children(expansion_q);
				defs_by_name[name].insert(expand_field);
				fields_by_name[name].insert(expand_field);
			} else if ( const_def_t *expand_constant = dynamic_cast<const_def_t*>(expand_def)) {
				expand_constant->enqueue_children(expansion_q);
				defs_by_name[name].insert(expand_constant);
				constants_by_name[name].insert(expand_constant);
			} else {
			}
		}


#else
		for ( auto def_set : gpu_def_tree->defs_by_name ) {
			for ( auto def : def_set.second ) {
				defs_by_name[def_set.first].insert(def);
			}
		}

		for ( auto group_set : gpu_def_tree->groups_by_name ) {
			for ( auto group : group_set.second ) {
				groups_by_name[group_set.first].insert(group);
			}
		}

		for ( auto reg_set : gpu_def_tree->regs_by_name ) {
			for ( auto reg : reg_set.second ) {
				regs_by_name[reg_set.first].insert(reg);
			}
		}
		for ( auto field_set : gpu_def_tree->fields_by_name ) {
			for (auto field : field_set.second ) {
				fields_by_name[field_set.first].insert(field);
			}
		}
		for (auto constant_set : gpu_def_tree->constants_by_name ) {
			for ( auto constant : constant_set.second ) {
				constants_by_name[constant_set.first].insert(constant);
			}
		}
#if 0
		for ( auto deleted_group_set : gpu_def_tree->deleted_groups_by_name ) {
			for ( auto deleted_group : deleted_group_set.second ) {
				deleted_groups_by_name[deleted_group_set.first].insert(deleted_group);
			}
		}
		for ( auto deleted_reg_set : gpu_def_tree->deleted_regs_by_name ) {
			for ( auto deleted_reg : deleted_reg_set.second ) {
				deleted_regs_by_name[deleted_reg_set.first].insert(deleted_reg);
			}
		}
		for ( auto deleted_field_set : gpu_def_tree->deleted_fields_by_name ) {
			for ( auto deleted_field : deleted_field_set.second ) {
				deleted_fields_by_name[deleted_field_set.first].insert(deleted_field);
			}
		}
		for ( auto deleted_constant_set : gpu_def_tree->deleted_constants_by_name ) {
			for ( auto deleted_constant : deleted_constant_set.second ) {
				deleted_constants_by_name[deleted_constant_set.first].insert(deleted_constant);
			}
		}
#endif
#endif

	}

}


def_t *def_tree_t::map_to_new(def_t *orig_def, map< def_t *, def_t * > &correlates)
{
	def_t *new_def = correlates[orig_def];
	assert(new_def);
	if ( correlates.find(new_def) != correlates.end() ) {
		new_def = correlates[new_def];
	}
	assert(new_def);
	return new_def;
}


//
// create a duplicate of the original and then coalesce where possible.
//
def_tree_t::def_tree_t(def_tree_t *agg_tree)
{
	//
	// setting up outside the tree so only coalesced entries are inserted.
	//
	map< string, def_set_t  >       new_defs_by_name;
	map< string, group_def_set_t >  new_groups_by_name;
	map< string, reg_def_set_t >    new_regs_by_name;
	map< string, field_def_set_t >  new_fields_by_name;
	map< string, const_def_set_t >  new_consts_by_name;


	map< def_t *, def_t * >   correlates;     // original->new (1:1)
	map< def_t *, def_set_t > rev_correlates; // new->original (1:1) and new->new (1:many)
	gpus = agg_tree->gpus;

	for ( auto agg_def_set : agg_tree->defs_by_name ) {
		string name = agg_def_set.first;

		for ( auto agg_def : agg_def_set.second ) {
			def_t *new_def = agg_def->clone(this);

			new_defs_by_name[name].insert(new_def);
			correlates.insert(make_pair(agg_def, new_def));
			rev_correlates[new_def].insert(agg_def);

			//
			// XXX dynamic cast no longer works here as a discriminant
			//
			//	reg_def_t    *new_deleted_reg_def    = dynamic_cast<deleted_reg_def_t *>   (new_def);

			group_def_t  *new_group_def  = dynamic_cast<group_def_t *> (new_def);
			reg_def_t    *new_reg_def    = dynamic_cast<reg_def_t *>   (new_def);
			field_def_t  *new_field_def  = dynamic_cast<field_def_t *> (new_def);
			const_def_t  *new_const_def  = dynamic_cast<const_def_t *> (new_def);

#if 0
			if      ( new_deleted_group_def    ) { }
			else if ( new_deleted_reg_def    ) { }
			else if ( new_deleted_field_def    ) { }
			else if ( new_deleted_constant_def    ) { }
			else
#endif
			if ( new_group_def  ) { new_groups_by_name [name].insert(new_group_def);  }
			else if ( new_reg_def    ) { new_regs_by_name   [name].insert(new_reg_def);    }
			else if ( new_field_def  ) { new_fields_by_name [name].insert(new_field_def);  }
			else if ( new_const_def  ) { new_consts_by_name [name].insert(new_const_def);  }
			else {assert(0);}
		}
	}

	//
	// all the (new) defs, all the time.
	//
	for ( auto nd : new_defs_by_name ) { // over each distinct def name

		string name = nd.first;

		//		set<def_t *>        &new_defs_with_name    = new_defs_by_name   [name];
		set<group_def_t*>   &new_groups_with_name  = new_groups_by_name [name];
		set<reg_def_t *>    &new_regs_with_name    = new_regs_by_name   [name];
		set<field_def_t*>   &new_fields_with_name  = new_fields_by_name [name];
		set<const_def_t*>   &new_consts_with_name  = new_consts_by_name [name];
	

		//
		// all these defs are named the same, but are defined
		// for different gpus.  and, they may have different values.
		// group based upon value.  and record the participating gpus.
		// complications can arise here if not all of the defs are
		// of the same type (register, field, etc).  so they have
		// to be sifted upon that as well, first.
		// along the way maintain original def<->new/cloned def correlation
		// for later use.
		//

		//
		// regs, all with the same name, possibly split across multiple values
		//
		map< uint64_t, reg_def_set_t > new_regs_with_value;
		for ( auto reg_def : new_regs_with_name ) {
			new_regs_with_value[reg_def->val].insert(reg_def);
		}
		for ( auto rv : new_regs_with_value ) {
			//uint64_t v = rv.first;
			reg_def_set_t &rs = rv.second;
			// take first, coalesce into it.  drop the others.
			reg_def_t *fr = *rs.begin();
			for ( auto ri = ++rs.begin(); ri != rs.end(); ri++ ){
				fr->def_gpus.insert((*ri)->def_gpus.begin(), (*ri)->def_gpus.end());
				correlates[(*ri)] = fr; // new->new, used later to implement coalescing
			}
			regs_by_name[name].insert(fr);
			defs_by_name[name].insert(fr);
		}

		//
		// fields, all with the same name, possibly split across multiple values
		//
		map< pair< size_t, size_t >, field_def_set_t > new_fields_with_value;
		for ( auto f : new_fields_with_name ) {
			new_fields_with_value[make_pair(f->high, f->low)].insert(f);
		}
		for ( auto fv : new_fields_with_value ) {
			//pair<size_t, size_t> v = fv.first;
			field_def_set_t &fs = fv.second;
			// take first, coalesce into it.  drop the others.
			field_def_t *ff = *fs.begin();// fs.erase(fs.begin());
			for ( auto fi = ++fs.begin(); fi != fs.end(); fi++ ) {
				ff->def_gpus.insert((*fi)->def_gpus.begin(), (*fi)->def_gpus.end());
				correlates[(*fi)] = ff;  // new->new, used later to implement coalescing
			}
			fields_by_name[name].insert(ff);
			defs_by_name[name].insert(ff);
		}

		//
		// constants, all with the same name, possibly split across multiple values
		//
		map< int64_t, const_def_set_t > new_consts_with_value;
		for ( auto c : new_consts_with_name ) {
			new_consts_with_value[c->val].insert(c);
		}
		for ( auto cv : new_consts_with_value ) {
			//			int64_t v = cv.first;
			const_def_set_t &cs = cv.second;
			// take first, coalesce into it.  drop the others.
			const_def_t *fc = *cs.begin();// cs.erase(cs.begin());
			for ( auto ci = ++cs.begin(); ci != cs.end(); ci++ ) {
				fc->def_gpus.insert((*ci)->def_gpus.begin(), (*ci)->def_gpus.end());
				correlates[(*ci)] = fc; // new->new, used later to implement coalescing
			}
			constants_by_name[name].insert(fc);
			defs_by_name[name].insert(fc);
		}

		//
		// groups all with the same name, no notion of "val" here
		// so this is slightly different than defs above...
		//
		group_def_set_t &gs = new_groups_with_name;
		if ( gs.begin() != gs.end() ) {
			assert(gs.begin() != gs.end());
			group_def_t *gc = *gs.begin();
			//		gs.erase(gs.begin());
			for ( auto gi = ++gs.begin(); gi != gs.end(); gi++ ) {
				gc->def_gpus.insert((*gi)->def_gpus.begin(), (*gi)->def_gpus.end());
				correlates[*gi] = gc; // new->new, used later to implement coalescing
			}
			groups_by_name[name].insert(gc);
			defs_by_name[name].insert(gc);
			groups.push_back(gc);
		}
	} // for each def name

	//
	// groups and the other defs have largely unrelated names.  so
	// after all the names have been traversed is the only time we can safely
	// work on group membership for the regs (and other non-group defs).
	//

	for ( auto group_with_name : groups_by_name ) {
		string group_name = group_with_name.first;
		group_def_set_t &group_def_set = groups_by_name[group_name];
		for ( auto group_def : group_def_set ) {
			// go back to each original group defn that this coalesced group
			// is derived from.  for those gather up all the child defs and
			// map *those* to the new correlates before inserting
			// into the coalesced group.

			def_set_t &original_group_set = rev_correlates[group_def];
			for ( auto _original_group : original_group_set) {
				group_def_t *original_group = dynamic_cast<group_def_t *>(_original_group);
				assert(original_group);
				assert( correlates[_original_group] == group_def );

				// regs
				for ( auto reg_set : original_group->regs_by_name ) {
					for (auto orig_child_reg : reg_set.second ) {
						reg_def_t *new_reg = dynamic_cast<reg_def_t *>(map_to_new(orig_child_reg, correlates));
						assert(new_reg);
						group_def->regs_by_name[new_reg->symbol].insert(new_reg);
					}
				}

				// now peform this same business with fields, and constants
				for ( auto field_set : original_group->fields_by_name ) {
					for (auto orig_child_field : field_set.second ) {
						field_def_t *new_field =
							dynamic_cast<field_def_t *>(map_to_new(orig_child_field, correlates));
						assert(new_field);
						group_def->fields_by_name[new_field->symbol].insert(new_field);
					}
				}
				// constants
				for ( auto const_set : original_group->constants_by_name ) {
					for (auto orig_child_const : const_set.second ) {
						const_def_t *new_const =
							dynamic_cast<const_def_t *>(map_to_new(orig_child_const, correlates));
						assert(new_const);
						group_def->constants_by_name[new_const->symbol].insert(new_const);
					}
				}
			}
		}
	}

	//
	// for each new (and potentially coalesced) def go back and inspect the
	// connection hierarchy (const->field->reg) to represent
	// the original parent/child pointers with their correlates.
	// note that the group -> {reg, field, const} mapping has already been performed above.
	//

	for ( auto reg_name_set : regs_by_name ) {
		reg_def_set_t &reg_set = reg_name_set.second;
		string reg_name = reg_name_set.first;

		for ( auto reg : reg_set ) {
			def_set_t &orig_reg_set = rev_correlates[reg];
			assert(orig_reg_set.size() == 1);
			reg_def_t *orig_reg = dynamic_cast< reg_def_t * >(*orig_reg_set.begin());
			assert(orig_reg);

			// link to match the original reg's fields (and consts)
			for ( auto orig_field_name_set : orig_reg->fields_by_name ) {
				field_def_set_t &orig_field_set = orig_field_name_set.second;
				string field_name = orig_field_name_set.first;
				for ( auto orig_field : orig_field_set ) {
					// orig_field -> new (coalesced) field
					field_def_t *new_coalesced_f =
						dynamic_cast<field_def_t *>( map_to_new(orig_field, correlates) );
					reg->fields_by_name[field_name].insert(new_coalesced_f);
				}
			}

			for ( auto orig_const_name_set : orig_reg->constants_by_name ) {
				const_def_set_t &orig_const_set = orig_const_name_set.second;
				string const_name = orig_const_name_set.first;
				for ( auto orig_const : orig_const_set ) {
					// orig_const -> new (coalesced) const
					const_def_t *new_coalesced_c =
						dynamic_cast<const_def_t *>( map_to_new(orig_const, correlates) );
					reg->constants_by_name[const_name].insert(new_coalesced_c);
				}
			}
		}
	}


	for ( auto field_name_set : fields_by_name ) {
		field_def_set_t &field_set = field_name_set.second;
		string field_name = field_name_set.first;

		for ( auto field : field_set ) {
			def_set_t &orig_field_set = rev_correlates[field];
			assert(orig_field_set.size() == 1);
			field_def_t *orig_field = dynamic_cast< field_def_t * >(*orig_field_set.begin());
			assert(orig_field);

			for ( auto orig_const_name_set : orig_field->constants_by_name ) {
				const_def_set_t &orig_const_set = orig_const_name_set.second;
				string const_name = orig_const_name_set.first;
				for ( auto orig_const : orig_const_set ) {
					// orig_const -> new (coalesced) const
					const_def_t *new_coalesced_c =
						dynamic_cast<const_def_t *>( map_to_new(orig_const, correlates) );
					field->constants_by_name[const_name].insert(new_coalesced_c);
				}
			}
		}
	}
}


ostream& operator<< (ostream& os, const group_def_t& d) {
	os << "group def " << d.symbol.c_str() << "= { " << d.def_gpus << " }" << endl;
	os << "\t" << d.regs_by_name.size() << " reg names" << endl;
	os << "\t" << d.fields_by_name.size() << " field names" << endl;
	os << "\t" << d.constants_by_name.size() << " const names }" << endl;
	
	return os;
}

ostream& operator<< (ostream& os, const def_tree_t& tree)
{
	os << "def_tree_t { gpus=[" << tree.gpus << "]";
	os << "groups=[";
	for ( auto g: tree.groups ) { os << "\t[" << *g << "] " << endl; }
	os << endl;
	os << tree.defs_by_name.size() << "\ttree def names" << endl;
	os << tree.regs_by_name.size() << "\ttree reg names" << endl;
	//	size_t reg_names = 0;
	
	for ( auto tr : tree.regs_by_name ) {
		//os << "info: tree reg " << tr.first.c_str() << endl;
	}
	//	os << reg_names << "\t real tree reg names" << endl;
	os << tree.fields_by_name.size() << "\ttree field names" << endl;
	os << tree.constants_by_name.size() << "\ttree constant names" << endl;
	os << endl;
	return os;
}

ostream &operator<<(ostream &os, const gpu_set_t &gpus)
{
	for (auto g : gpus ) {
		os << g->name() << " ";
	}
	return os;
}

ostream &operator<<(ostream &os, const gpu_list_t &gpus)
{
	for (auto g : gpus ) {
		os << g->name() << " ";
	}
	return os;
}
