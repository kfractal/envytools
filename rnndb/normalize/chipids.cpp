// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
/*
 *  >insert nvidia open source copyright<
 */

#include <array>
#include <fstream>
#include <list>
#include <unordered_map>
#include <cstdint>
#include <stdexcept>
#include <locale>
#include <ctime>

#include <QDir>
#include <QFileInfo>
#include <QFile>

#include "gen_registers.h"

static int now_year;

#define yyFlexLexer exprFlexLexer
#include <FlexLexer.h>

#include "expr.h"
FooLexer* lexer = 0;

#include "chipids.h"

using namespace std;

#ifndef USE_QT_REGEX
#include <regex>
using namespace std::regex_constants;
#else
#    include <QRegularExpression>
#endif

defn_set_t __defines;
defn_index_t __defn_index;

bool nouveau_only_mode    = true;
bool per_hwref_file_mode  = true;  // probably a ton of equiv sets if false
bool target_gpus_mode     = true;  // as opposed to table driven
#define USE_EQUIV_CLASSES 1
bool per_gpu_mode         = !USE_EQUIV_CLASSES;  // vs. all at once
bool use_symbol_blacklist = false; // always gates (post whitelist) if enabled.
bool use_hwref_file_whitelist  = false;
bool use_fuzzy_match_whitelist = false; // otherwise strict
bool use_goofy_rejections      = true;  // don't emit silly stuff (e.g.: #define FOO_TRUE 1)
bool use_symbol_whitelist      = true;  // seed to start

std::vector<string> symbol_whitelist_gpus { "gk20a", "gm20b" };

int hex_switchover_value = 32; // highest decimal is this
// should be >= 32 otherwise bitfields show up in hex (e.g.: "0x10:0x11" )

gpu_set_t nouveau_gpus;
list<gpuid_t*> target_gpus;
gpu_set_t no_gpus, all_gpus;


gpu_family_t
	g8x_family     {"g8x"},
	tesla_family   {"tesla"},
	kepler_family  {"kepler"},
	fermi_family   {"fermi"},
	maxwell_family {"maxwell"},
	pascal_family  {"pascal"};

gpu_family_t *family_order[] {
	&g8x_family,
	&tesla_family,
	&fermi_family,
	&kepler_family,
	&maxwell_family,
	&pascal_family
};

extern map<string, bool> symbol_whitelist;
map<string, bool> symbol_blacklist;
map<string, bool> hwref_file_blacklist {
	{ "vpmSigNames.h" , true},
	{ "user_access_map.h", true},
	{ "user_access_map_gzip.h", true},
	{ "pm_signals.h", true },
	{ "kind_macros.h", true},
	//{ "dev_graphics_nobundle.h", true } // super dense, requires special handling
};

// unused for now (typically used for investigations)
map<string, bool> hwref_file_whitelist {
	{ "dev_mmu.h",  true},
	{ "dev_flush.h", true},
	{ "dev_ltc.h", true},
		//{ "dev_ram.h",  true},
};

//
// Grouped by nouveau-recognized (or not) then on arch, id.
//

//
// master list of nouveau supported gpus
//
static vector<gpuid_t::ctor_t> gpuid_table {

	{ "g80"     , 0x0080 , "${hwref}/nv50",  true , &g8x_family },
	{ "g84"     , 0x0084 , "${hwref}/g84",   true , &g8x_family },
	{ "g86"     , 0x0086 , "${hwref}/g86",   true , &g8x_family },
	{ "g92"     , 0x0092 , "${hwref}/g92",   true , &g8x_family },
	{ "g94"     , 0x0094 , "${hwref}/g94",   true , &g8x_family },
	{ "g96"     , 0x0096 , "${hwref}/g96",   true , &g8x_family },
	{ "g98"     , 0x0098 , "${hwref}/g98",   true , &g8x_family },

	{ "mcp77"   , 0x00aa , "${hwref}/igt206", true , &tesla_family },
	{ "mcp79"   , 0x00ac , "${hwref}/igt209", true , &tesla_family },
	{ "mcp89"   , 0x00af , "${hwref}/igt21a", true , &tesla_family },
	{ "g200"    , 0x00a0 , "${hwref}/gt200", true , &tesla_family },
	{ "gt215"   , 0x00a3 , "${hwref}/gt215", true , &tesla_family },
	{ "gt216"   , 0x00a5 , "${hwref}/gt216", true , &tesla_family },
	{ "gt218"   , 0x00a8 , "${hwref}/gt218", true , &tesla_family },


	{ "gf100"   , 0x00c0 , "${hwref}/fermi/gf100",  true , &fermi_family },
	{ "gf104"   , 0x00c4 , "${hwref}/fermi/gf104",  true , &fermi_family },
	{ "gf106"   , 0x00c3 , "${hwref}/fermi/gf106",  true , &fermi_family },
	{ "gf108"   , 0x00c1 , "${hwref}/fermi/gf108",  true , &fermi_family },
	{ "gf110"   , 0x00c8 , "${hwref}/fermi/gf100b", true , &fermi_family },
	{ "gf116"   , 0x00cf , "${hwref}/fermi/gf106b", true , &fermi_family },
	{ "gf114"   , 0x00ce , "${hwref}/fermi/gf104b", true , &fermi_family },
	{ "gf117"   , 0x00d7 , "${hwref}/fermi/gf117",  true , &fermi_family },
	{ "gf119"   , 0x00d9 , "${hwref}/fermi/gf119",  true , &fermi_family },

	{ "gk104"   , 0x00e4 , "${hwref}/kepler/gk104",  true , &kepler_family },
	{ "gk106"   , 0x00e6 , "${hwref}/kepler/gk106",  true , &kepler_family },
	{ "gk107"   , 0x00e7 , "${hwref}/kepler/gk107",  true , &kepler_family },
	{ "gk110"   , 0x00f0 , "${hwref}/kepler/gk110",  true , &kepler_family },
	{ "gk110b"  , 0x00f1 , "${hwref}/kepler/gk110b", true , &kepler_family },
	{ "gk208b"  , 0x0106 , "${hwref}/kepler/gk208s", true , &kepler_family },
	{ "gk208"   , 0x0108 , "${hwref}/kepler/gk208",  true , &kepler_family },
	{ "gk20a"   , 0x00ea , "${hwref}/kepler/gk20a",  true , &kepler_family },

	{ "gm107"   , 0x0117 , "${hwref}/maxwell/gm107", true , &maxwell_family },
	{ "gm108"   , 0x0118 , "${hwref}/maxwell/gm108", true , &maxwell_family }, /* check released first!*/
	{ "gm204"   , 0x0124 , "${hwref}/maxwell/gm204", true , &maxwell_family },
	{ "gm206"   , 0x0126 , "${hwref}/maxwell/gm206", true , &maxwell_family },
	{ "gm20b"   , 0x012b , "${hwref}/maxwell/gm20b", true , &maxwell_family },

};

struct repl_symbol_t {
#ifndef USE_QT_REGEX
	regex  match_expr; // set programmatically (i.e. don't bother in initializer)
#else
	QRegularExpression match_expr;
#endif
	string value;      // set in static initializer or at alloc/init
};

//
// nvidia-internal hardware header locations.
//
unordered_map<string, repl_symbol_t> symbol_map {
	{ "sw_root" , { .value = { "/Volumes/too/t/sw" } } },
	{ "chips_a" , { .value = { "${sw_root}/dev/gpu_drv/chips_a" } } },
	{ "hwref"   , { .value = { "${chips_a}/drivers/common/inc/hwref" } } },
};

// note:
// the term "symbols" here refers to the names used in finding hwref file
// paths and such.  not the macros/symbols in the headers themselves...
//
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
string replace_symbol_refs(const string &_src)
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


bool hwref_file_blacklisted(const string &fn) {
	return hwref_file_blacklist.find(fn) != hwref_file_blacklist.end();
}
bool hwref_file_whitelisted(const string &fn) {
	return hwref_file_whitelist.find(fn) != hwref_file_whitelist.end();
}
bool use_hwref_file(const string &fn){
	return !hwref_file_blacklisted(fn) && (!use_hwref_file_whitelist || hwref_file_whitelisted(fn));
}

set<string> all_hwref_files;
set<string> seed_hwref_files;

// information from gen_registers about the type of a symbol.
// values aren't recorded there, only the type.
std::multimap<std::string, group_t *>    chip_groups;
std::multimap<std::string, reg_t *>      chip_regs;
std::multimap<std::string, field_t *>    chip_fields;
std::multimap<std::string, constant_t *> chip_constants;

std::multimap<uint32_t, defn_t *> reg_val_index;

//
// use gen_registers_{gpu} to populate the symbol whitelist
// gen_registers also specifies the type and value of those symbols.
//
void init_symbol_whitelist(const string &wl_gpu)
{
	if ( wl_gpu == "gk20a" ){
		emit_groups_gk20a();
		std::map<std::string, group_t *>    * gk20a_groups    = get_groups();
		std::map<std::string, reg_t *>      * gk20a_regs      = get_regs();
		std::map<std::string, field_t *>    * gk20a_fields    = get_fields();
		std::map<std::string, constant_t *> * gk20a_constants = get_constants();

		for (auto g : *gk20a_groups)    chip_groups.   insert(g);
		for (auto r : *gk20a_regs)      chip_regs.     insert(r);
		for (auto f : *gk20a_fields)    chip_fields.   insert(f);
		for (auto c : *gk20a_constants) chip_constants.insert(c);
		// leaking
	} else if (wl_gpu == "gm20b" ) {
		emit_groups_gm20b();
		std::map<std::string, group_t *>    * gm20b_groups    = get_groups();
		std::map<std::string, reg_t *>      * gm20b_regs      = get_regs();
		std::map<std::string, field_t *>    * gm20b_fields    = get_fields();
		std::map<std::string, constant_t *> * gm20b_constants = get_constants();

		for (auto g : *gm20b_groups)    chip_groups.   insert(g);
		for (auto r : *gm20b_regs)      chip_regs.     insert(r);
		for (auto f : *gm20b_fields)    chip_fields.   insert(f);
		for (auto c : *gm20b_constants) chip_constants.insert(c);
		// leaking
	}
}

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
		if ( use_symbol_whitelist ) {
			auto found = find(symbol_whitelist_gpus.begin(), symbol_whitelist_gpus.end(), _d.name);
			if ( found != symbol_whitelist_gpus.end() )
				seed_hwref_files.insert(fn);
		}
		all_hwref_files.insert(fn);
		if ( use_hwref_file(fn) ) {
			// cout << "info: " << name() << " < " << fn << endl;
			_hwref_files.insert(fn);
		}
	}
}


list<string> *common_hwref_files(list<gpuid_t *> &gpus)
{
	//cout << "num gpus:" << gpus.size();
	map<string, size_t> hwref_file_count;
	list<string> *common_files = new list<string>;
	for ( auto &gpu : gpus) {
		//cout << "info: " << gpu->name() << endl;
		for ( auto &f : gpu->hwref_files() ) {
			//cout << "info: \t\t" << f << endl;
			if ( hwref_file_count.find(f) == hwref_file_count.end() ) {
				hwref_file_count[f] = 1;
			} else {
				hwref_file_count[f]++;
			}
		}
	}
	// those with a full count are common to all
	for ( map<string, size_t>::iterator m = hwref_file_count.begin(); m != hwref_file_count.end(); m++) {
		if ( m->second == gpus.size() ) {
			common_files->push_back(m->first);
			// cout << "info: count for " << m->first << " " << m->second << endl;
		} else {
			//cout << "warning: count for " << m->first << " " << m->second << endl;
		}

	}
	return common_files;
}


void init_gpuids() // ostream &out = std::cout )
{
	for (size_t v =0; v < gpuid_table.size(); v++ )
	{
		gpuid_t::ctor_t &gpu_ctor = gpuid_table[v];
		gpuid_t *new_gpu = new gpuid_t(gpu_ctor);

		if ( !nouveau_only_mode || (nouveau_only_mode && new_gpu->nouveau()) ) {
			//cout << "insert gpu: " << new_gpu->name() << endl;
			new_gpu->get_hwref_files();
			target_gpus.push_back(new_gpu);
			if( ! new_gpu->family() ) {
				cerr << "error: gpus must be assigned to a family [" << new_gpu->name() << "]" << endl;
				exit(1);
			}
			new_gpu->family()->insert(new_gpu);
			all_gpus.insert(new_gpu);
		}
	}

	for ( auto &t: target_gpus ) {
#if 0
		//cout << "target:" << t->id();
		stringstream tmpss;
		tmpss << "#define __nv_" << t->name() << "__ ";
		int len = tmpss.str().size();
		out << tmpss.str();
		tmpss.str("");
		tmpss << "0x" << hex << t->id();
		int space_cols = 40 - (len+tmpss.str().size()+1);
		string gap;  gap.insert(0, space_cols, ' ');
		out << gap << tmpss.str() << endl;
#endif
	}

#if 0
	for ( auto &family: family_order ) {
		// this, so a new (invisible/unused) arch family won't be emitted.
		if ( !family->gpus().size() ) {
			continue;
		}
		string def = family->def();
		out << def << endl;
	}
#endif
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

static string normalize_token(std::list<FooLexer::token_t>::iterator &t)
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


bool fuzzy_match_whitelist(const string &def);
bool is_goofy(const string &def, const string &val);


void process_gpu_defs(gpuid_t *g, string def_file_name)
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
		// cerr << "info: " << full_path << " doesn't exist ..." << endl;
		// typically ok.  some headers are new/changed per-chip.
		// but we scan over all possibilities, so...
		return;
	}

	if (0) cerr << "info: process [\"" << full_path << "\"]" << endl;

	std::ifstream ifs;
	ifs.open (full_path, std::ifstream::in);
	if ( ! (ifs.good() || ifs.eof() ) ) {
		cerr << "error: couldn't open " << full_path << " for reading." << endl;
		exit(1);
		return; /*NOTREACHED*/
	}

	int line_nr = -1;

	stringstream outstring;
	if ( lexer )
		delete lexer;
	lexer = new FooLexer(&ifs, &outstring);

	while (lexer->yylex() != 0) { }

	if ( outstring.str().size() )
		cerr << "#info lex[" << outstring.str() << "]" << endl;
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
		line_nr++;
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
			if ( ident_match != "i" && ident_match != "G" ) {
				cerr << "warning: not expecting anything other than \"i\" or"
					" \"G\" as macro argument." << endl;
				cerr << "warning : saw \"" << ident_match <<
					"\" in def'n for [" << def_match << "]" << endl;

			}
#endif
			idstring = "(" + ident_match + ")";
		}

		if ( use_symbol_whitelist && ( symbol_whitelist.find(def_match) ==
									   symbol_whitelist.end() ) ) {
			// fuzzy match allows register fields through which are a part
			// of any otherwise whitelisted/ok register...
			if ( ! (use_fuzzy_match_whitelist && fuzzy_match_whitelist(def_match)) ) {
				continue;
			}
		}

		if ( use_symbol_blacklist && ( symbol_blacklist.find(def_match) !=
									   symbol_blacklist.end() ) ) {
			continue;
		}
		if ( use_goofy_rejections && is_goofy(def_match, val_match) ) {
			continue;
		}

		// is it a register or a field or a constant? (theoretically could be multiple types?)
		bool is_reg = false, is_field = false, is_constant = false;
		if ( chip_regs.find(def_match) != chip_regs.end() ) {
			is_reg = true;
		} else if (chip_fields.find(def_match) != chip_fields.end()) {
			is_field = true;
		} else if (chip_constants.find(def_match) != chip_constants.end()) {
			is_constant = true;
		}

		//XXX is this correct?
		// now, add the (i) to the symbol if it should be.
		// () should match (i) and (i,j) as well since they would collide.
		def_match += idstring;

		defn_t *symbol_defn       = 0;
		defn_index_t *defns_index = g->defns_index();
		defn_set_t *defns         = g->defines();

		defn_index_t::iterator find_defn = defns_index->find(def_match);
		if ( find_defn == defns_index->end() ) { // first encounter with the symbol (this gpu)
			symbol_defn = (*defns_index)[def_match] = new defn_t(def_match);
			symbol_defn->line_nr = line_nr;
			symbol_defn->is_reg      = is_reg;
			symbol_defn->is_field    = is_field;
			symbol_defn->is_constant = is_constant;
			defns->insert(symbol_defn);
		} else {
			symbol_defn = find_defn->second;
			if ( line_nr < symbol_defn->line_nr ) {
				symbol_defn->line_nr = line_nr;
			}
		}

		defn_val_index_t::iterator find_val = symbol_defn->val_index.find(val_match);
		defn_val_t *defn_val = 0;
		if ( find_val == symbol_defn->val_index.end() ) { // first encounter
			defn_val = symbol_defn->val_index[val_match] = new defn_val_t(val_match);
			defn_val->line_nr = line_nr;
			symbol_defn->vals.insert(defn_val);

			if ( symbol_defn->is_reg ) {
				try {
					uint32_t vnum = std::stoull (val_match, nullptr,0);
					reg_val_index.insert(std::make_pair( vnum, symbol_defn ));
					// std::cout << "info: adding reg match val " << std::hex << vnum << " for " << symbol_defn->symbol << endl;

				} catch ( std::invalid_argument w) {
					// std::cout << "error: failed to convert val " << val_match << " for " << symbol_defn->symbol << endl;
				}
				// <std::string, defn_t*>(val_match, symbol_defn));
			}

		} else {
			defn_val = find_val->second;
			if ( line_nr < defn_val->line_nr) {
				defn_val->line_nr = line_nr;
			}
		}
		defn_val->gpus.insert(g);
	}
}


bool sort_defn( const defn_t *i, const defn_t *j) {
	return i->line_nr < j->line_nr;
}

bool sort_val( const defn_val_t *i, const defn_val_t *j) {
	return i->line_nr < j->line_nr;
}

void emit_hwref_macros(defn_set_t *defs, ostream &out = std::cout )
{

	// sort the definitions by line_nr
	list<defn_t *> sorted_defs;
	for ( auto &d: (*defs) ) {
		sorted_defs.push_back(d);
	}
	sorted_defs.sort(sort_defn);

	gpu_equiv_class_t *current_equiv_class = 0;
	for ( auto &d: sorted_defs ) {
		// out << "defn: " << d->symbol << " " << endl;
		list<defn_val_t *> sorted_vals;
		for ( auto &v: d->vals) {
			sorted_vals.push_back(v);
		}
		sorted_vals.sort(sort_val);

		for ( auto &v: sorted_vals) {
			stringstream defline;
			if ( v->eq_class != current_equiv_class ) {
				if ( current_equiv_class ) {
					out << "#endif" << endl;
				}
				out << "#if " << v->eq_class->ref() << "(__nv_chipset_target__)" << endl;
				current_equiv_class = v->eq_class;
			}

			if ( per_gpu_mode ) {
				stringstream defline;
				defline << "#define " << d->symbol;

				int space_cols = 67 - (defline.str().size() + v->val.size() + 1);
				if ( space_cols <= 0 ) {
					space_cols = 79 - (defline.str().size() + v->val.size() + 1);
				}
				if ( space_cols <= 0 ) space_cols = 1;
				string gap;  gap.insert(0, space_cols, ' ');

				out << defline.str() << gap << v->val << endl;
			} else {
				out <<"#\tdefine " << d->symbol << " " << v->val << endl;
			}

		}
	}
	if ( current_equiv_class )
	{
		out << "#endif\n";
	}
}

int chipids_main()
{
	time_t time_now = time(0);
	tm* gm_time_now = gmtime(&time_now);
	now_year = gm_time_now->tm_year + 1900;

	init_symbols(); // ancillary symbol scheme...
	init_gpuids();

	for ( vector<string>::iterator wi = symbol_whitelist_gpus.begin();
		  wi != symbol_whitelist_gpus.end(); wi++) {
		init_symbol_whitelist(*wi);
	}

	list<string> *potential_files = new list<string>();
	for ( auto &fn : seed_hwref_files ) {
		if ( use_hwref_file(fn) ) {
			potential_files->push_back(fn);
		}
		// cerr << "info: potential file " << fn << endl;
	}

	for ( auto &fn : *potential_files ) {
		if ( hwref_file_blacklist.find(fn) != hwref_file_blacklist.end())
			continue;
		for ( auto g = target_gpus.begin(); g != target_gpus.end(); g++ ) {
			process_gpu_defs(*g, fn);
		}
	}

	return 0;
}


void FooLexer::push_hex_literal(const char *s) {
	_tokens.push_back(token_t(token_type_e::hex_literal, std::string(s)));
}
void FooLexer::push_dec_literal(const char *s) {
	_tokens.push_back(token_t(token_type_e::dec_literal, std::string(s)));
}
void FooLexer::push_identifier(const char *s) {
	_tokens.push_back(token_t(token_type_e::identifier, std::string(s)));
}
void FooLexer::push_grouping(const char *s) {
	_tokens.push_back(token_t(token_type_e::grouping, std::string(s)));
}
void FooLexer::push_lop(const char *s) {
	_tokens.push_back(token_t(token_type_e::lop, std::string(s)));
}
void FooLexer::push_uop(const char *s) {
	_tokens.push_back(token_t(token_type_e::uop, std::string(s)));
}
void FooLexer::push_mop(const char *s) {
	_tokens.push_back(token_t(token_type_e::mop, std::string(s)));
}
void FooLexer::push_unk(const char *s) {
	_tokens.push_back(token_t(token_type_e::unk, std::string(s)));
}
void FooLexer::push_hashop() {
	_tokens.push_back(token_t(token_type_e::hashop, std::string("#")));
}
void FooLexer::push_keyword(const char *s) {
	_tokens.push_back(token_t(token_type_e::keyword, std::string(s)));
}
void FooLexer::push_whitespace() {
	_tokens.push_back(token_t(token_type_e::whitespace, std::string()));
}
void FooLexer::push_comment(const char *) {
	_tokens.push_back(token_t(token_type_e::comment, std::string()));
}

void FooLexer::push_dquot_string(const char *) {
	_tokens.push_back(token_t(token_type_e::dquot_string, std::string()));
}
void FooLexer::push_squot_string(const char *){
	_tokens.push_back(token_t(token_type_e::squot_string, std::string()));
}
void FooLexer::push_newline() {
	_tokens.push_back(token_t(token_type_e::newline, std::string()));
}

bool fuzzy_match_whitelist(const string &def)
{
	bool did = false;
	string match_instead;

	// look to see if this is an element of a register is whitelisted.
	// if so then drag anything in which is a subset (fields).
	size_t pos = 0;
	int    u  = 0;
	array<size_t, 4> uscore { {string::npos, string::npos, string::npos}};
	for ( pos = 0; (u < 4) && (pos < def.length()); pos++) {
		if ( def[pos] == '_' ) uscore[u++] = pos;
	}
	if ( u >= 3 ) {  // ala "NV_PFIFO_STATUS_"...
		match_instead = def.substr(0, uscore[2]);
		if ( symbol_whitelist.find(match_instead) != symbol_whitelist.end() ) {
			did = true;
		}
	}
	if (did) {
		//cerr << "#info: fuzzy matched [" << def << "] with [" <<
		//    match_instead << "] instead" << endl;
	}

	return did;
}


struct goofy_t {
#ifndef USE_QT_REGEX
	regex  def;
	string val;
	goofy_t(const regex &d, const string &v) : def(d), val(v){ }
	goofy_t(const string &d, const string &v) : def(regex(d)), val(v){ }
#else
	QRegularExpression def;
	QString val;
	goofy_t(const QRegularExpression &d, const QString &v) : def(d), val(v){ }
	goofy_t(const QString &d, const QString &v) : def(QRegularExpression(d)), val(v){ }
#endif
};

static vector<goofy_t> goofs {
	{ "^.+_TRUE$",     "1"},
	{ "^.+_FALSE$",    "0"},
	{ "^.+_ENABLED$",  "1"},
	{ "^.+_DISABLED$", "0"},
	{ "^.+_YES$",      "1"},
	{ "^.+_NO$",       "0"},
	{ "^.+_NOT_PENDING$", "0"}, // note order here is important vs. the one to follow!
	{ "^.+_PENDING$",     "1"},
		};

bool is_goofy(const string &def, const string &val)
{
	bool dbg_it = false;
	string dbg_it_needle = "NV_PGRAPH_INTR_NOTIFY_PENDING";
	if ( 0 ) { // def == dbg_it_needle ) {
		dbg_it = true;
	}
	for ( auto &g: goofs ) {

#ifndef USE_QT_REGEX
		if ( regex_match(def, g.def) && (g.val == val))
			return true;
#else
		QRegularExpressionMatch m; //  = QString::fromStdString(def).match(g.def);
		QString val_qstr = QString::fromStdString(val);
		QString def_qstr = QString::fromStdString(def);
		m = g.def.match(def_qstr);
		if ( dbg_it ) {
			cerr << "info: needle[" << dbg_it_needle << "] vs. [" << g.def.pattern().toStdString() << "] valid [" << m.isValid() << "] match [" << m.hasMatch() << "]" << endl;
			cerr << "info: needle[" << dbg_it_needle << "] def [" << def_qstr.toStdString() << "] val [" << val_qstr.toStdString() << "] gval [" << g.val.toStdString() << "]" << endl;
		}
		if (m.isValid() && m.hasMatch() && (g.val == val_qstr)) {
			if (dbg_it) {
				cerr << "info: needle returning true" << endl;
			}
			return true;
		}
#endif
	}
	return false;
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
void calculate_equiv_classes(defn_set_t *defs,  string def_file_name ) {
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

gpu_equiv_class_t *gpu_equiv_class_t::snap(const string &tag)
{
	for ( size_t i = 0; i < _snapshot.size(); i++) {
		if ( *this == *(_snapshot[i]) ) {
			return _snapshot[i];
		}
	}
	int order = _snapshot.size();
	stringstream refstr; refstr << "__nv_" << tag << "_compat_" << order;
	stringstream defstr; defstr << "#define __nv_" << tag << "_compat_" << order << "(x)  ((x)!=(x)\\";
	for ( auto &family : _families ) {	defstr << "\n\t\t|| " << family->ref() << "(x)\\";	}
	for ( auto &gpu : _gpus ) {	defstr << "\n\t\t|| ((x)==" << gpu->ref() << ")\\";	}
	defstr << "\n\t)" << endl;
	_def = defstr.str();
	_ref = refstr.str();
	gpu_equiv_class_t *snapped = new gpu_equiv_class_t(*this);
	_snapshot.push_back(snapped);
	return snapped;
}

void emit_equiv_classes( ostream &out)
{
	for ( int i = 0; i < gpu_equiv_class_t::snapshot_size(); i++) {
		gpu_equiv_class_t *eq_class_i = gpu_equiv_class_t::snapshot_at(i);
		out << eq_class_i->def();
	}
}

// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#include <string>
#include <iostream>
#include <sstream>
#ifndef USE_QT_REGEX
#include <regex>
#else
#include <QRegularExpression>
#endif

static bool dbg_copyright = false;

using namespace std;
#ifndef USE_QT_REGEX
static regex copyright_regex(" \\* Copyright \\(c\\) ([0-9]+)(-([0-9]+))?, .*NVIDIA.*");
#else
static QRegularExpression copyright_regex(" \\* Copyright \\(c\\) ([0-9]+)(-([0-9]+))?, .*NVIDIA.*");
#endif

static const char *NV_nouveau_boilerplate[]= {
    "/*\n",
    " * Copyright (c) ",
    ", NVIDIA CORPORATION. All rights reserved.\n",
    " *\n",
    " * Permission is hereby granted, free of charge, to any person obtaining a\n",
    " * copy of this software and associated documentation files (the \"Software\"),\n",
    " * to deal in the Software without restriction, including without limitation\n",
    " * the rights to use, copy, modify, merge, publish, distribute, sublicense,\n",
    " * and/or sell copies of the Software, and to permit persons to whom the\n",
    " * Software is furnished to do so, subject to the following conditions:\n",
    " *\n",
    " * The above copyright notice and this permission notice shall be included in\n",
    " * all copies or substantial portions of the Software.\n",
    " *\n",
    " * THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n",
    " * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n",
    " * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL\n",
    " * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n",
    " * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING\n",
    " * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER\n",
    " * DEALINGS IN THE SOFTWARE.\n",
    " */\n",
    NULL
};
static size_t NV_nouveau_date_line = 1; /* index to the line with the copyright date break */

bool existing_copyright_in_stream(istream &in, int &year_begin, int &year_end)
{
    year_begin = year_end = -1;
    in.seekg (0, in.beg);
    if (!in.good()) return false;

    if ( dbg_copyright )
	cerr << "#info: checking for existing copyright" << endl;

    bool present = false;
    string line;
    while ( in.good() && (!present) ) {
		getline(in, line); if ( !line.size() ) continue;

#ifndef USE_QT_REGEX
		smatch match_results;
		regex_match (line, match_results, copyright_regex);
		if ( match_results.size() ) {
			string ins(match_results[1]);
			istringstream inss(ins);
			inss >> year_begin;
			if ( dbg_copyright )
				cerr << "#info: matched copyright check year: " << ins << " " << year_begin << endl;
			return true;
		}
#else
		QRegularExpressionMatch m;
		m = copyright_regex.match(QString::fromStdString(line));
		if ( m.isValid() && m.hasMatch() ) {
			string ins = m.captured(1).toStdString();
			//string ins(match_results[1]);
			istringstream inss(ins);
			inss >> year_begin;
			if ( dbg_copyright )
				cerr << "#info: matched copyright check year: " << ins << " " << year_begin << endl;
			return true;
		}
#endif

    }
    return false;
}

void emit_copyright(ostream &out, int year_begin, int year_end)
{
    size_t l = 0;
    const char *p = NV_nouveau_boilerplate[l];
    while ( p ) {
	out << p;
	if ( l == NV_nouveau_date_line ) {
	    out << year_begin;
	    if ( (year_end != year_begin) && (year_end != -1) ) {
		out << "-" << year_end;
	    }
	    p = NV_nouveau_boilerplate[++l];
	    out << p;
	}
	p = NV_nouveau_boilerplate[++l];
    };
}
