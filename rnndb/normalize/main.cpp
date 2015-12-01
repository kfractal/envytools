// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#include <QDebug>
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

Main::Main(int &argc, char **argv) : QCoreApplication(argc, argv), _validator(_schema)
{
	_root = "root.xml";
}

void Main::start()
{

	attr_spec_t defaults; // XXX: defaults not actually set yet...
	attr_stack.push(defaults); // also note the constructor trick screws up this push op (resets b's)

	int rc = read_root();

	if ( (_warn || _verbose) ) {
		for (auto e : _ignored_elements ) {
			qDebug() << "warning: ignored element" << e;
		}
		for (auto a : _ignored_attributes) {
			qDebug() << "warning: ignored attribute" << a;
		}
		for (auto ne: _nested_elements ) {
			qDebug() << "warning: unexpected nesting of" << ne << "element";
		}
	}

	exit(rc); // Main::exit() sets *intent* to exit w/rc.
	return;
}

int Main::read_root()
{
	_root_dir = QDir(QFileInfo(_root).dir());
	if (_verbose) qDebug() << "info: root dir is " << _root_dir.absolutePath();
	return cd_and_read(_root);
}

// returns to current at the end
int Main::cd_and_read(const QString &file_path)
{
	QDir  cur_dir = QDir::current();
	QDir  file_dir = QFileInfo(file_path).dir();
	QFile file(file_path);

	if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
		QFileInfo fi(file_path);
		bool retry_at_root = (fi.isRelative());
		bool ok = false;
		if ( retry_at_root ) {
			QDir::setCurrent(_root_dir.absolutePath());
			file.setFileName(file_path);
			if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
				ok = true;
				file_dir = _root_dir;
			}
		}
		if ( !ok ) {
			qDebug() << "error: couldn't open" << file_path;
			return -1;
		}
	}

	QDir::setCurrent( file_dir.path() );
	int r = read_file(&file);
	QDir::setCurrent( cur_dir.path() );
	return r;
}

int Main::read_file(QFile *file)
{
	// do not return just anywhere...
	// see "done:" label below
	_current_file.push(QFileInfo(*file));
	if ( _debug) qDebug() << "debug: read_file" << file->fileName();
	if (_validate_schema && _schema.isValid() ) {
		if (_validator.validate(file, QUrl::fromLocalFile(file->fileName())))
            qDebug() << "instance document is valid";
        else
            qDebug() << "instance document is invalid";
	}

	int rc = 0;
	QXmlStreamReader xml(file);

	// ...
	while (!xml.atEnd()) {
		auto token_type = xml.readNext();
		// ...  do (recursive descent style, graph building) processing
		
		switch (token_type) {
			case QXmlStreamReader::StartDocument:
				if ( _verbose ) qDebug() << "info: root start doc";
				break;
			case QXmlStreamReader::EndDocument:
				if ( _verbose ) qDebug() << "info: root end doc";
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
				if ( _verbose ) qDebug() << "info: root chars";
				break;
			case QXmlStreamReader::Comment:
				if ( _verbose ) qDebug() << "info: root comment";
				break;
			case QXmlStreamReader::DTD:
				if ( _verbose ) qDebug() << "info: root dtd";
				break;
			case QXmlStreamReader::EntityReference:
				if ( _verbose ) qDebug() << "info: root entity ref";
				break;
			case QXmlStreamReader::ProcessingInstruction:
				if ( _verbose ) qDebug() << "info: root processing instr";
				break;
			case QXmlStreamReader::NoToken:
				if ( _verbose ) qDebug() << "info: root no token";
				break;
			case QXmlStreamReader::Invalid:
			default:
				qDebug() << "error: root invalid/unk token";
				rc = -1;
				goto done;
				break; /*NOTREACHED*/
		}
	}
 done:
	if (xml.hasError()) {
		qDebug() << "error: xml has error?";
		rc = -1;
	}
	if (_debug) {
		qDebug()  << "debug: file complete" << _current_file.top().fileName();
	}
	_current_file.pop();
	return rc;
}

// misc junk for handlers
static void show_reg32_attr_spec(const attr_spec_t &a);
static bool reg_info(const QStack<attr_spec_t> &attr_stack, uint64_t &offset, QString &name);

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
	if ( ! _nestable_elements.contains(name_str(e)) ) {
		for ( auto si : _current_element ) {
			if ( si == name_str(e) ) {
				_nested_elements.insert(si);
				qDebug() << "warning: nested" << si << "at line" << 
					e.lineNumber() << "of" << _current_file.top().filePath();
			}
		}
	}
	_current_element.push(name_str(e));

	attr_spec_t t = attr_stack.top();
	t.zap_spec_bits(); // inherited values...
	attr_stack.push(t);

	int rc = 0;
	auto f = _element_handlers.find(name_str(e));
	if ( f == _element_handlers.end() ) {
		_ignored_elements.insert(name_str(e));
		rc = -1;
		goto done;
		//return -1;
	}

	rc = (this->**f)(e);

 done:
	if (rc) {
		qDebug() << "error: failed handler for" << name_str(e);
	}

	// don't pop. leave that for element closers
	return rc;
}

int Main::end_element(QXmlStreamReader &e)
{
	int rc;
	auto f = _element_closers.find(name_str(e));
	if ( f == _element_closers.end() ) {
		qDebug() << "failed to find closer for" << name_str(e);
		_ignored_elements.insert(name_str(e));
		rc = -1;
		goto done;
	}

	rc = (this->**f)(e);

	if (rc) {
		qDebug() << "error: failed closer for" << name_str(e);
	}

 done:
	if ( _current_element.size() == 0 ) {
		qDebug() << "error: about to pop empty element stack?";
	}
	_current_element.pop();


	if ( attr_stack.size() <= 1 ) {
		qDebug() << "error: about to hit empty attr stack?";
	}
	attr_stack.pop();
	return rc;
}

int Main::handle_import(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( a.name() == "file" ) {
			if ( _verbose ) qDebug() << "info: importing file" << a.value();
			rc = cd_and_read(a.value().toString());
			if ( rc )
				return rc;
		} else {
			ignored_attr(e,a);
		}
	}
	return rc;
}
int Main::close_import(QXmlStreamReader &e)
{
	return 0;
}

#undef ATTR
#undef ELEMENT
#define ATTR(X)    static QString X##_str { #X };
#define ELEMENT(E) namespace E ##_element { E ## _ELEMENT_ATTRS() };
ELEMENTS()



int Main::handle_array(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == array_element::length_str) {
			spec._length.from(a.value());
		} else if (name_str(a) == array_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == array_element::offset_str) {
			spec._offset.from(a.value());
		} else if (name_str(a) == array_element::stride_str) {
			spec._stride.from(a.value());
		} else if (name_str(a) == array_element::variants_str) {
			spec._variants.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}


	return rc;
}
int Main::close_array(QXmlStreamReader &e)
{
	return 0;
}



int Main::handle_domain(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == domain_element::bare_str) {
			spec._bare.from(a.value());
		} else if (name_str(a) == domain_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == domain_element::prefix_str) {
			spec._prefix.from(a.value());
		} else if (name_str(a) == domain_element::size_str) {
			spec._size.from(a.value());
		} else if (name_str(a) == domain_element::variants_str) {
			spec._variants.from(a.value());
		} else if (name_str(a) == domain_element::varset_str) {
			spec._varset.from(a.value());
		} else if (name_str(a) == domain_element::width_str) {
			spec._width.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_domain(QXmlStreamReader &e)
{
	return 0;
}


int Main::handle_doc(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
			ignored_attr(e,a);
	}
	return rc;
}
int Main::close_doc(QXmlStreamReader &e)
{
	return 0;
}


int Main::handle_spectype(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == spectype_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == spectype_element::type_str) {
			spec._type.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_spectype(QXmlStreamReader &e)
{
	return 0;
}


int Main::handle_b(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		ignored_attr(e,a);
	}
	return rc;
}
int Main::close_b(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_reg16(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == reg16_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == reg16_element::offset_str) {
			spec._offset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}

// note: all reg16_ELEMENT_ATTRS() *are* processed.
// (XXX but there's no reason we shouldn't allow all the other reg's
// attrs here as well... )
int Main::close_reg16(QXmlStreamReader &e)
{
	uint64_t offset;
	QString name;
	bool ok = reg_info(attr_stack, offset, name);

	if ( ok ) {
		qDebug() << QString("reg16: %1 0x%2").arg(name).
			arg(offset, 8 /*note: 32b still*/, 16, QChar('0'));
	}
	else {
		qDebug() << "error: bogus reg16 info? dumping attr stack...";
		dump_attr_stack();
		return -1;
	}
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
								if (_verbose) qDebug() << "info: loading db schema from" << file.fileName();
								_schema.load(&file, QUrl::fromLocalFile(file.fileName()));
								if ( !_schema.isValid()) {
									qDebug() << "warning: invalid schema" << sfi.absoluteFilePath();
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
int Main::close_database(QXmlStreamReader &e)
{
	return 0;
}


int Main::handle_bitset(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == bitset_element::inline_str) {
			spec._inline.from(a.value());
		} else if (name_str(a) == bitset_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == bitset_element::prefix_str) {
			spec._prefix.from(a.value());
		} else if (name_str(a) == bitset_element::variants_str) {
			spec._variants.from(a.value());
		} else if (name_str(a) == bitset_element::varset_str) {
			spec._varset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_bitset(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_copyright(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == copyright_element::year_str) {
			spec._year.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_copyright(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_reg8(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == reg8_element::access_str) {
			spec._access.from(a.value());
		} else if (name_str(a) == reg8_element::length_str) {
			spec._length.from(a.value());
		} else if (name_str(a) == reg8_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == reg8_element::offset_str) {
			spec._offset.from(a.value());
		} else if (name_str(a) == reg8_element::shr_str) {
			spec._shr.from(a.value());
		} else if (name_str(a) == reg8_element::type_str) {
			spec._type.from(a.value());
		} else if (name_str(a) == reg8_element::variants_str) {
			spec._variants.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}

// note: not all are reg8_ELEMENT_ATTRS() are processed yet.
int Main::close_reg8(QXmlStreamReader &e)
{
	uint64_t offset;
	QString name;
	bool ok = reg_info(attr_stack, offset, name);

	if ( ok ) {
		qDebug() << QString("reg8: %1 0x%2").arg(name).
			arg(offset, 8 /*note: 32b still*/, 16, QChar('0'));
	}
	else {
		qDebug() << "error: bogus reg8 info? dumping attr stack...";
		dump_attr_stack();
		return -1;
	}
	return 0;
}

int Main::handle_nick(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == nick_element::name_str) {
			spec._name.from(a.value());
		} else {
			ignored_attr(e,a);
		}

	}

	return rc;
}
int Main::close_nick(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_license(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		ignored_attr(e,a);
	}
	return rc;
}
int Main::close_license(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_enum(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == enum_element::bare_str) {
			spec._bare.from(a.value());
		} else if (name_str(a) == enum_element::inline_str) {
			spec._inline.from(a.value());
		} else if (name_str(a) == enum_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == enum_element::varset_str) {
			spec._varset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_enum(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_use_group(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == use_group_element::name_str) {
			spec._name.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_use_group(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_li(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		ignored_attr(e,a);
	}
	return rc;
}
int Main::close_li(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_reg64(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == reg64_element::length_str) {
			spec._length.from(a.value());
		} else if (name_str(a) == reg64_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == reg64_element::offset_str) {
			spec._offset.from(a.value());
		} else if (name_str(a) == reg64_element::shr_str) {
			spec._shr.from(a.value());
		} else if (name_str(a) == reg64_element::variants_str) {
			spec._variants.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}

// note: not all reg64_ELEMENT_ATTRS() are processed yet
int Main::close_reg64(QXmlStreamReader &e)
{
	uint64_t offset;
	QString name;
	bool ok = reg_info(attr_stack, offset, name);
	if ( ok ) {
		qDebug() << QString("reg64: %1 0x%2").arg(name).arg(offset, 16, 16, QChar('0'));
	}
	else {
		qDebug() << "error: bogus reg64 info? dumping attr stack...";
		dump_attr_stack();
		return -1;
	}
	return 0;
}



int Main::handle_reg32(QXmlStreamReader &e)
{
	int rc = 0;

	attr_spec_t &spec = attr_stack.top(); 

	for ( auto a: e.attributes() ) {
		if (name_str(a) == reg32_element::access_str) {
			spec._access.from(a.value());
		} else if (name_str(a) == reg32_element::align_str) {
			spec._align.from(a.value());
		} else if (name_str(a) == reg32_element::length_str) {
			spec._length.from(a.value());
		} else if (name_str(a) == reg32_element::max_str) {
			spec._max.from(a.value());
		} else if (name_str(a) == reg32_element::min_str) {
			spec._min.from(a.value());
		} else if (name_str(a) == reg32_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == reg32_element::offset_str) {
			spec._offset.from(a.value());
		} else if (name_str(a) == reg32_element::shr_str) {
			spec._shr.from(a.value());
		} else if (name_str(a) == reg32_element::stride_str) {
			spec._stride.from(a.value());
		} else if (name_str(a) == reg32_element::type_str) {
			spec._type.from(a.value());
		} else if (name_str(a) == reg32_element::variants_str) {
			spec._variants.from(a.value());
		} else if (name_str(a) == reg32_element::varset_str) {
			spec._varset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}

//
// note: not all reg32_ELEMENT_ATTRS() are processed yet.
//
int Main::close_reg32(QXmlStreamReader &e)
{
	uint64_t offset;
	QString name;
	bool ok = reg_info(attr_stack, offset, name);
	if ( ok ) {
		qDebug() << QString("reg32: %1 0x%2").arg(name).arg(offset, 8, 16, QChar('0'));
	}
	else {
		qDebug() << "error: bogus reg32 info? dumping attr stack...";
		dump_attr_stack();
		return -1;
	}
	return 0;
}

int Main::handle_brief(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		ignored_attr(e,a);
	}
	return rc;
}
int Main::close_brief(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_author(QXmlStreamReader &e)
{

	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if ( name_str(a) == author_element::email_str ) {
			spec._email.from(a.value());
		} else if ( name_str(a) == author_element::name_str ) {
			spec._name.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_author(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_ul(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		ignored_attr(e,a);
	}
	return rc;
}
int Main::close_ul(QXmlStreamReader &e)
{
	return 0;
}



int Main::handle_bitfield(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();
	// if we're handling a bitfield, does that mean we're always in a reg*?
	
	for ( auto a: e.attributes() ) {
		if ( name_str(a) == bitfield_element::add_str ) {
			spec._add.from(a.value());
		} else if (name_str(a) == bitfield_element::align_str ) {
			spec._align.from(a.value());
		} else if (name_str(a) == bitfield_element::high_str ) {
			spec._high.from(a.value());
		} else if (name_str(a) == bitfield_element::low_str ) {
			spec._low.from(a.value());
		} else if (name_str(a) == bitfield_element::max_str ) {
			spec._max.from(a.value());
		} else if (name_str(a) == bitfield_element::min_str ) {
			spec._min.from(a.value());
		} else if (name_str(a) == bitfield_element::name_str ) {
			spec._name.from(a.value());
		} else if (name_str(a) == bitfield_element::pos_str ) {
			spec._pos.from(a.value());
		} else if (name_str(a) == bitfield_element::radix_str ) {
			spec._radix.from(a.value());
		} else if (name_str(a) == bitfield_element::shr_str ) {
			spec._shr.from(a.value());
		} else if (name_str(a) == bitfield_element::type_str ) {
			spec._type.from(a.value());
		} else if (name_str(a) == bitfield_element::variants_str ) {
			spec._variants.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_bitfield(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_stripe(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == stripe_element::length_str) {
			spec._length.from(a.value());
		} else if (name_str(a) == stripe_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == stripe_element::offset_str) {
			spec._offset.from(a.value());
		} else if (name_str(a) == stripe_element::prefix_str) {
			spec._prefix.from(a.value());
		} else if (name_str(a) == stripe_element::stride_str) {
			spec._stride.from(a.value());
		} else if (name_str(a) == stripe_element::variants_str) {
			spec._variants.from(a.value());
		} else if (name_str(a) == stripe_element::varset_str) {
			spec._varset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_stripe(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_group(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == group_element::name_str) {
			spec._name.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_group(QXmlStreamReader &e)
{
	return 0;
}

int Main::handle_value(QXmlStreamReader &e)
{
	int rc = 0;
	attr_spec_t &spec = attr_stack.top();

	for ( auto a: e.attributes() ) {
		if (name_str(a) == value_element::name_str) {
			spec._name.from(a.value());
		} else if (name_str(a) == value_element::value_str) {
			spec._value.from(a.value());
		} else if (name_str(a) == value_element::variants_str) {
			spec._variants.from(a.value());
		} else if (name_str(a) == value_element::varset_str) {
			spec._varset.from(a.value());
		} else {
			ignored_attr(e,a);
		}
	}

	return rc;
}
int Main::close_value(QXmlStreamReader &e)
{
	return 0;
}

// e.g.: const std::vector<std::string> Main::array_element_attrs { "length", ... };
#undef ELEMENT
#undef ATTR
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
	int depth = attr_stack.length();
	for ( int a = 0; a < depth; a++) {
		const attr_spec_t &attr = attr_stack[a];
		show_reg32_attr_spec(attr);
	}
}


static bool reg_info(const QStack<attr_spec_t> &attr_stack, uint64_t &offset, QString &name)
{
	name = QString();
	offset = 0x0;
	int depth = attr_stack.length();
	bool offset_hit = false;
	QString delim;
	for ( int a = 0; a < depth; a++) {
		if ( attr_stack[a]._offset.b ) {
			offset_hit |= true;
			offset += attr_stack[a]._offset.n;
		}
		if ( attr_stack[a]._name.b) {
			name += delim + attr_stack[a]._name.s;
			delim = "::"; // XXX just for now...
		}
	}

	return offset_hit && name.length();
	if ( !(offset_hit  && name.length()) ) {
		return false;
	}
	return true;
}
