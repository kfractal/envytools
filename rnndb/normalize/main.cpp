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
	if ( _debug) qDebug() << "debug:" << __func__;
	int rc = read_root();

	if ( (_warn || _verbose) &&
		 (_ignored_elements.size() || _ignored_attributes.size() ) ) {
		for (auto e : _ignored_elements ) {
			qDebug() << "warning: ignored element" << e;
		}
		for (auto a : _ignored_attributes) {
			qDebug() << "warning: ignored attribute" << a;
		}
	}

	exit(rc); // Main::exit() sets *intent* to exit w/rc.
	return;
}

int Main::read_root()
{
	_root_dir = QDir(QFileInfo(_root).dir());
	// qDebug() << "root dir is " << _root_dir.absolutePath();
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
		if ( _verbose ) qDebug() << "info: couldn't open" << file_path << ", path is" << fi.path();
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
				if ( _verbose ) qDebug() << "info: root start elem" << xml.name();
				rc = handle_element(xml);
				if (rc)
					return rc;
				break;
			case QXmlStreamReader::EndElement:
				if ( _verbose ) qDebug() << "info: root end elem" << xml.name();
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
				return -1;
				break; /*NOTREACHED*/
		}
	}
	if (xml.hasError()) {
		qDebug() << "error: xml has error?";
		return -1;
	}
	return rc;
}


//
// handlers for specific elements and attributes etc are handled below.
// note that the xml schema validator while in place isn't working properly
// yet (not sure whether that's due to faults in the xml, or what, though
// some were fixed already).
//
// there shouldn't be too much in the way of error handling going on here
// re: illegal elements, illegal attribute names.  assume the validator is handling
// those.  but, out of bounds and other sorts of things can and should be managed
// here as usual (whatever is out of scope for the xml schema validator).
//

#define ELEMENT(X) { #X , &Main::handle_ ## X },

QMap<QString, Main::element_handler_t> Main::_element_handlers {
	ELEMENTS()
	{ "use-group", &Main::handle_use_group },
};
#undef ELEMENT


int Main::handle_element(QXmlStreamReader &e)
{
	auto f = _element_handlers.find(e.name().toString());
	if ( f == _element_handlers.end() ) {
		_ignored_elements.insert(e.name().toString());
		return -1;
	}
	return (this->**f)(e);
}


#define check_attrs(X) do {											\
		if (X ## _element_valid_attrs.end() == X ##_element_valid_attrs.find(a.name().toString())) { \
			qDebug() << "warning: unexpected attr" << a.name().toString(); \
		}																\
	} while (0)


int Main::handle_import(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		check_attrs(import);
		if ( a.name() == "file" ) {
			if ( _verbose ) qDebug() << "info: importing file" << a.value();
			rc = cd_and_read(a.value().toString());
			if ( rc )
				return rc;
		} else {
			_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
			rc = -1;
		}
	}
	return rc;
}



int Main::handle_array(QXmlStreamReader &e)
{
	static std::map<std::string, std::size_t> attr_index {
		{ "length",   0 },
		{ "name",     1 }, 
		{ "offset" ,  2 },
		{ "stride",   3 },
		{ "variants", 4 },
	};

	int rc = 0;
	if (_verbose) qDebug() << "array element";
	for ( auto a: e.attributes() ) {
		check_attrs(array);
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}

	return rc;
}

int Main::handle_domain(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_doc(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_spectype(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_b(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_reg16(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
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
			if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
			_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
		}
	}
	return rc;
}


int Main::handle_bitset(QXmlStreamReader &e)
{

	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}
int Main::handle_copyright(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_reg8(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_nick(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_license(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_enum(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_use_group(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_li(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_reg64(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_reg32(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_brief(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_author(QXmlStreamReader &e)
{

	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_ul(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}


int Main::handle_bitfield(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_stripe(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_group(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

int Main::handle_value(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
	}
	return rc;
}

const QStringSet Main::array_element_valid_attrs     { "length", "name", "offset", "stride", "variants" };
const QStringSet Main::author_element_valid_attrs    { "email", "name" };
const QStringSet Main::bitfield_element_valid_attrs  { "add", "align", "high", "low", "max", "min", "name", "pos", "radix", "shr", "type", "variants" };
const QStringSet Main::bitset_element_valid_attrs    { "inline", "name", "prefix", "variants", "varset" };
const QStringSet Main::copyright_element_valid_attrs { "year" };
const QStringSet Main::database_element_valid_attrs  { "schemaLocation" };
const QStringSet Main::domain_element_valid_attrs    { "bare", "name", "prefix", "size", "variants", "varset", "width" };
const QStringSet Main::enum_element_valid_attrs      { "bare","inline", "name", "varset" };
const QStringSet Main::group_element_valid_attrs     { "name" };
const QStringSet Main::import_element_valid_attrs    { "file" };
const QStringSet Main::nick_element_valid_attrs      { "name" };
const QStringSet Main::reg16_element_valid_attrs     { "name", "offset" };
const QStringSet Main::reg32_element_valid_attrs     { "access", "align", "length", "max", "min", "name", "offset", "shr", "stride", "type", "variants", "varset" };
const QStringSet Main::reg64_element_valid_attrs     { "length", "name", "offset", "shr", "variants" };
const QStringSet Main::reg8_element_valid_attrs      { "access", "length", "name", "offset", "shr", "type", "variants" };
const QStringSet Main::spectype_element_valid_attrs  { "name","type" };
const QStringSet Main::stripe_element_valid_attrs    { "length", "name", "offset", "prefix", "stride", "variants", "varset" };
const QStringSet Main::use_group_element_valid_attrs { "name" };
const QStringSet Main::value_element_valid_attrs     { "name", "value", "variants", "varset" };

