// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-

#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QXmlStreamReader>

#include "main.h"

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
							if (_warn) qDebug() << "warning: ignored" << xml.name() << "attribute";
						}
					}
				} else {
					if (_warn) qDebug() << "warning: ignored" << xml.name() << "element";
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
	exit(rc); // Main::exit() sets *intent* to exit w/rc.
	return;
}
