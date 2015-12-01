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
	QStack<QFileInfo> _current_file; // for error reporting

	int handle_element(QXmlStreamReader &e);
	int end_element(QXmlStreamReader &e);
	QStack<QString> _current_element;

	QSet<QString> _ignored_attributes;
	QSet<QString> _ignored_elements;

	typedef int (Main::*element_handler_t)(QXmlStreamReader &);
	static QMap<QString, element_handler_t> _element_handlers;
	static QMap<QString, element_handler_t> _element_closers;


	// element handler/closer prototypes
#define ELEMENT(X)	int handle_ ## X (QXmlStreamReader &);
	ELEMENTS()
#undef ELEMENT
#define ELEMENT(X)	int close_ ## X (QXmlStreamReader &);
	ELEMENTS()
#undef ELEMENT


	// element attribute sets
	// e.g.: array_element_attrs[...]
#define ELEMENT(X) static const std::vector<std::string> X ##_element_attrs;
#define ATTR(X) #X,
	ELEMENTS()
#undef ATTR
#undef ELEMENT

	// parsing metrics, etc
	static QSet<QString> _nestable_elements; // ok to nest
  	       QSet<QString> _nested_elements;   // unexpected

	QStack<attr_spec_t> attr_stack;

	void dump_attr_stack();

	QSet<QString> _all_xml_files;
	QSet<QString> _read_files;
	QSet<QString> _ignored_xml_files;
	void find_xml_files();
};

//
// misc convenience/de-clutter goo
//
#define ignored_attr(E, A) do {							\
		_ignored_attributes.insert(QString("%1.%2").	\
			arg(name_str(E)).arg(name_str(A)));			\
	}													\
	while (0)
