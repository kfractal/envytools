// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-

#include <vector>
#include <string>

#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSet>
#include <QString>
#include <QStack>
#include <QXmlStreamReader>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

typedef QSet<QString> QStringSet;

#include "elements.h"

class Main : public QCoreApplication
{
	Q_OBJECT

public:
	Main(int & argc, char ** argv);

public slots:
	void start();

protected:

	QXmlSchema _schema;
	QXmlSchemaValidator _validator;

	static constexpr bool _debug   = false;
	static constexpr bool _verbose = false;
	static constexpr bool _warn    = true;
	static constexpr bool _validate_schema = false;

	bool _error;
	QString _root;
	QDir _root_dir;

	int read_root();
	int cd_and_read(const QString &);
	int read_file(QFile *);

	int handle_element(QXmlStreamReader &e);
	void end_element(QXmlStreamReader &e);
	QStack<QString> _current_element;

	QSet<QString> _ignored_attributes;
	QSet<QString> _ignored_elements;

	typedef int (Main::*element_handler_t)(QXmlStreamReader &);
	static QMap<QString, element_handler_t> _element_handlers;


	// element handler prototypes
#define ELEMENT(X)	int handle_ ## X (QXmlStreamReader &);
	ELEMENTS()
#undef ELEMENT


	// element attribute sets
	// e.g.: array_element_attrs[...]
#define ELEMENT(X) static const std::vector<std::string> X ##_element_attrs;
#define ATTR(X) #X,
	ELEMENTS()
#undef ATTR
#undef ELEMENT


};
