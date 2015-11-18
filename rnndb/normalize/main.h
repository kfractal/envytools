// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSet>
#include <QXmlStreamReader>

//
// valid XML elements
// note the "use-group" element is handled differentely as needed since it
// doesn't form a nice token
//
#define ELEMENTS()			\
	ELEMENT(import)			\
	ELEMENT(group)			\
	ELEMENT(value)			\
	ELEMENT(array)			\
	ELEMENT(domain)			\
	ELEMENT(doc)			\
	ELEMENT(spectype)		\
	ELEMENT(b)				\
	ELEMENT(reg16)			\
	ELEMENT(database)		\
	ELEMENT(bitset)			\
	ELEMENT(copyright)		\
	ELEMENT(reg8)			\
	ELEMENT(nick)			\
	ELEMENT(license)		\
	ELEMENT(enum)			\
	ELEMENT(li)				\
	ELEMENT(reg64)			\
	ELEMENT(reg32)			\
	ELEMENT(brief)			\
	ELEMENT(author)			\
	ELEMENT(ul)				\
	ELEMENT(bitfield)		\
	ELEMENT(stripe)			\
	ELEMENT(use_group)		\
	/*ELEMENT(use-group)*/    


class Main : public QCoreApplication
{
	Q_OBJECT

public:
	Main(int & argc, char ** argv);

public slots:
	void start();

protected:
	static constexpr bool _debug   = false;
	static constexpr bool _verbose = false;
	static constexpr bool _warn    = true;

	bool _error;
	QString _root;
	QDir _root_dir;

	int read_root();
	int cd_and_read(const QString &);
	int read_file(QFile &);

	QSet<QString> _ignored_attributes;
	QSet<QString> _ignored_elements;

	typedef int (Main::*element_handler_t)(QXmlStreamReader &);
	static QMap<QString, element_handler_t> _element_handlers;
	int handle_element(QXmlStreamReader &e);

	// element handler prototypes (lots)
#define ELEMENT(X)	int handle_ ## X (QXmlStreamReader &);
	ELEMENTS()
#undef ELEMENT
};
