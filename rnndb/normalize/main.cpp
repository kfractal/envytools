// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-

#include <QDebug>
#include <QQueue>
#include <QTimer>
#include <QXmlSchema>
#include <QRegularExpression>

#include "main.h"

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
}

void Main::start()
{
	// XXX: defaults not actually set yet...
	// should almost certainly involve the schema.
	attr_spec_t defaults;
	defaults._offset.v = 0; // int doesn't have default constructor

	defaults.zap_spec_bits(); // mark as inherited values...
	_attr_stack.push(defaults);

	int rc = read_root();

	if ( (_warn || _verbose) ) {
		for (auto e : _ignored_elements ) {
			out() << "warning: ignored element " << e << endl;
		}
		for (auto a : _ignored_attributes) {
			out() << "warning: ignored attribute " << a << endl;
		}
		for (auto ne: _nested_elements ) {
			out() << "warning: unexpected nesting of " << ne << " element" << endl;
		}
		if ( _verbose)  {
			for ( auto rf : _read_files ) {
				out() << "info: read " << rf << endl;
			}
		}
		if ( !rc ) { // don't bother if there was an error...
			for ( auto xf : _all_xml_files ) {
				if ( _read_files.find(xf) == _read_files.end() ) {
					out() << "warning: didn't use " << xf << " ?" << endl;
				}
			}
		}
	}

	exit(rc); // Main::exit() sets *intent* to exit w/rc.
	return;
}

int Main::read_root()
{
	_root_dir = QDir(QFileInfo(_root).dir());
	if (_verbose) out() << "info: root dir is " << _root_dir.absolutePath() << endl;

	// this is used to check for files which might be improperly ignored.
	// i.e.: it's not used to drive the hierarchy traversal...
	find_xml_files();

	int rc = cd_and_read(_root);

	return rc;
}

// returns to current at the end
int Main::cd_and_read(const QString &file_path)
{
	QFileInfo file_info(file_path);
	QString file_name = file_info.fileName();

	out() << "info: file path " << file_path << endl;// << (relative?"(relative)":"") << endl;

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

	// ...
	while (!xml.atEnd()) {
		auto token_type = xml.readNext();
		// ...  do (recursive descent style, graph building) processing

		switch (token_type) {
			case QXmlStreamReader::StartDocument:
				if ( _verbose ) out() << "info: root start doc" << endl;
				break;
			case QXmlStreamReader::EndDocument:
				if ( _verbose ) out() << "info: root end doc" << endl;
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
				if ( _verbose ) out() << "info: root chars" << endl;
				break;
			case QXmlStreamReader::Comment:
				if ( _verbose ) out() << "info: root comment" << endl;
				break;
			case QXmlStreamReader::DTD:
				if ( _verbose ) out() << "info: root dtd" << endl;
				break;
			case QXmlStreamReader::EntityReference:
				if ( _verbose ) out() << "info: root entity ref" << endl;
				break;
			case QXmlStreamReader::ProcessingInstruction:
				if ( _verbose ) out() << "info: root processing instr" << endl;
				break;
			case QXmlStreamReader::NoToken:
				if ( _verbose ) out() << "info: root no token" << endl;
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
	if (true || _debug) {
		out()  << "debug: file complete " << _current_file.top().fileName() << endl;
	}
	_current_file.pop();
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

//
// i'm not certain what flavors of validation are
// possible with the schema validator yet.  but i'm curious
// to know here if we see any nesting of elements.
// for most of them it makes no sense...
//
// hmm... more of these are nesting than i expected.
// bitfield, stripe, array, etc.  it seems they are
// are implementing inherited attributes more
// than anything.  so treating them that way for now.
//
QSet<QString> Main::_nestable_elements {
	"array",
	"bitfield",
	"domain",
	"database",
	"import",
	"stripe",
};

int Main::handle_element(QXmlStreamReader &e)
{
	auto f = _element_handlers.find(name_str(e));
	if ( f == _element_handlers.end() ) {
		_ignored_elements.insert(name_str(e));
		out() << "error: unknown element " << name_str(e) << " at line " <<
			e.lineNumber() << " of " << _current_file.top().filePath() << endl;
		return -1;
	}

	if ( ! _nestable_elements.contains(name_str(e)) ) {
		for ( auto si : _current_element ) {
			if ( si == name_str(e) ) {
				_nested_elements.insert(si);
				out() << "warning: nested " << si << " at line " <<
					e.lineNumber() << " of " << _current_file.top().filePath() << endl;
			}
		}
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
			rc = cd_and_read(a.value().toString());
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
	out() << QString("array: %1 0x%2").arg(_attrs._name.v).
		arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0')) << endl;

	return rc;
}

int Main::close_array(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_domain(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(domain);
	out() << QString("domain: %1").arg(_attrs._name.v) << endl;

	return rc;
}

int Main::close_domain(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_doc(QXmlStreamReader &e)
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
	STACK_ELEMENT_ATTRS(reg16);

	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;

	out() << QString("reg16: %1 0x%2").arg(name).
		arg(offset, 8 /*note: 32b still*/, 16, QChar('0')) << endl;

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
	STACK_ELEMENT_ATTRS(reg8);

	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;

	out() << QString("reg8: %1 0x%2").arg(name).
		arg(offset, 8 /*note: 32b still*/, 16, QChar('0')) << endl;

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
	STACK_ELEMENT_ATTRS(reg64);

	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;

	out() << QString("reg64: %1 0x%2").arg(name).
		arg(offset, 16, 16, QChar('0')) << endl;

	return rc;
}

int Main::close_reg64(QXmlStreamReader &)
{
	return 0;
}

// note: work in progress...
int Main::handle_reg32(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(reg32);

	uint64_t offset = _attrs._offset.v;
	QString name    = _attrs._name.v;

	out() << QString("reg32: %1 0x%2").arg(name).
		arg(offset, 8, 16, QChar('0')) << endl;

	return rc;
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

	out() << QString("bitfield: %1 %2:%3").arg(_attrs._name.v).
		arg(_attrs._high.v).arg(_attrs._low.v) << endl;

	return rc;
}

int Main::close_bitfield(QXmlStreamReader &)
{
	return 0;
}

int Main::handle_stripe(QXmlStreamReader &e)
{
	STACK_ELEMENT_ATTRS(stripe);
	out() << QString("stripe: %1 0x%2").arg(_attrs._name.v).
		arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0')) << endl;

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
	QString c_str; if ( _attrs._offset.b ) {
		c_str = QString(" 0x%1").arg(_attrs._offset.v, 8 /*note: 32b still*/, 16, QChar('0'));
	}
	if ( _attrs._high.b && _attrs._low.b ) { // recall just giving pos -> defines these
		c_str += QString(" %1:%2").arg(_attrs._high.v).arg(_attrs._low.v);
	}
	if ( in_bitfield() ) {
		out() << QString("bitfield value: %1 %2%3").
			arg(_attrs._name.v).arg(_attrs._value.v).arg(c_str) << endl;
	} else {
		out() << QString("value: %1 %2").
			arg(_attrs._name.v).arg(_attrs._value.v) << endl;
	}

	return rc;
}

int Main::close_value(QXmlStreamReader &)
{
	return 0;
}
#undef ATTR

// e.g.: const std::vector<std::string> Main::array_element_attrs { "length", ... };
#define ELEMENT(X) const std::vector<std::string> Main:: X ##_element_attrs { X ## _ELEMENT_ATTRS() };
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
	out () << "info: the number of xml files is " << _all_xml_files.size() << endl;
	for ( auto v: _all_xml_files ) {
		out() << v << endl;
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
