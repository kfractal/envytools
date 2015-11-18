// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-

#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QXmlStreamReader>

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




				if ( xml.name() == "import" ) {
					for (auto a: xml.attributes()) {
						// if ( _verbose ) qDebug() << "info:\t" << a.name() << a.value();
						if ( a.name() == "file" ) {
							if ( _verbose ) qDebug() << "info: import" << a.value();
							rc = cd_and_read(a.value().toString());
							if ( rc )
								return rc;
						} else {
							// if (_warn) qDebug() << "warning: ignored" << a.name() << "attribute";
							_ignored_attributes.insert(QString("%1.%2").arg(xml.name().toString()).arg(a.name().toString()));
						}
					}
				} else {
					// if (_warn) qDebug() << "warning: ignored" << xml.name() << "element";
					_ignored_elements.insert(xml.name().toString());
				}
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

int Main::handle_element(QString e)
{
	/*QMap<QString, element_handler_t>::const_iterator */ auto f =
		_element_handlers.find(e);
	if ( f == _element_handlers.end() ) {
		return -1;
	}
    #define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))
	return (this->**f)();
}

int Main::handle_array()
{
	int rc = 0;
	return rc;
}

int Main::handle_domain()
{
	int rc = 0;
	return rc;
}

int Main::handle_doc()
{
	int rc = 0;
	return rc;
}

int Main::handle_spectype()
{
	int rc = 0;
	return rc;
}

int Main::handle_b()
{
	int rc = 0;
	return rc;
}

int Main::handle_reg16()
{
	int rc = 0;
	return rc;
}

int Main::handle_database()
{
	int rc = 0;
	return rc;
}
int Main::handle_bitset()
{
	int rc = 0;
	return rc;
}
int Main::handle_copyright()
{
	int rc = 0;
	return rc;
}

int Main::handle_reg8()
{
	int rc = 0;
	return rc;
}

int Main::handle_nick()
{
	int rc = 0;
	return rc;
}

int Main::handle_license()
{
	int rc = 0;
	return rc;
}

int Main::handle_enum()
{
	int rc = 0;
	return rc;
}

int Main::handle_use_group()
{
	int rc = 0;
	return rc;
}

int Main::handle_li()
{
	int rc = 0;
	return rc;
}

int Main::handle_reg64()
{
	int rc = 0;
	return rc;
}

int Main::handle_reg32()
{
	int rc = 0;
	return rc;
}

int Main::handle_brief()
{
	int rc = 0;
	return rc;
}

int Main::handle_author()
{
	int rc = 0;
	return rc;
}

int Main::handle_ul()
{
	int rc = 0;
	return rc;
}

int Main::handle_bitfield()
{
	int rc = 0;
	return rc;
}

int Main::handle_stripe()
{
	int rc = 0;
	return rc;
}

int Main::handle_group()
{
	int rc = 0;
	return rc;
}

int Main::handle_value()
{
	int rc = 0;
	return rc;
}
