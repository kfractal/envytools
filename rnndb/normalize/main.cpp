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

#include <cassert>
#include <set>
#include <stdexcept>
#include <queue>
#include <sstream>
#include <limits>
#include <algorithm>
#include <iostream>
#include <bitset>

#include <QDebug>
#include <QQueue>
#include <QTimer>
#include <QXmlSchema>
#include <QRegularExpression>

#include "main.h"
#include "ip_def.h"

QSet<gpuid_t *> var_all_gpus;

int main (int argc, char **argv)
{
	Main app(argc, argv);
	QTimer::singleShot(0, &app, SLOT(start()));
	return app.exec();
}

Main::Main(int &argc, char **argv) :
	QCoreApplication(argc, argv), _in(stdin), _out(stdout), _err(stderr),
	_validator(_schema), _validator_msg_handler(&_out, &_err, this)
{
	_root = "root.xml";
	_validator.setMessageHandler(&_validator_msg_handler);
	_current_stream_reader = 0;
}


void Main::start()
{
	//	if ( check_exec_environment() ) {
	if ( QFileInfo(_root).exists() ) {
		if (_verbose) {
			out() << "info: using root " << _root << endl;
		}
	} else {
		out() << "error: can't find root " << _root << endl;
		exit(-1);
		return;
	}
	//	}

	//
	// read nvidia's hwref files.  only whitelisted definitions
	// are returned from this operation.
	//
	vector<def_tree_t*> gpu_def_trees = read_ip_defs();

	def_tree_t *aggregate_tree = new def_tree_t(gpu_def_trees);
	def.coalesced = new def_tree_t(aggregate_tree);
	accelerate_defs();

	if (_verbose) {
		out() << "info: processed nvidia hwref files for" << endl;
		for ( auto &t: target_gpus ) {
			out() << "info:\t" << t->name().c_str() << endl;
		}

		out() << "info: the ip whitelist contains " <<
			ip_whitelist::chip_groups.size()    << " groups, " <<
			ip_whitelist::chip_regs.size()      << " registers, " <<
			ip_whitelist::chip_fields.size()    << " fields and " <<
			ip_whitelist::chip_constants.size() << " constants overall." << endl;

		out() << "info: the ip whitelist deletes " <<
			ip_whitelist::chip_deleted_groups.size()    << " groups, " <<
			ip_whitelist::chip_deleted_regs.size()      << " registers, " <<
			ip_whitelist::chip_deleted_fields.size()    << " fields and " <<
			ip_whitelist::chip_deleted_constants.size() << " constants overall." << endl;

	}

	for ( auto t : target_gpus ) 
		var_all_gpus.insert(t);

	// now read in the xml hierarchy (starting with root.xml).

	// XXX: defaults not actually set yet...
	// should almost certainly involve the schema.
	attr_spec_t defaults;
	defaults._offset.v = 0; // int doesn't have default constructor

	defaults.zap_spec_bits(); // mark as inherited values...
	_attr_stack.push(defaults);

	// xml hierarchy traversal
	int rc = read_rnndb_root();

	if ( _warn || _verbose ) {
		if ( _verbose)  {
			for ( auto rf : _read_files ) {
				out() << "info: read " << rf << endl;
			}
		}
		if ( !rc ) {
			for ( auto xf : _all_xml_files ) {
				if ( _read_files.find(xf) == _read_files.end() ) {
					out() << "warning: didn't use " << xf << " ?" << endl;
				}
			}
		}
	}


	// write out the xml files.
	if ( _emit_normalized_hierarchy ) {
		for ( auto xf : _all_xml_files ) {
			emit_file(xf);
		}
	}

	QFileInfo file_info("results/additions.xml");
	QDir file_dir = file_info.dir();
	if ( !file_dir.exists() ) {
		if ( !file_dir.mkpath(file_dir.path()) ) {
			out() << "error: couldn't make path " << file_dir.path() << endl;
			return;
		}
	}
	QFile out_file("results/additions.xml");
	if ( !out_file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text) ) {
		out() << "error: couldn't open result file " << out_file.fileName() << endl;
	}

	//QString ns("http://nouveau.freedesktop.org/");
	QString ns;

	file_content_t addit("results/additions.xml");

	for ( auto group_set : def.coalesced->groups_by_name ) {
		for ( auto group : group_set.second ) {
			for ( auto reg_set : group->regs_by_name ) {
				for ( auto reg : reg_set.second ) {
					// mark is "was it already added/updated elsewhere?".
					if (!reg->mark) {
						produce_register_content(reg, &addit);
						reg->mark = 1;
					}
				}
			}
		}
	}


	QXmlStreamWriter out_xml(&out_file);
	out_xml.setAutoFormatting(true);
	for ( auto node : addit.nodes ) {
		node->write(out_xml);
	}
	out_file.close();


	// gpu variant sets enumeration
	{
		QFileInfo file_info("results/gpus.xml");
		QDir file_dir = file_info.dir();
		if ( !file_dir.exists() ) {
			if ( !file_dir.mkpath(file_dir.path()) ) {
				out() << "error: couldn't make path " << file_dir.path() << endl;
				return;
			}
		}
		QFile out_file("results/gpus.xml");
		if ( !out_file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text) ) {
			out() << "error: couldn't open result file " << out_file.fileName() << endl;
		}
		file_content_t gpus("results/gpus.xml");
		xml_element_node_t *v = produce_gpu_variants_content(0);
		gpus.nodes.append(v);


		QXmlStreamWriter out_xml(&out_file);
		out_xml.setAutoFormatting(true);
		for ( auto node : gpus.nodes ) {
			node->write(out_xml);
		}
		out_file.close();
	}


	exit(rc); // Main::exit() sets *intent* to exit w/rc.
	return;
}

int Main::read_rnndb_root()
{
	_root_dir = QDir(QFileInfo(_root).dir());
	if (_verbose) out() << "info: root dir is " << _root_dir.absolutePath() << endl;

	// this is used to create a superset of files which can be
	// re-written in normalized form.  however, the xml file list isn't
	// used to drive the discovery of elements.  that happens by way
	// of the element parser... (through import statements).
	find_xml_files();

	int rc = read_path(_root);

	return rc;
}

// returns to current at the end
int Main::read_path(const QString &file_path)
{
	QFileInfo file_info(file_path);
	QString file_name = file_info.fileName();

	if (_verbose) out() << "info: file path " << file_path << endl;

	QFile file(file_path);
	bool ok;

	ok = file.open(QIODevice::ReadOnly | QIODevice::Text);

	int r = -1;
	if ( ok ) {
		r = read_file(&file);
	} else {
		out() << "error: couldn't open " << file_path << endl;
	}

	return r;
}

int Main::read_file(QFile *file)
{
	if (_skip_multiple_file_scans && 
		(_read_files.find(file->fileName()) != _read_files.end()) ) {
		return 0;
	}
	_current_file.push(QFileInfo(*file));
	_read_files.insert(file->fileName());

	if (_validate_schema && _schema.isValid() ) {

		if ( _verbose) out() << "info: validating " << file->fileName() << endl;
		// note: the url here has some sort of namespace impact?
		// names like "bus/pmc.xml"/ make the validator choke in weird places.
		// i have no idea why yet (using external web-based checkers say they're
		// fine).  just hacking it for now while i try to understand it.
		QFileInfo fi(*file);
		if (_validator.validate(file , QUrl::fromLocalFile(fi.fileName()))) {
			// ok
		}
		else {
			out() << "error: instance document " << file->fileName() << " is invalid" << endl;
			// more verbose error coming out of the validator already.
			_current_file.pop();
			return -1;
		}
		file->seek(0); // validator likely moved it :)
	}

	int rc = 0;

	QXmlStreamReader xml(file);
	_current_stream_reader = &xml;

	// ...
	while (!xml.atEnd()) {
		auto token_type = xml.readNext();
		// ...  do (recursive descent style, graph building) processing

		switch (token_type) {
			case QXmlStreamReader::StartDocument:
				rc = handle_document(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::EndDocument:
				rc = end_document(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::StartElement:
				rc = handle_element(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::EndElement:
				rc = end_element(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::Characters:
				rc = handle_chars(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::Comment:
				rc = handle_comment(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::DTD:
				rc = handle_dtd(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::EntityReference:
				rc = handle_entity_reference(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::ProcessingInstruction:
				rc = handle_processing_instruction(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::NoToken:
				rc = handle_no_token(xml);
				if (rc)
					goto done;
				break;
			case QXmlStreamReader::Invalid:
			default:
				out() << "error: root invalid/unk token" << endl;
				rc = -1;
				goto done;
				break; /*NOTREACHED*/
		}
	}
 done:
	if (xml.hasError()) {
		out() << "error: xml has error?\n";
		rc = -1;
	}
	if (_debug) {
		out()  << "debug: file complete " << _current_file.top().fileName() << endl;
	}
	_current_file.pop();

	_current_stream_reader = 0;
	return rc;
}

// misc junk for handlers
static void show_reg32_attr_spec(const attr_spec_t &a);

//
// handlers for specific elements and attributes etc are below.
// note that the xml schema validator while in place isn't working properly
// yet (not sure whether that's due to faults in the xml, or what, though
// some were fixed already).
//
// there shouldn't be too much in the way of error handling happening here
// re: illegal elements, or attribute names.  the validator will (eventually)
// catch those.  but, out of bounds and other semantic problems can still occur.
//

#define ELEMENT(X) { #X , &Main::handle_ ## X },

QMap<QString, Main::element_handler_t> Main::_element_handlers {
	ELEMENTS()
	{ "use-group", &Main::handle_use_group },
};
#undef ELEMENT

#define ELEMENT(X) { #X , &Main::close_ ## X },

QMap<QString, Main::element_handler_t> Main::_element_closers {
	ELEMENTS()
	{ "use-group", &Main::close_use_group },
};
#undef ELEMENT


#define name_str(X) (X.name().toString())

int Main::handle_element(QXmlStreamReader &e)
{
	auto f = _element_handlers.find(name_str(e));
	if ( f == _element_handlers.end() ) {
		_ignored_elements.insert(name_str(e));
		out() << "error: unknown element " << name_str(e) << " at line " <<
			e.lineNumber() << " of " << _current_file.top().filePath() << endl;
		return -1;
	}

	//
	// the attribute stack records where attributes have been
	// specified, both values as well as bits for true/false
	// (given at that level of the stack or not).
	//
	// the stack is massaged/flattened here so that the handlers can
	// treat the stack as a pair of attr sets:
	//     . attrs specified local to the element
	//     . attrs specified *before* the element
	//
	_current_element.push(name_str(e));
	_attr_stack.push(_attr_stack.top());

	//----------------------------------------------------------------------
	file_content_t *content = current_file_content();
	xml_element_node_t *enode = new xml_element_node_t( name_str(e), e.attributes(),
														content->current_node.top() );

	content->current_node.top()->nodes.append(enode);
	content->current_node.push(enode);
	//----------------------------------------------------------------------

	int rc = (this->**f)(e); // records attrs seen in top()

	if (rc) {
		out() << "error: failed handler for " << name_str(e) << " at line " <<
			e.lineNumber() << " of " << _current_file.top().filePath() << endl;
	}

	return rc;
}

int Main::end_element(QXmlStreamReader &e)
{
	int rc;
	file_content_t *content = current_file_content();
	auto f = _element_closers.find(name_str(e));
	if ( f == _element_closers.end() ) {
		out() << "error: failed to find closer for " << name_str(e) << endl;
		_ignored_elements.insert(name_str(e));
		rc = -1;
		goto done;
	}

	rc = (this->**f)(e);

	if (rc) {
		out() << "error: failed closer for " << name_str(e) << " at line " <<
			e.lineNumber() << " of " << _current_file.top().filePath() << endl;
	}

 done:
	if ( _current_element.size() == 0 ) {
		out() << "error: about to pop empty element stack?" << endl;
	}
	_current_element.pop();
	content->last_node = content->current_node.top();
	content->current_node.pop();

	if ( _attr_stack.size() <= 1 ) {
		out() << "error: about to hit empty attr stack?" << endl;
	}
	_attr_stack.pop();
	_attrs = _attr_stack.top();
	return rc;
}

int Main::handle_import(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( a.name() == "file" ) {
			if ( _verbose ) out() << "info: importing file " << a.value().toString() << endl;

			rc = read_path(a.value().toString());
			if ( rc )
				return rc;
		} else {
			ignored_attr(e, a);
		}
	}
	return rc;
}

int Main::close_import(QXmlStreamReader &)
{
	return 0;
}

#undef ATTR
#undef ELEMENT
#define ATTR(X)    static QString X##_str { #X };
#define ELEMENT(E) namespace E ##_element { E ## _ELEMENT_ATTRS() };
ELEMENTS()
#undef ELEMENT
#undef ATTR

//
// XXX: would prefer closed form solution.  but at least
// find a way to note where to add/remove attribute set tweak
// locations...
//
void Main::flatten_attrs(const attr_spec_t &a)
{
#define assign_attr(X) do { if (a._##X.b) { _attrs._##X.from       (a._##X); } } while (0)
#define concat_attr(X) do { if (a._##X.b) { _attrs._##X.concat_from(a._##X); } } while (0)
	if ( a._name.b   ) _attrs._name.  concat_from(a._name, "::");
	if ( a._offset.b ) _attrs._offset.add_from   (a._offset);
	if ( a._pos.b    ) { // pos overrides high and low (locally)
		_attrs._high.from(a._pos.v);
		_attrs._low. from(a._pos.v);
		_attrs._pos. from(a._pos.v);
	} else {
		assign_attr(low);
		assign_attr(high);
	}
	
	assign_attr(bare);
	assign_attr(inline);
	assign_attr(access);
	assign_attr(type);
	assign_attr(varset);
	assign_attr(prefix);
	assign_attr(year);
	assign_attr(email);
	assign_attr(value);
	assign_attr(shr);
	assign_attr(size);
	assign_attr(stride);
	assign_attr(length);
	assign_attr(width);
	assign_attr(align);
	assign_attr(max);
	assign_attr(min);
	assign_attr(add);
	assign_attr(radix);
	assign_attr(variants);
}


#define ATTR(X) else if ( name_str(a) == #X ) { local_attrs . _##X .from(a.value()); }
#define STACK_ELEMENT_ATTRS(E)						\
	attr_spec_t local_attrs;						\
	int rc = 0;										\
	do {											\
		for ( auto a: e.attributes() ) {			\
			if (0) { }								\
			E ## _ELEMENT_ATTRS()					\
			else { rc = -1; ignored_attr(e,a); }	\
		}											\
		if ( rc )	{								\
			out() << "error: rc failed in " << __func__	<< endl;	\
			return rc;								\
		}											\
		flatten_attrs(local_attrs);					\
		_attr_stack.top() = _attrs;					\
	} while (0)

//
// element handlers, closers
//
// the 'closers' are kept because i have an idea they'll be needed
// later.  but perhaps not...
//
int Main::handle_array(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(array);
	if (false) { 
		out() << QString("array: %1 0x%2").arg(_attrs._name.v).
			arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0')) << endl;
	}

	return rc;
}

int Main::close_array(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_domain(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(domain);
	if (false) {
		out() << QString("domain: %1").arg(_attrs._name.v) << endl;
	}

	return rc;
}

int Main::close_domain(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_doc(QXmlStreamReader &e) //element, not structure
{
	STACK_ELEMENT_ATTRS(doc);
	return rc;
}

int Main::close_doc(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_spectype(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(spectype);
	return rc;
}

int Main::close_spectype(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_b(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(b);
	return rc;
}

int Main::close_b(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_reg16(QXmlStreamReader &e)
{
	//uint64_t inherited_offset = _attrs._offset.v;

	STACK_ELEMENT_ATTRS(reg16);

	//	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;
	QSet<gpuid_t *> var_gpus;
	if ( _attrs._variants.v.size() ) {
		var_gpus = variants_to_gpus(_attrs._variants.v);
	} else {
		var_gpus = var_all_gpus;
	}
	return rc;
}

int Main::close_reg16(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_database(QXmlStreamReader &e)
{
	static QMap<QString, bool> schema_cache;

	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( a.name() == "schemaLocation" ) {
			QString attr_val = a.value().toString();
			// schema lookup (success or failure) can be expensive.
			// don't do it more than once per url as the answer won't change.
			if ( schema_cache.find(attr_val) == schema_cache.end() ) {
				// split the string into a whitespace delimited set.
				// of those look for "*.xsd" files.  if a file local to root
				// exists matching that name, then use it.  otherwise punt.
				QStringList seps = attr_val.split(QRegularExpression("\\s+"));
				for ( auto s : seps ) {
					QUrl surl(s);
					if ( surl.isRelative() ) {
						QString local = _root_dir.absolutePath() + "/" + s;
						QFileInfo sfi(local);
						if (sfi.exists()) {
							QFile file(sfi.absoluteFilePath());
							if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
								if (_verbose) out() << "info: loading db schema from " << file.fileName() << endl;
								_schema.load(&file, QUrl::fromLocalFile(file.fileName()));
								if ( !_schema.isValid()) {
									out() << "warning: invalid schema " << sfi.absoluteFilePath() << endl;
								}
								schema_cache[attr_val] = true;
							}
						}
					}
				}
				if ( schema_cache.find(attr_val) == schema_cache.end() ) {
					// nothing above was tricked into believing a real one
					// was available.  so at least don't try any longer with
					// this same request string...
					schema_cache[attr_val] = false;
				}
			}
		} else {
			ignored_attr(e,a);
		}
	}
	return rc;
}

int Main::close_database(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_bitset(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(bitset);
	return rc;
}

int Main::close_bitset(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_copyright(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(copyright);
	return rc;
}

int Main::close_copyright(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_reg8(QXmlStreamReader &e)
{
	//	uint64_t inherited_offset = _attrs._offset.v;

	STACK_ELEMENT_ATTRS(reg8);

	//	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;
	QSet<gpuid_t *> var_gpus;
	if ( _attrs._variants.v.size() ) {
		var_gpus = variants_to_gpus(_attrs._variants.v);
	} else {
		var_gpus = var_all_gpus;
	}

	return rc;
}

int Main::close_reg8(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_nick(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(nick);
	return rc;
}
int Main::close_nick(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_license(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(license);
	return rc;
}

int Main::close_license(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_enum(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(enum);
	return rc;
}

int Main::close_enum(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_use_group(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(use_group);
	return rc;
}

int Main::close_use_group(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_li(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(li);
	return rc;
}

int Main::close_li(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_reg64(QXmlStreamReader &e)
{
	//	uint64_t inherited_offset = _attrs._offset.v;

	STACK_ELEMENT_ATTRS(reg64);

	//	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;

	return rc;
}

int Main::close_reg64(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_reg32(QXmlStreamReader &e)
{
	//	uint64_t inherited_offset = _attrs._offset.v;

	STACK_ELEMENT_ATTRS(reg32);

	// this offset is the flattened version...
	uint64_t absolute_offset = _attrs._offset.v;

	QString name    = _attrs._name.v;

	QVector<QString> name_pieces = name.split("::").toVector();
	if ( name_pieces.size() && (name_pieces[0] != "NV_MMIO") ) {
		return 0;
	}
	file_content_t *content = current_file_content();

	xml_element_node_t *enode = 
		dynamic_cast<xml_element_node_t*>(content->current_node.top());

	if ( !enode ) {
		throw logic_error("expected to find an element "
						  "(xml_element_node_t*) at top");
	}

	QSet<gpuid_t *> var_gpus;
	if ( _attrs._variants.v.size() ) {
		var_gpus = variants_to_gpus(_attrs._variants.v);
	} else {
		var_gpus = var_all_gpus;
	}

	//
	// Find register defns with the same offset.
	// The point here is to discern what if any modifications
	// should be made to the xml at this point to reflect information
	// within the def tree(s).
	//
	auto regs_at_value = def.regs_by_value_and_name.find(absolute_offset);
	if ( regs_at_value != def.regs_by_value_and_name.end() ) {
		//		out()  << "info: value 0x" << hex << absolute_offset << endl;
		for ( auto reg_at_name : regs_at_value->second ) {
			//	out()  << "info:\tname " << reg_at_name.first.c_str() << endl;
			for ( auto reg : reg_at_name.second ) {
				/*xml_element_node_t *new_reg32 =*/ produce_register_content(reg, enode);
				reg->mark = 1;
				//	out() << "info:\t\tgpus " << gpus_to_variants(reg->def_gpus) << endl;
			}
		}
	}

	return 0;
}

//
// note: not all reg32_ELEMENT_ATTRS() are processed yet.
//
int Main::close_reg32(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_brief(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(brief);
	return rc;
}

int Main::close_brief(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_author(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(author);
	return rc;
}

int Main::close_author(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_ul(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(ul);
	return rc;
}

int Main::close_ul(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_bitfield(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(bitfield);

	QSet<gpuid_t *> var_gpus;

	if ( _attrs._variants.v.size() ) {
		var_gpus = variants_to_gpus(_attrs._variants.v);
	} else {
		var_gpus = var_all_gpus;
	}

	if (false) {
		out() << QString("bitfield: %1 %2:%3").arg(_attrs._name.v).
			arg(_attrs._high.v).arg(_attrs._low.v) << endl;
	}

	return rc;
}

int Main::close_bitfield(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_stripe(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(stripe);
	if (false) {
		out() << QString("stripe: %1 0x%2").arg(_attrs._name.v).
			arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0')) << endl;
	}

	return rc;
}

int Main::close_stripe(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_group(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(group);
	return rc;
}

int Main::close_group(QXmlStreamReader &)
{
	return 0;
}

bool Main::in_bitfield()
{
	for ( auto i = 0; i < _current_element.length(); i++)
		if ( _current_element[i] == "bitfield" )
			return true;
	return false;
}
int Main::handle_value(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(value);

	QSet<gpuid_t *> var_gpus;

	if ( _attrs._variants.v.size() ) {
		var_gpus = variants_to_gpus(_attrs._variants.v);
	} else {
		var_gpus = var_all_gpus;
	}

	QString c_str; if ( _attrs._offset.b ) {
		c_str = QString(" 0x%1").arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0'));
	}
	if ( _attrs._high.b && _attrs._low.b ) { // recall just giving pos -> defines these
		c_str += QString(" %1:%2").arg(_attrs._high.v).arg(_attrs._low.v);
	}
	if ( in_bitfield() ) {
		if (false) {
			out() << QString("bitfield value: %1 %2%3").
				arg(_attrs._name.v).arg(_attrs._value.v).arg(c_str) << endl;
		}
	} else {
		if (false) {
			out() << QString("value: %1 %2").
				arg(_attrs._name.v).arg(_attrs._value.v) << endl;
		}
	}

	return rc;
}

int Main::close_value(QXmlStreamReader &)
{
	return 0;
}
#undef ATTR

// e.g.: const vector<string> Main::array_element_attrs { "length", ... };
#define ELEMENT(X) const vector<string> Main:: X ##_element_attrs { X ## _ELEMENT_ATTRS() };
#define ATTR(X) #X,
ELEMENTS()
#undef ATTR
#undef ELEMENT



static void show_reg32_attr_spec(const attr_spec_t &a)
{
#undef ATTR
#define ATTR(X) /* qDebug()... << */ << QString("%1").arg( a. _## X . b )
	QDebug d = qDebug();
	QDebugStateSaver saver(d);
	qDebug().nospace().noquote()
		reg32_ELEMENT_ATTRS() ;
#undef ATTR
}

void Main::dump_attr_stack()
{
	int depth = _attr_stack.length();
	for ( int a = 0; a < depth; a++) {
		const attr_spec_t &attr = _attr_stack[a];
		show_reg32_attr_spec(attr);
	}
}

void Main::find_xml_files()
{
	QRegularExpression file_name_re(".*\\.xml");
	QMap<QString, QDir *> dirmap;
	QQueue<QDir *>        dirq;
	QMap<QString, QFileInfo *> filemap;
	QQueue<QFileInfo *>        fileq;

	// prime the pump
	dirq.enqueue(&_root_dir);

	//
	// A queue and marker scheme is used traverse the
	// directory hierarchy (instead of recursion).
	//
	while ( dirq.size() ) {

		QDir *dir = dirq.dequeue();
		if ( !dir->exists() )
			continue;

		QFileInfoList dirlist = dir->entryInfoList();

		// push children dirs into the queue.  push files which match the
		// file search expr into the fileq.

		for ( QFileInfoList::iterator i = dirlist.begin(); i != dirlist.end(); i++) {
			QString full = i->path() + "/" + i->fileName();
			if ( i->isFile() ) {
				QRegularExpressionMatch m = file_name_re.match(i->fileName());
				bool matched = m.hasMatch();
				if ( !matched )
					continue;
				fileq.enqueue( new QFileInfo( full ) );
			}
			else if ( i->isDir() ) {
				if ( i->fileName() == "." || i->fileName() == ".." )
					continue;

				if ( dirmap.find(full) == dirmap.end() ) {
					QDir * d = new QDir(full);
					dirq.enqueue(d);
					dirmap[full] = d;
				}
			}
		}
	}

	while ( fileq.size() ) {
		QFileInfo *fi = fileq.dequeue();
		QString root_rel_path = fi->filePath();
		if ( fi->filePath().startsWith("./") ) {
			root_rel_path.remove(0,2);
		}
		_all_xml_files.insert(root_rel_path);
	}
	if (_verbose) {
		out () << "info: the number of xml files is " << _all_xml_files.size() << endl;
		for ( auto v: _all_xml_files ) {
			out() << "info:" << v << endl;
		}
	}
}

//
// specializations of spec_t<T> for conversions, etc.
//
template<> void spec_t<bool>::from(const QStringRef &r)
{
	b = true;
	if ( ( 0 == r.compare("yes", Qt::CaseInsensitive) )  ||
		 ( 0 == r.compare("true", Qt::CaseInsensitive) ) ||
		 ( r == "1" ) )
		v = true;
	v = false;
}
template<> void spec_t<QString>::from(const QStringRef &r)
{
	v = r.toString();
	b = true;
}
template<> void spec_t<uint32_t>::from(const QStringRef &r)
{
	bool ok;
	v = r.toUInt(&ok); b = true;
	// XXX docs for QString::to*Int/Long says it spots the "0x" on its own.
	// but empirically, it does not...
	if ( !ok && r.startsWith("0x", Qt::CaseInsensitive) ) {
		v = r.toUInt(&ok, 16);
		if ( !ok ) {
			QTextStream out(stdout); // hack
			out  << "error: couldn't turn this: \"" << v << "\" into a number." << endl;
			v = ~(uint32_t)0;
		}
	}
}

void ValidatorMessageHandler::handleMessage(QtMsgType type, const QString & description,
							   const QUrl & identifier, const QSourceLocation & sourceLocation)
{
	QString warning("warning");
	QString error("error");
	if ( type == QtFatalMsg ) {
		(*_out) << "error: schema validation failure: [";
	} else {
		(*_out) << "warning: schema validation: ";
	}
	(*_out) << description << "] " <<
		" id: " << identifier.toString() <<
		" at line " << sourceLocation.line() <<
		" of " <<	sourceLocation.uri().toString() <<
		" column " << sourceLocation.column() << endl;
}

file_content_t *Main::current_file_content()
{
	if ( _current_file.isEmpty() )
		throw runtime_error("missing current file context");

	QString current_file_path = _current_file.top().filePath();
	if ( _xml_file_content.find(current_file_path) == _xml_file_content.end() ) {
		_xml_file_content[current_file_path] = file_content_t(current_file_path);
	}
	return &_xml_file_content[current_file_path];
}

int Main::handle_document(QXmlStreamReader &e) // the xml structure, not the "document" element
{
	file_content_t *content = current_file_content();
	xml_document_node_t *doc = new xml_document_node_t();
	content->nodes.append(doc);
	content->current_node.push(doc);

	if (false) {
		out() << _current_file.top().filePath() << ".document[" << e.text().toString() << endl;
	}
	return 0;
}

int Main::end_document(QXmlStreamReader &e)
{
	file_content_t *content = current_file_content();
	if (false) {
		out() << e.text().toString() << "]end document " << _current_file.top().filePath() << endl;
	}
	content->current_node.pop();
	return 0;
}

int Main::handle_chars(QXmlStreamReader &e)
{
	if ( e.isWhitespace() )
		return 0;

	QString file = _current_file.top().filePath();
	if (false) {
		out() << "chars[" << e.text().toString() << "]\n";
	}

	file_content_t *content = current_file_content();
	xml_chars_node_t *chars = new xml_chars_node_t(e.text().toString(), e.isCDATA());
	content->current_node.top()->nodes.append(chars);
	return 0;
}

int Main::handle_comment(QXmlStreamReader &e)
{
	if (false) {
		out() << "comment[" << e.text().toString() << "]\n";
	}

	file_content_t *content = current_file_content();

	if ( content->last_node ) {
		// kind of a hack.  shift comments into the actual text.  they can't
		// really be manipulated wrt the auto-formatting otherwise.
		xml_chars_node_t * chars = new xml_chars_node_t (e.text().toString() ,false,
														 content->last_node);
		content->last_node->nodes.append(chars);
	} else {
		xml_comment_node_t *comment = new xml_comment_node_t(e.text().toString());
		content->current_node.top()->nodes.append(comment);
	}

	return 0;
}

int Main::handle_dtd(QXmlStreamReader &e)
{
	if (false) {
		out() << "dtd[]\n";
	}
	file_content_t *content = current_file_content();
	xml_dtd_node_t *dtd = new xml_dtd_node_t(e.text().toString(),
											 e.notationDeclarations(),
											 e.entityDeclarations());
	content->current_node.top()->nodes.append(dtd);

	return 0;
}

int Main::handle_entity_reference(QXmlStreamReader &e)
{
	if (false) {
		out() << "entity ref[]\n";
	}
	file_content_t *content = current_file_content();
	xml_entity_reference_node_t *ref =
		new xml_entity_reference_node_t(e.name().toString(), e.text().toString());
	content->current_node.top()->nodes.append(ref);
	// should this be an error?
	return 0;
}

int Main::handle_processing_instruction(QXmlStreamReader &e)
{
	if (false) {
		out() << "processing instruction[]\n";
	}
	file_content_t *content = current_file_content();
	xml_processing_instruction_node_t *pi =
		new xml_processing_instruction_node_t(e.processingInstructionTarget().toString(),
											  e.processingInstructionData().toString());
	content->current_node.top()->nodes.append(pi);

	return 0;
}

int Main::handle_no_token(QXmlStreamReader &/*e*/)
{
	if (false) {
		out() << "no token[]\n";
	}
	return 0;
}


void Main::emit_file(const QString &rel_name)
{
	if ( _xml_file_content.find(rel_name) == _xml_file_content.end() ) {
		out() << "info: no content for " << rel_name << endl;
		return;
	}

	file_content_t &file_content = _xml_file_content[rel_name];
	QFileInfo file_info("results/" + rel_name);
	QDir file_dir = file_info.dir();
	if ( !file_dir.exists() ) {
		if ( !file_dir.mkpath(file_dir.path()) ) {
			out() << "error: couldn't make path " << file_dir.path() << endl;
			return;
		}
	}
	QFile out_file("results/" + rel_name);
	if ( !out_file.open(QIODevice::WriteOnly|QIODevice::Truncate|QIODevice::Text) ) {
		out() << "error: couldn't open result file " << out_file.fileName() << endl;
	}

	//QString ns("http://nouveau.freedesktop.org/");
	QString ns;

	QXmlStreamWriter out_xml(&out_file);
	out_xml.setAutoFormatting(true);
	for ( auto node : file_content.nodes ) {
		node->write(out_xml);
	}
	out_file.close();
}

// missing modern gpus
// { "GM200" },

// legacy gpus
struct legacy_gpu_t {
	string name;
	legacy_gpu_t(const char *g) : name(g) { }
	legacy_gpu_t(const string s) : name(s) { }
	bool operator < (const legacy_gpu_t &o) const { return name < o.name; }
};
set<legacy_gpu_t> legacy_gpus {
	{ "c51" },
	{ "g70" },
	{ "g71" },
	{ "g72" },
	{ "g73" },
	{ "nv10" },
	{ "nv11" },
	{ "nv15" },
	{ "nv17" },
	{ "nv18" },
	{ "nv1a" },
	{ "nv1f" },
	{ "nv1" },
	{ "nv20" },
	{ "nv25" },
	{ "nv30" },
	{ "nv31" },
	{ "nv34" },
	{ "nv35" },
	{ "nv36" },
	{ "nv3t" },
	{ "nv3" },
	{ "nv40" },
	{ "nv41" },
	{ "nv43" },
	{ "nv44a" },
	{ "nv44" },
	{ "nv4" },
	{ "nv5" },
};

// class/interface names, etc
struct class_names_t {
	string name;
	class_names_t(const char *n) : name(n) { }
	class_names_t(const string s) : name(s) { }
	bool operator < (const class_names_t &o) const { return name < o.name; }
};

set<class_names_t> class_names {
	{ "adt7473" },
	{ "adt7473_1" },
	{ "fp" },
	{ "g200_3d" },
	{ "g80_2d" },
	{ "g80_3d" },
	{ "g80_compute" },
	{ "g80_m2mf" },
	{ "g80_sifm" },
	{ "g80_surface_2d" },
	{ "g84_3d" },
	{ "g84_cipher" },
	{ "gf100_2d" },
	{ "gf100_3d" },
	{ "gf100_compute" },
	{ "gf100_m2mf" },
	{ "gf108_3d" },
	{ "gf110_3d" },
	{ "gf110_compute" },
	{ "gk104_3d" },
	{ "gk104_compute" },
	{ "gk104_copy" },
	{ "gk104_p2mf" },
	{ "gk110_compute" },
	{ "gk110_p2mf" },
	{ "gm107_copy" },
	{ "gm204_3d" },
	{ "gp" },
	{ "gt215_3d" },
	{ "gt215_compute" },
	{ "mcp89_3d" },
	{ "nv10_3d" },
	{ "nv10_dvd_subpicture" },
	{ "nv10_ifc" },
	{ "nv10_multitex_triangle" },
	{ "nv10_sifm" },
	{ "nv10_surface_3d" },
	{ "nv10_textured_triangle" },
	{ "nv10_texupload" },
	{ "nv15_3d" },
	{ "nv15_blit" },
	{ "nv17_3d" },
	{ "nv1_beta" },
	{ "nv1_bitmap" },
	{ "nv1_blit" },
	{ "nv1_chroma" },
	{ "nv1_clip" },
	{ "nv1_ifc" },
	{ "nv1_ifm" },
	{ "nv1_itm" },
	{ "nv1_line" },
	{ "nv1_lin" },
	{ "nv1_pattern" },
	{ "nv1_point" },
	{ "nv1_rect" },
	{ "nv1_rop" },
	{ "nv1_texlinbeta" },
	{ "nv1_texlin" },
	{ "nv1_texquadbeta" },
	{ "nv1_texquad" },
	{ "nv1_tri" },
	{ "nv20_3d" },
	{ "nv25_3d" },
	{ "nv30_3d" },
	{ "nv35_3d" },
	{ "nv3_gdi" },
	{ "nv3_m2mf" },
	{ "nv3_rop" },
	{ "nv3_sifc" },
	{ "nv3_sifm" },
	{ "nv3_textured_triangle" },
	{ "nv40_3d" },
	{ "nv40_ifc" },
	{ "nv40_index" },
	{ "nv40_lin" },
	{ "nv40_me" },
	{ "nv40_sifc" },
	{ "nv40_surface_swz" },
	{ "nv40_texupload" },
	{ "nv41_vp1" },
	{ "nv44_3d" },
	{ "nv4_beta4" },
	{ "nv4_blit" },
	{ "nv4_chroma" },
	{ "nv4_dvd_subpicture" },
	{ "nv4_gdi" },
	{ "nv4_ifc" },
	{ "nv4_index" },
	{ "nv4_lin" },
	{ "nv4_multitex_triangle" },
	{ "nv4_pattern" },
	{ "nv4_rect" },
	{ "nv4_sifc" },
	{ "nv4_sifm" },
	{ "nv4_surface_2d" },
	{ "nv4_surface_3d" },
	{ "nv4_surface_swz" },
	{ "nv4_textured_triangle" },
	{ "nv4_tri" },
	{ "nv5_ifc" },
	{ "nv5_index" },
	{ "nv5_sifc" },
	{ "nv5_sifm" },
	{ "tcp" },
	{ "tep" },
};


gpuid_t *Main::target_gpu_by_name(const QString &gpu_name)
{
	gpuid_t *gpuid = 0;
	string gpu_name_str = gpu_name.toLower().toStdString();
	auto f = target_gpus_by_name.find(gpu_name_str);
	if ( f == target_gpus_by_name.end() ) {
		if ( legacy_gpus.find(gpu_name_str) == legacy_gpus.end() ) {
			if ( class_names.find(gpu_name_str) == class_names.end() ) {
				// no match, anywhere.
				out() << "error: couldn't find a category to place [" <<
					gpu_name_str.c_str() << "]" << endl;
				return 0;
			} else {
				// it matched a class id
				return 0;
			}
		} else {
			// it is a legacy gpu.
			f = target_gpus_by_name.find("legacy");
			if ( f == target_gpus_by_name.end() ) {
				// shouldn't happen
				out() << "error: failed to find the legacy gpu id" << endl;
				return 0;
			}
			gpuid = f->second;
			if ( _warned_about_legacy_gpu.find(gpu_name_str) == _warned_about_legacy_gpu.end() ) {
				out() << "warning: matched 'legacy' for gpu [" << gpu_name_str.c_str() << "]" << endl;
				_warned_about_legacy_gpu.insert(gpu_name_str);
			}
		}
	} else {
		// it's a gpu
		gpuid = f->second;
	}
	return gpuid;
}


//
// For each specific variant set utilized, produce a list of exactly
// which gpus are mapped to it.  This removes any ambiguity in the
// variants' specification and still allows for human-readable use in
// most locations.
//
xml_element_node_t *Main::produce_gpu_variants_content(xml_element_node_t *parent)
{
	QXmlStreamAttributes sets_attrs;
	xml_element_node_t *gpu_sets_node = new xml_element_node_t("gpu_sets", sets_attrs, parent);

	QMap<vector<bool>, QString>::const_iterator gpu_set_i;
	for ( gpu_set_i = gpu_set_variant_map.begin(); gpu_set_i != gpu_set_variant_map.end(); 
		  gpu_set_i++ ) {
		QXmlStreamAttributes set_attrs;
		set_attrs.append("name", gpu_set_i.value());
		xml_element_node_t *gpu_set_node = new xml_element_node_t("gpu_set", set_attrs, gpu_sets_node);
		QSet<gpuid_t *> gpus = variants_to_gpus(gpu_set_i.value());
		if ( !gpus.size() ) {
			out() << "error: gpu set with zero size: " << gpu_set_i.value() << endl;
			//			out() << "error: gpu_set_i: " << to_string(gpu_set_i.key()).c_str() << endl;
			out() << "error: gpu_set_i: " << gpu_set_i.key() << endl;
		}
		for ( auto g : gpus ) {
			QXmlStreamAttributes gpu_attrs;
			gpu_attrs.append("name", g->name().c_str());
			xml_element_node_t *gpu_node = new xml_element_node_t("gpu", gpu_attrs, gpu_set_node);
			gpu_set_node->nodes.append(gpu_node);
		}
		gpu_sets_node->nodes.append(gpu_set_node);
	}
	return gpu_sets_node;
}


QString Main::gpus_to_variants(const gpu_set_t &gpus)
{
	vector<bool> e = enumerate_gpu_set(gpus);

	auto f = gpu_set_variant_map.find(e);
	if ( f != gpu_set_variant_map.end() ) {
		return *f;
	}

	QString var_gpus, delim;
	bool b = false;
	queue<int> q;
	for ( size_t l = 0; l < e.size(); l++ ) {
		if ( b ^ e[l] ) {
			q.push(l);
			b = !b;
		}
	}
	while ( !q.empty() ) {
		if ( q.size() > 1 ) {
			gpuid_t *lo = target_gpus_by_ordering[q.front()];   q.pop();
			gpuid_t *hi = target_gpus_by_ordering[q.front()-1]; q.pop(); // inclusive
			if ( lo != hi ) {
				var_gpus += QString("%1%2-%3").
					arg(delim).
					arg(QString::fromStdString(lo->name()).toUpper()).
					arg(QString::fromStdString(hi->name()).toUpper());
			} else {
				var_gpus += QString("%1%2").arg(delim).
					arg(QString::fromStdString(lo->name()).toUpper());
			}
			delim = " ";
		}
		else {
			gpuid_t *lo = target_gpus_by_ordering[q.front()]; q.pop();
			var_gpus += QString("%1%2-").
				arg(delim).
				arg(QString::fromStdString(lo->name()).toUpper());
		}
	}
	if ( gpu_set_variant_map.find(e) == gpu_set_variant_map.end() ) {
		gpu_set_variant_map[e] = var_gpus;
	}

	return var_gpus;
}

QSet<gpuid_t *> Main::variants_to_gpus(const QString &variants)
{
	// lookup cache.  frequently used, enough to worry about...
	auto f = variant_map.find(variants);
	if ( f != variant_map.end() ) {
		return *f;
	}
	// note: added trailing ':' to catch bogus case on "single"
	QRegularExpression single           ("^\\s*(\\w+)(:?)\\s*$");
	QRegularExpression range_inclusive  ("^\\s*(\\w+)\\s*-\\s*(\\w+)\\s*$");
	QRegularExpression range_exclusive  ("^\\s*(\\w+)\\s*:\\s*(\\w+)\\s*$");
	QRegularExpression before_exclusive ("^\\s*:\\s*(\\w+)\\s*$");
	QRegularExpression before_inclusive ("^\\s*-\\s*(\\w+)\\s*$");
	QRegularExpression after_inclusive  ("^\\s*(\\w+)-\\s*$");

	QSet<gpuid_t *> var_gpus;

	// target gpus is the set of all being gpus being considered.

	QStringList groups = variants.split(" ");
	QStringList pieces;
	for ( auto g: groups ) {
		pieces << g.split(",");
	}
	for (auto p : pieces ) {
		QRegularExpressionMatch m = single.match(p);
		if ( m.hasMatch() ) {
			QString var_gpu = m.captured(1);
			gpuid_t *gpu = target_gpu_by_name(var_gpu);
			if ( gpu ) {
				var_gpus.insert(gpu);
			}
			if ( m.captured(2).size() ) {
				out() << "warning: illegal variants spec(?) treated as single gpu [" <<
					variants << "]" << endl;
			}
			continue;
		}

		m = range_inclusive.match(p);
		if ( m.hasMatch() ) {
			//     var1-var2  all variants starting with var1 up to and including var2
			QString low  = m.captured(1);
			QString high = m.captured(2);
			gpuid_t *gpu_low  = target_gpu_by_name(low);
			gpuid_t *gpu_high = target_gpu_by_name(high);
			size_t  lowid = 0, highid = 0;
			if ( gpu_low && gpu_high ) {
				lowid = gpu_low->ordering_index();
				highid = gpu_high->ordering_index();
				for ( size_t gi = lowid; gi <= highid && (gi < target_gpus_by_ordering.size()); gi++ ) {
					var_gpus.insert(target_gpus_by_ordering[gi]);
				}
			} else {
				// not gpus
			}
			continue;
		}

		m = range_exclusive.match(p);
		if ( m.hasMatch() ) {
			//     var1:var2 all variants starting with var1 up to, but not including var2
			QString low  = m.captured(1);
			QString high = m.captured(2);
			gpuid_t *gpu_low  = target_gpu_by_name(low);
			gpuid_t *gpu_high = target_gpu_by_name(high);
			size_t lowid = 0, highid = 0;
			if ( gpu_low && gpu_high ) {
				lowid  = gpu_low->ordering_index();
				highid = gpu_high->ordering_index();
				for ( size_t gi = lowid; (gi < highid) && (gi < target_gpus_by_ordering.size()); gi++ ) {
					var_gpus.insert(target_gpus_by_ordering[gi]);
				}
			} else {
				// not gpus
			}
			continue;
		}

		m = before_inclusive.match(p);
		if ( m.hasMatch() ) {
			//     -var1 all variants up to and including var1
			QString high = m.captured(1);
			gpuid_t *gpu_high = target_gpu_by_name(high);
			size_t lowid = 0, highid = 0;
			if (gpu_high) {
				highid = gpu_high->ordering_index();
				for ( size_t gi = lowid; gi <= highid && (gi < target_gpus_by_ordering.size()); gi++ ) {
					var_gpus.insert(target_gpus_by_ordering[gi]);
				}
			} else {
				// not gpus
			}
			continue;
		}

		m = before_exclusive.match(p);
		if ( m.hasMatch() ) {
			//     :var1 all variants before var1
			QString high = m.captured(1);
			gpuid_t *gpu_high = target_gpu_by_name(high);
			size_t lowid = 0, highid = 0;
			if ( gpu_high ) {
				highid = gpu_high->ordering_index();
				for ( size_t gi = lowid; gi < highid && (gi < target_gpus_by_ordering.size()); gi++ ) {
					var_gpus.insert(target_gpus_by_ordering[gi]);
				}
			} else {
				// not gpus
			}
			continue;
		}

		m = after_inclusive.match(p);
		if ( m.hasMatch() ) {
			//     var1-  all variants starting from var1
			QString low = m.captured(1);
			gpuid_t *gpu_low = target_gpu_by_name(low);
			size_t lowid = 0, highid;
			if ( gpu_low ) {
				lowid = gpu_low->ordering_index();
				highid = target_gpus_by_ordering.size();
				for ( size_t gi = lowid; gi < highid && (gi < target_gpus_by_ordering.size()); gi++ ) {
					var_gpus.insert(target_gpus_by_ordering[gi]);
				}
			} else {
				// not gpus
			}
			continue;
		}
		// never matched.  
		out() << "error: failed to match variant piece " << p << " from " << variants << endl;
		out() << "error: in file " << _current_file.top().filePath();
		//XXX bug in the line # handling?  doesn't seem to line up.
		if ( _current_stream_reader ) {
			out() << " near line " << _current_stream_reader->lineNumber() << endl;
		} else {
			out() << endl;
		}
	}
	variant_map[variants] = var_gpus;
	return var_gpus;
}

vector<bool> enumerate_gpu_set(const QSet<gpuid_t *> &gpus)
{
	vector<bool> v(target_gpus_by_ordering.size(), false);
	for ( auto gpu : gpus ) {
		v[gpu->ordering_index()] = true;
	}
	return v;
}

vector<bool> enumerate_gpu_set(const set<gpuid_t *> &gpus)
{
	vector<bool> v(target_gpus_by_ordering.size(), false);
	for ( auto gpu : gpus ) {
		v[gpu->ordering_index()] = true;
	}
	return v;
}


// this operation doesn't necessarily need to be meaningful, just consistent.
// it's for use in the gpuid set -> variant string map.
bool operator < (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b)
{
	vector<bool> a_val, b_val;
	a_val = enumerate_gpu_set(a);
	b_val = enumerate_gpu_set(b);
	return a_val < b_val;
}

bool operator == (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b)
{
	vector<bool> a_val, b_val;
	a_val = enumerate_gpu_set(a);
	b_val = enumerate_gpu_set(b);
	return a_val == b_val;
}

bool operator > (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b)
{
	vector<bool> a_val, b_val;
	a_val = enumerate_gpu_set(a);
	b_val = enumerate_gpu_set(b);
	return a_val > b_val;
}


void Main::accelerate_defs()
{
	out() << "info: coalesced tree" << endl;
	out() << *def.coalesced << endl;

	// pre-compute some sets/groupings which are heavily used later.
	for ( auto reg_set : def.coalesced->regs_by_name ) {
		for ( auto reg : reg_set.second ) {
			def.regs_by_value_and_name[reg->val][reg->symbol].insert(reg);
		}
	}

	if ( _verbose ) {
		for ( auto reg_val_name_set : def.regs_by_value_and_name ) {
			for ( auto reg_val_set : reg_val_name_set.second ) {
				for ( auto reg_val : reg_val_set.second ) {
					reg_def_t *r = reg_val;
					out() << "info: val: 0x" << hex << r->val << " name: " <<
						r->symbol.c_str() << " gpus: " <<
						gpus_to_variants(r->def_gpus) << endl;
				}
			}
		}
	}
}

#if 0
    // this info is in the whitelist (at group)
	map<string, string> group_to_file_unit = {
		{"bus", "bus/pbus.xml"},
		{"ccsr", },
		{"ce2", },
		{"ctxsw_prog", },
		{"fb", "memory/g80_pfb.xml"},
		{"fifo", "fifo/pfifo.xml"},
		{"flush", },
		{"fuse", "bus/pfuse.xml"},
		{"gmmu", "memory/g80_vm.xml"},
		{"gr", "graph/g80_pgraph/pgraph.xml"},
		{"ltc", },
		{"mc", "bus/pmc.xml"},
		{"pbdma", },
		{"pri_ringmaster", },
		{"pri_ringstation_sys", },
		{"proj", },
		{"pwr", },
		{"ram", },
		{"therm", },
		{"timer", "bus/ptimer.xml"},
		{"top", },
		{"trim", }
	};

	ip_whitelist::group_t * pgraph;
	auto f_pgraph = ip_whitelist::chip_groups.equal_range("gr");
	for ( auto fi = f_pgraph.first; fi != f_pgraph.second; fi++ ) {
		ip_whitelist::group_t *group = fi->second;
		for ( auto ri : group->regs ) {
			ip_whitelist::reg_t *reg = ri.second;
			if ( intersected_defns.find(reg->def) != intersected_defns.end() ) {
				out() << "info: update " << qStr(reg->def) << endl;
			} else {
				out() << "info: add    " << qStr(reg->def) << endl;
			}
		}
	}
#endif



bool compare_fields(field_def_t *field_a, field_def_t *field_b)
{
	if ( field_a->low != field_b->low ) {
		return field_a->low < field_b->low;
	}
	// low_a == low_b
	if ( field_a->high != field_b->high ) {
		// low_a == low_b , high_a != high_b
		return field_a->high < field_b->high;
	}
	// how_a == low_b, high_a == high_b
	return field_a->symbol < field_b->symbol;
}

void Main::produce_const_content(const_def_t *const_def, xml_element_node_t *parent_node)
{
	QXmlStreamAttributes const_attrs;
	stringstream ss;
	ss << const_def->val;
	const_attrs.append( "value", ss.str().c_str());
	const_attrs.append( "name", const_def->symbol.c_str() );
	const_attrs.append( "variants", gpus_to_variants(const_def->def_gpus));
	xml_element_node_t *new_constant =
		new xml_element_node_t("value", const_attrs, parent_node);
	parent_node->nodes << new_constant;
}

bool compare_constants(const_def_t *const_a, const_def_t *const_b)
{
	if ( const_a->val != const_b->val )
		return const_a->val < const_b->val;
	return const_a->symbol < const_a->symbol;
}

void Main::produce_field_content(field_def_t *field, xml_element_node_t *parent_reg)
{
	QXmlStreamAttributes field_attrs;
	field_attrs.append("name", field->symbol.c_str());
	stringstream high; high << field->high;
	stringstream low; low  << field->low;
	if ( high.str() == low.str() ) {
		field_attrs.append("pos", low.str().c_str());
	} else {
		// natural to put high first.  but existing content has low, high.
		field_attrs.append("low", low.str().c_str());
		field_attrs.append("high", high.str().c_str());
	}
	field_attrs.append("variants", gpus_to_variants(field->def_gpus));

	xml_element_node_t *field_node = 
		new xml_element_node_t("bitfield", field_attrs, parent_reg);
	parent_reg->nodes << field_node;

	vector<const_def_t *> sorted_consts;
	for ( auto fc : field->constants_by_name ) {
		for ( auto c : fc.second ) {
			sorted_consts.push_back(c);
		}
	}
	sort(sorted_consts.begin(), sorted_consts.end(), compare_constants);
	for ( auto constant : sorted_consts ) {
		produce_const_content(constant, field_node);
	}
}


void Main::produce_register_content(reg_def_t *reg_def, file_content_t *content)
{
	content->nodes.append( produce_register_content(reg_def, (xml_element_node_t*)0)/*new_reg32*/ );
}

xml_element_node_t *Main::produce_register_content(reg_def_t *reg_def, xml_element_node_t *reg_node)
{
	QXmlStreamAttributes reg_attrs;
	stringstream ss;
	ss << "0x" << hex << reg_def->val;
	reg_attrs.append( "offset",   ss.str().c_str());
	reg_attrs.append( "name",     reg_def->symbol.c_str());
	reg_attrs.append( "bare",     "yes");
	reg_attrs.append( "variants", gpus_to_variants(reg_def->def_gpus));

	if ( !reg_node ) {
		reg_node = new xml_element_node_t("reg32", reg_attrs);
	} else {
		// update/modify pre-existing
		QXmlStreamAttributes &existing_attrs = reg_node->attrs;
		for ( int ai = 0; ai < existing_attrs.size(); ai++) {
			if (existing_attrs[ai].name() == "name") {
				existing_attrs[ai] = QXmlStreamAttribute("name", reg_def->symbol.c_str());
			} else if (existing_attrs[ai].name() == "bare") {
				existing_attrs[ai] = QXmlStreamAttribute("bare", "yes");
			} else if (existing_attrs[ai].name() == "variants") {
				existing_attrs[ai] = QXmlStreamAttribute("variants", gpus_to_variants(reg_def->def_gpus));
			}
		}
	}

	vector<field_def_t *> sorted_fields;
	for ( auto ff : reg_def->fields_by_name ) {
		for ( auto f : ff.second ) {
			sorted_fields.push_back(f);
		}
	}
	for ( auto field : sorted_fields ) {
		produce_field_content(field, reg_node);
	}

	return reg_node;

}

ostream &operator<<(ostream &os, const vector<bool> &e)
{
	string s(e.size(), '_');
	for ( size_t i = 0; i < e.size(); i++) {
		s[i] = e[i]?'1':'0';
	}
	os << s;
	return os;
}
