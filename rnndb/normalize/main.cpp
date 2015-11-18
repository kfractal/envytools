// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-

#include <QDebug>
#include <QTimer>

#include "main.h"

int main (int argc, char **argv)
{
	Main app(argc, argv);
	QTimer::singleShot(0, &app, SLOT(start()));
	return app.exec();
}

Main::Main(int &argc, char **argv) : QCoreApplication(argc, argv)
{
	_root = "../root.xml";
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
	int r = read_file(file);
	QDir::setCurrent( cur_dir.path() );
	return r;
}

int Main::read_file(QFile &file)
{
	int rc = 0;
	QXmlStreamReader xml(&file);

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

int Main::handle_import(QXmlStreamReader &e)
{
	int rc = 0;
	for ( auto a: e.attributes() ) {
		// if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		if ( a.name() == "file" ) {
			if ( _verbose ) qDebug() << "info: import" << a.value();
			rc = cd_and_read(a.value().toString());
			if ( rc )
				return rc;
		} else {
			// if (_warn) qDebug() << "warning: ignored" << a.name() << "attribute";
			_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
			rc = -1;
		}
	}
	return rc;
}

int Main::handle_array(QXmlStreamReader &e)
{
	int rc = 0;
	if (_verbose) qDebug() << "array element";
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
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
	int rc = 0;
	for ( auto a: e.attributes() ) {
		if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
		_ignored_attributes.insert(QString("%1.%2").arg(e.name().toString()).arg(a.name().toString()));
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

#if 0
warning: ignored attribute "array.length"
warning: ignored attribute "array.name"
warning: ignored attribute "array.offset"
warning: ignored attribute "array.stride"
warning: ignored attribute "array.variants"
warning: ignored attribute "author.email"
warning: ignored attribute "author.name"
warning: ignored attribute "bitfield.add"
warning: ignored attribute "bitfield.align"
warning: ignored attribute "bitfield.high"
warning: ignored attribute "bitfield.low"
warning: ignored attribute "bitfield.max"
warning: ignored attribute "bitfield.min"
warning: ignored attribute "bitfield.name"
warning: ignored attribute "bitfield.pos"
warning: ignored attribute "bitfield.radix"
warning: ignored attribute "bitfield.shr"
warning: ignored attribute "bitfield.type"
warning: ignored attribute "bitfield.variants"
warning: ignored attribute "bitset.inline"
warning: ignored attribute "bitset.name"
warning: ignored attribute "bitset.prefix"
warning: ignored attribute "bitset.variants"
warning: ignored attribute "bitset.varset"
warning: ignored attribute "copyright.year"
warning: ignored attribute "database.schemaLocation"
warning: ignored attribute "domain.bare"
warning: ignored attribute "domain.name"
warning: ignored attribute "domain.prefix"
warning: ignored attribute "domain.size"
warning: ignored attribute "domain.variants"
warning: ignored attribute "domain.varset"
warning: ignored attribute "domain.width"
warning: ignored attribute "enum.bare"
warning: ignored attribute "enum.inline"
warning: ignored attribute "enum.name"
warning: ignored attribute "enum.varset"
warning: ignored attribute "group.name"
warning: ignored attribute "nick.name"
warning: ignored attribute "reg16.name"
warning: ignored attribute "reg16.offset"
warning: ignored attribute "reg32.access"
warning: ignored attribute "reg32.align"
warning: ignored attribute "reg32.length"
warning: ignored attribute "reg32.max"
warning: ignored attribute "reg32.min"
warning: ignored attribute "reg32.name"
warning: ignored attribute "reg32.offset"
warning: ignored attribute "reg32.shr"
warning: ignored attribute "reg32.stride"
warning: ignored attribute "reg32.type"
warning: ignored attribute "reg32.variants"
warning: ignored attribute "reg32.varset"
warning: ignored attribute "reg64.length"
warning: ignored attribute "reg64.name"
warning: ignored attribute "reg64.offset"
warning: ignored attribute "reg64.shr"
warning: ignored attribute "reg64.variants"
warning: ignored attribute "reg8.access"
warning: ignored attribute "reg8.length"
warning: ignored attribute "reg8.name"
warning: ignored attribute "reg8.offset"
warning: ignored attribute "reg8.shr"
warning: ignored attribute "reg8.type"
warning: ignored attribute "reg8.variants"
warning: ignored attribute "spectype.name"
warning: ignored attribute "spectype.type"
warning: ignored attribute "stripe.length"
warning: ignored attribute "stripe.name"
warning: ignored attribute "stripe.offset"
warning: ignored attribute "stripe.prefix"
warning: ignored attribute "stripe.stride"
warning: ignored attribute "stripe.variants"
warning: ignored attribute "stripe.varset"
warning: ignored attribute "use-group.name"
warning: ignored attribute "value.name"
warning: ignored attribute "value.value"
warning: ignored attribute "value.variants"
warning: ignored attribute "value.varset"
#endif
