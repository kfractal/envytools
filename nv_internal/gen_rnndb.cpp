/*
 * Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
 * insert GPL v2
 */
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <time.h>
#include <regex.h>
#include <libgen.h>

#include "gen_rnndb.h"
#include "gen_rnndb_cmdline.h"

#include <QDebug>
#include <QMap>
#include <QList>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDomDocument>
#include <QString>
#include <QXmlStreamWriter>

extern "C" {
    /* do not change the name of this symbol */
    struct gengetopt_args_info args_info;
}

using namespace std;

//
// misc junk ...
//
inline ostream& operator << ( ostream &os, QString qstr) {
    return os << qstr.toStdString();
}

inline bool verbose()  {
    return args_info.verbose_arg;
}

// emit_xml controls paths useable only for debug purposes at this point.
inline bool emit_xml() {
    return false;
}

template <class X>
inline string hexStr( X x ) {
    stringstream ss;
    ss << "0x" << hex << x;
    return ss.str();
}

//
// rnndb (re)construction
//
struct rnndb_t {
    struct array_t {
	int length;
	string name;
	int offset;
	int stride;
	list<string> variants;
    };
    struct author_t {
	string email;
	string name;
    };
    struct bitfield_t {
	uint32_t add;
	uint32_t align;
	uint32_t high;
	uint32_t low;
	uint32_t max;
	uint32_t min;
	string name;
	uint32_t pos;
	uint32_t shr;
	string type;
	list<string> variants;
    };
    struct bitset_t {
	bool inline_;
	string name;
	string prefix;
	list<string> variants;
	string varset;
    };
    struct copyright_t {
	string year;
    };
    struct domain_t { 
	bool bare;
	string name;
	string prefix;
	uint32_t size;
	list<string> variants;
	string varset;
	uint32_t width;
    };
    struct enum_t { 
	bool bare;
	bool inline_;
	list<string> variants;
	string varset;
    };
    struct group_t {
	string name;
    };
    struct import_t {
	string file;
    };
    struct nick_t {
	string name;
    };
    struct reg16_t {
	string name;
	uint32_t offset;
    };
    struct reg32_t {
	uint32_t access;
	uint32_t align;
	uint32_t length;
	uint32_t max;
	uint32_t min;
	string name;
	uint32_t offset;
	uint32_t shr;
	uint32_t stride;
	uint32_t type;
	list<string> variants;
	string varset;
    };
    struct reg64_t {
	uint32_t length;
	string name;
	uint32_t offset;
	uint32_t shr;
	list<string> variants;
	string varset;
    };
    struct reg8_t {
	uint32_t access;
	uint32_t length;
	string name;
	uint32_t offset;
	uint32_t shr;
	uint32_t type;
	list<string> variants;
    };
    struct spectype_t {
	string name;
	uint32_t type;
    };
    struct stripe_t {
	uint32_t length;
	string name;
	uint32_t offset;
	string prefix;
	uint32_t stride;
	list<string> variants;
	string varset;
    };
    struct use_group_t {
	string name;
    };
    struct value_t {
	string name;
	uint32_t value;
	list<string> variants;
	string varset;
    };
};


//
// ---- gen_registers (of old) ----
//
// the system below starts running in the ip specific files:
//     gk20a/gen_rnndb_gk20a.cpp and
//     gm20b/gen_rnndb_gm20b.cpp.
// those define the registers, fields etc to be added/checked/emitted.
//
// that code can be used in multiple passes/ways.
// the framework around it (these functions below, aimed at
// by macros, fnd pointers, foo) is really what determines the output
// to be produced. in this case what's going on is that the gk20a/gm20b
// ip definitions are being routed to dbs and held for use later.
//
// along the way, or afterward some of that information can be dumped
// for debug or other nefarious purpose (creating #define ... style headers
// would be one...
//
struct emitted_db {
    map<string, group    *>  * group_map;
    map<string, reg      *>  * register_map;
    map<string, field    *>  * field_map;
    map<string, constant *>  * constant_map;
    emitted_db() { 
	group_map    = new map<string, group    *>();
	register_map = new map<string, reg      *>();
	field_map    = new map<string, field    *>();
	constant_map = new map<string, constant *>();
    }
};

struct emitted_db *current_db = 0;

group    * current_group    = 0;
reg      * current_register = 0;
constant * current_constant = 0;
field    * current_field    = 0;

string current_scope() {
    string scope = string("group.") + string(current_group->name);
    scope += ( ( current_register ? (string(".") + current_register->name) : ".!r" ) +
	       ( current_field    ? (string(".") + current_field->name)    : ".!f" ) +
	       ( current_constant ? (string(".") + current_constant->name) : ".!c" )
	     );
    return scope;
}

QString current_status(const char *m) { 
    return QString("%1 %2").arg(m).arg(QString::fromStdString(current_scope()));
}

string emit_scope() {
    // group is always in scope.  shortcuts may be taken elsewhere though
    string scope = string(current_group->name);
    reg *r = current_register;

    if (r && (r->name != "") )
	scope = r->def;

    return scope;
}


static inline QXmlStreamWriter *current_writer() {
    if ( emit_xml() )
	return current_group ? &current_group->stream : 0;
    else
	return 0;
}

static string gen_rnndb_filename(const group_spec &g) {
    return string(g.name) + string(".xml");
}

void begin_group(const group_spec &gs)
{
    if (current_group)
	end_group();

    (*current_db->group_map)[gs.name] = current_group = new group(gs);
    string group_filename = gen_rnndb_filename(gs);

    current_group->file.setFileName( QString::fromStdString(group_filename));
    if ( emit_xml() ) {
	if (! current_group->file.open( QIODevice::WriteOnly) )
	    throw domain_error("can't open" + group_filename + "for writing");

	current_group->stream.setDevice(&current_group->file);
	current_group->stream.setAutoFormatting(true);
	current_group->stream.writeStartDocument();
	current_group->stream.writeStartElement("database");
	current_group->stream.writeStartElement("domain");
	current_group->stream.writeAttribute("name", gs.def);
    }
}

void end_group() {
    end_register();
    if ( current_group && emit_xml() ) {
	current_group->stream.writeEndElement();  // domain
	current_group->stream.writeEndElement();  // database
	current_group->stream.writeEndDocument(); // doc
	current_group->file.close();
    }
    current_group = 0;
}

void end_register(void) {
    end_field();

    if ( ! current_register )
	return;

    if ( emit_xml() )
	current_writer()->writeEndElement(); // reg
    current_register = 0;
}

void end_scope(void) {
    end_register();
}

void end_field(void) {
    if ( ! current_field )
	return;

    if ( emit_xml() ) 
	current_writer()->writeEndElement(); // bitfield
    current_field = 0;
}

void emit_register(const register_spec &rs) {
    end_register();
    if (!current_group)
	throw logic_error("missing group");

    string group_name = current_group->def;
    string reg_name = rs.def;

    string offset_hex_str = hexStr(rs.offset);
    string offset_str = QString::number(rs.offset).toStdString();

    (*current_db->register_map)[ rs.def ] = current_register = new reg(rs);

    QString name(rs.def);
    name.replace(QString::fromStdString(group_name) + "_", QString(""),
		 Qt::CaseInsensitive);
    if ( emit_xml() ) {
	current_writer()->writeStartElement("reg32");
	current_writer()->writeAttribute("offset", QString::fromStdString(offset_hex_str));
	current_writer()->writeAttribute("name", name);
	current_writer()->writeAttribute("variants", "GK20A");
    }
}

void emit_offset(const register_spec &rs) {
    emit_register(rs);
}

void delete_register(const register_spec &rs) {
    // the spec holds the info about deleting...
    emit_register(rs);
}

/* begin scope is just setting up a register which won't be emitted */
void begin_scope(const register_spec &rs) {
    emit_register(rs);
}

void emit_field(const field_spec &fs) {
    end_field();

    string reg_name   = emit_scope();
    string field_name = fs.def;

    (*current_db->field_map)[ field_name ] = current_field = new field(fs);

    if ( (*current_db->register_map).find(reg_name) == (*current_db->register_map).end() ) {
        cerr << "can't find reg " << reg_name << " for " << field_name << endl;
	exit(1);
    } else {
        (*current_db->register_map)[reg_name]->fields[field_name] = current_field;
    }

    QString name(fs.def);
    name.replace(QString::fromStdString(reg_name) + "_", QString(""), Qt::CaseInsensitive);
    
    if ( emit_xml() ) {
	current_writer()->writeStartElement("bitfield");
	current_writer()->writeAttribute("low", QString::number(fs.lowbit));
	current_writer()->writeAttribute("high", QString::number(fs.highbit));
	current_writer()->writeAttribute("name", name);
    }
}


void emit_constant(const constant_spec &cs) {
    string scope = emit_scope();
    (*current_db->constant_map)[ cs.def ] = current_constant = new constant(cs);

    if ( current_field ) {
	current_field->constants[cs.def]    = current_constant;
    } else if (current_register) {
	current_register->constants[cs.def] = current_constant;
    } else if (current_group) { 
	current_group->constants[cs.def]    = current_constant;
    } else {
        cerr << "what scope is [" << cs.def << "] at?\n";
        exit(1);
    }

    QString name(cs.def);
    name.replace(QString::fromStdString(scope) + "_", QString(""),
		 Qt::CaseInsensitive);

    if ( emit_xml() ) {
	current_writer()->writeStartElement("enum");
	current_writer()->writeAttribute("name", name);
	current_writer()->writeAttribute("value", QString::number(cs.value));
	current_writer()->writeEndElement();
    }
    current_constant = 0;
}

//
// ---- rnndb ----
//
// the dom document paths below are used to read in the existing rnndb xml files.
// once in-memory the docs can be manipulated and written back out...
// along the read paths there's a set of encountered elements and attribute maps kept
// for debug purposes...
//
QMap<QString, QDomDocument *>          file_doc_map; // filename -> doc
QMap<QString, QMap<QString, QString> > elem_attr_set; // element names -> attr name maps

QDomDocument *read_doc_file(const QString &name, const QString &workdir);

void scan_node(QDomNode n);
void scan_attribute(QDomAttr a) {
    QDomElement e = a.ownerElement();
    QString tag = e.tagName();
    QString attr = a.name();
    if ( elem_attr_set[tag].find(attr) == elem_attr_set[tag].end() ) {
	elem_attr_set[tag][attr] = "1"; // only needs to exist, not have a proper value...
    }
}

void scan_element(QDomElement e) {
    QString tag = e.tagName();
    QDomNamedNodeMap attrs = e.attributes();
    int attr_count = attrs.size();
    for ( int a = 0; a < attr_count; a++ ) {
	scan_attribute(attrs.item(a).toAttr());
    }
    QDomNodeList c = e.childNodes();
    for ( int s = 0; s < c.length(); s++) {
	scan_node( c.at(s) );
    }
}

void scan_text(QDomText t) {
}

void scan_comment(QDomComment c) {

}

void scan_node(QDomNode n) {

    switch (n.nodeType()) {
    case QDomNode::ElementNode :
	scan_element(n.toElement());
	break;
    case QDomNode::AttributeNode :
	scan_attribute(n.toAttr());
	break;
    case QDomNode::TextNode:
	scan_text(n.toText());
	break;
    case QDomNode::CommentNode:
	scan_comment(n.toComment());
	break;

    case QDomNode::DocumentNode:
    case QDomNode::DocumentTypeNode:
    case QDomNode::DocumentFragmentNode:
    case QDomNode::EntityReferenceNode:
    case QDomNode::EntityNode:
    case QDomNode::ProcessingInstructionNode:
    case QDomNode::NotationNode:
    case QDomNode::BaseNode:
    case QDomNode::CharacterDataNode:
    case QDomNode::CDATASectionNode:
	throw domain_error("unhandled node");
	break;

    default:
	throw domain_error("unk node");
	break;
    }
}

QList<QDomDocument *>
scan_doc(QDomDocument *doc, const QString &workdir)
{
    // scan for and pull imports in first...
    QList<QDomDocument *> imported_docs;
    QDomNodeList import_nodes = doc->elementsByTagName("import");

    for (int n = 0; n < import_nodes.length(); n++ ) {
	QDomElement e = import_nodes.at(n).toElement();
	QString import_file_name = e.attribute("file", "");

	if ( file_doc_map.find( import_file_name ) == file_doc_map.end() ) {
	    QDomDocument *new_doc = read_doc_file(import_file_name, workdir);
	    imported_docs << new_doc;
	    file_doc_map[import_file_name] = new_doc;
	}
    }

    // now scan the tree node by node...
    QDomNode n = doc->documentElement().firstChild();
    while(!n.isNull()) {
	scan_node(n);
	n = n.nextSibling();
    }

    return imported_docs;
}

// no interpretation here... just reading the file into memory.
QDomDocument *read_doc_file(const QString &name, const QString &workdir)
{
    QFileInfo fi(name);

    QString file_name;
    if ( ! fi.isAbsolute() ) {
	file_name = QString("%1/%2").arg(workdir, name);
    } else {
	file_name = name;
    }

    QFile file(file_name);

    if ( ! file.open(QIODevice::ReadOnly) ) {
	cerr << "couldn't open " << file_name.toStdString() << endl;
	return 0;
    }

    QDomDocument *doc = new QDomDocument();
    if ( ! doc->setContent(&file) ) {
	cerr << "couldn't set content from " << file_name.toStdString() << endl;
	return 0;
    }
    return doc;
}

//
// this will read in the entire rnn db starting from the given (root) filename.
// but, only the files referred by to by imports will be pulled in.  files which
// are not referenced in the root hierarchy/content will be ignored.
//
QDomDocument * read_rnn_db(const QString &root_file_name)
{
    QFileInfo rfi(root_file_name);
    QString workdir = rfi.absoluteDir().absolutePath();
    QDomDocument *root_doc;
    QList<QDomDocument *> pending_docs;
    pending_docs.push_back( root_doc = read_doc_file(rfi.absoluteFilePath(), workdir));

    while ( pending_docs.size() ) {
	QList<QDomDocument *> imports = scan_doc(pending_docs.front(), workdir);
	pending_docs.pop_front();
	pending_docs.append( imports );
    }
    file_doc_map["root"] = root_doc;
    return root_doc;
}

//
// below are emitters for the traditional "drf" style of nvidia
// macros.  each _d_omain, _r_egister, _f_ield gets a specific
// style of definition.  there's some attention paid to pretty
// output (columns lining up, whatnot).
//
static std::vector<int> stops { 80, 95, 128 };
inline int stop_gap(int content_width) {
    int s = 0, gap;
    do {
	gap = stops[s] - (content_width);
	s++;
    } while ( (gap < 1) && (s < stops.size()) );
    gap = max(gap, 1);
    return gap;
}

std::string nv_drf_definition(group *g) {
    stringstream lhs, rhs, result;
    lhs << "#define " << g->def << " ";
    rhs << hex << "0x" << setfill('0') << setw (8) << g->base <<
	":0x" << setfill('0') << setw (8) << g->extent << " /*d*/";
    int gap = stop_gap(lhs.str().length() + rhs.str().length());
    result << lhs.str() << setfill(' ') << setw(gap) << "" << rhs.str() << endl;
    return result.str();
}

std::string nv_drf_definition(reg *r) { 
    stringstream lhs, rhs, result;
    lhs << "#define " << r->def << " ";
    rhs << hex << "0x" << r->offset << " /*r*/";
    int gap = stop_gap(lhs.str().length() + rhs.str().length());
    result << lhs.str() << setfill(' ') << setw(gap) << "" << rhs.str() << endl;
    return result.str();
}

std::string nv_drf_definition(field *f) {
    stringstream lhs, rhs, result;
    lhs << "#define " << f->def << " ";
    rhs << f->highbit << ":" << f->lowbit << " /*f*/";
    int gap = stop_gap(lhs.str().length() + rhs.str().length());
    result << lhs.str() << setfill(' ') << setw(gap) << "" << rhs.str() << endl;
    return result.str();
}

std::string nv_drf_definition(constant *c) {
    stringstream lhs, rhs, result;
    lhs << "#define " << c->def << " ";
    rhs << hex << "0x" << setw(8) << setfill('0') << c->value << " /*c*/";
    int gap = stop_gap(lhs.str().length() + rhs.str().length());
    result << lhs.str() << setfill(' ') << setw(gap) << "" << rhs.str() << endl;
    return result.str();
}

//
// following are functions which take a specific definition (domain, register,
// field, etc) and go searching the rnndb for elements which map to it.  once
// found those rnndb definitions are scoured for consistency/deltas.  deltas
// should cause new nodes with different variant members to be created.
// otherwise (i.e. if there is no real change to the d,r,f foo then) the
// variants alone are updated.
//
void integrate_group(group *G)
{
    // find any/all elements in the rnndb which cover the same group/domain
    // e.g. bus, mc, graph, ...
}
void integrate_register(reg *R)
{
    // find uses of register with either the same name or same offset
}
void integrate_field(field *F)
{
    // find uses of the same field
}
void integrate_constant(constant *C)
{
    // find uses of the same constant/enum
}

//
// this is a bit of a multi-function tool at the moment...
// the following will produce output that is quasi-legit:
//
//     ./gen_rnndb --verbose 1 --root ../rnndb/root.xml 
//
int main (int argc, char **argv)
{
    int rc = 0;
    char *cwd = getcwd(0,0);
    QDomDocument *doc = 0;

    if (!cwd) throw domain_error("can't get cwd");
    cout << "cwd = " << cwd << endl;

    if (cmdline_parser(argc, argv, &args_info) != 0)
	throw domain_error("can't parse cmd line input");

    // while reading the db side-effects generate are a list of elements encountered.
    // and with those elements a set of attrs found within each.  

    doc = read_rnn_db(QString(args_info.root_arg));

    // print out the list of encountered elements and their attrs.
    if (verbose()) {
	QStringList elem_tags = elem_attr_set.keys();
	for (QStringList::iterator e = elem_tags.begin(); e != elem_tags.end(); e++) {
	    QStringList attrs = elem_attr_set[*e].keys();
	    foreach (const QString &attr, attrs) {
		cout << *e << ":" << attr;
	    }
	}
    }

    //
    // working on a specific set of conditions here...
    // so, hard-coding for gk20a and gm20b.
    //
    struct emitted_db gk20a_db;
    struct emitted_db gm20b_db;

    current_db = &gk20a_db;
    emit_groups_gk20a();

    current_db = &gm20b_db;
    emit_groups_gm20b();

    current_db = 0;

    map<string, emitted_db *> db_map {
	{"gk20a", &gk20a_db },
	{"gm20b", &gm20b_db }
    };

    map<string, group    *>::iterator gi;
    map<string, reg      *>::iterator ri;
    map<string, field    *>::iterator fi;
    map<string, constant *>::iterator ci;
    emitted_db *db = &gk20a_db;

    // groups/domains
    for ( gi = db->group_map->begin(); gi != db->group_map->end(); gi++ ) {
	if (verbose()) { cout << nv_drf_definition(gi->second); }
	integrate_group(gi->second);
    }

    // registers
    for ( ri = db->register_map->begin(); ri != db->register_map->end(); ri ++ ) {
	if (verbose()) { cout << nv_drf_definition(ri->second); }
	integrate_register(ri->second);
    }
    
    // fields
    for ( fi = db->field_map->begin(); fi != db->field_map->end(); fi ++ ) {
	if (verbose()) { cout << nv_drf_definition(fi->second); }
	integrate_field(fi->second);
    }
    
    // consts/enums
    for ( ci = db->constant_map->begin(); ci != db->constant_map->end(); ci ++ ) {
	if (verbose()) { cout << nv_drf_definition(ci->second); }
	integrate_constant(ci->second);
    }


    chdir(cwd);
    return rc;
}
