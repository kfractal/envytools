// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
/*
 *  >insert nvidia open source copyright<
 */

#include <vector>
#include <string>

#include <QCoreApplication>
#include <QFile>
#include <QDir>
#include <QSet>
#include <QString>
#include <QStack>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QAbstractMessageHandler>

typedef QSet<QString> QStringSet;

#include "ip_def.h"
#include "elements.h"

struct file_content_t {
	QList<xml_node_t *> nodes;
	QStack<xml_node_t *> current_node;
	QString _path;
	file_content_t () : _path(QString()) { }
	file_content_t (const QString &path) : _path(path) { }
};


class ValidatorMessageHandler : public QAbstractMessageHandler
{
	Q_OBJECT
public:
	ValidatorMessageHandler(QTextStream *o, QTextStream *e, QObject *p = 0) :
		QAbstractMessageHandler(p), _out(o), _err(e) { }

protected:
	QTextStream *_out;
	QTextStream *_err;
	virtual void handleMessage(QtMsgType type, const QString & description,
							   const QUrl & identifier, const QSourceLocation & sourceLocation);
};

class gpuid_t;
class Main : public QCoreApplication
{
	Q_OBJECT

public:
	Main(int & argc, char ** argv);

	QTextStream &out() { return _out; }
	QTextStream &in()  { return _in;  }
	QTextStream &err() { return _err; }

public Q_SLOTS:
	void start();

protected:
	QTextStream _in;
	QTextStream _out;
	QTextStream _err;

	QXmlSchema _schema;
	QXmlSchemaValidator _validator;
	ValidatorMessageHandler _validator_msg_handler;

	static constexpr bool _debug   = false;
	static constexpr bool _verbose = false;
	static constexpr bool _warn    = true;
	static constexpr bool _validate_schema = true;
	static constexpr bool _skip_multiple_file_scans = true;
	static constexpr bool _emit_normalized_hierarchy = true;

	bool _error;
	QString _root;
	QDir _root_dir;

	int read_root();
	int read_path(const QString &);
	int read_file(QFile *);
	QStack<QFileInfo> _current_file; // for error reporting

	int handle_element(QXmlStreamReader &e);
	int end_element(QXmlStreamReader &e);
	QStack<QString> _current_element;
	bool in_bitfield();

	QSet<QString> _ignored_attributes;
	QSet<QString> _ignored_elements;
	inline void ignored_attr(QXmlStreamReader &E, QXmlStreamAttribute &A) {
		_ignored_attributes.insert(QString("%1.%2").
			arg(E.name().toString()).arg(A.name().toString()));
	}

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

	QStack<attr_spec_t> _attr_stack;
	attr_spec_t _attrs;
	void flatten_attrs(const attr_spec_t &);

	void dump_attr_stack();

	QSet<QString> _all_xml_files;
	QSet<QString> _read_files;
	QSet<QString> _ignored_xml_files;
	void find_xml_files();

	file_content_t * current_file_content();
	QMap<QString, file_content_t> _xml_file_content;
	void emit_file(const QString &file);

	int handle_document(QXmlStreamReader &e);
	int end_document(QXmlStreamReader &e);

	int handle_chars(QXmlStreamReader &e);
	int handle_comment(QXmlStreamReader &e);
	int handle_dtd(QXmlStreamReader &e);
	int handle_entity_reference(QXmlStreamReader &e);
	int handle_processing_instruction(QXmlStreamReader &e);
	int handle_no_token(QXmlStreamReader &e);

	QSet<gpuid_t *>variants_to_gpus(const QString &);
	QString gpus_to_variants(gpu_set_t &);

	QMap<QString, QSet<gpuid_t *>> variant_map;
	QMap<uint64_t, QString> gpu_set_variant_map;

	void update_defs();

};

uint64_t enumerate_gpu_set(const QSet<gpuid_t*> &gpus);
uint64_t enumerate_gpu_set(const set<gpuid_t*> &gpus);
bool operator < (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b);
bool operator == (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b);
bool operator > (const QSet<gpuid_t *> &a, const QSet<gpuid_t *>&b);

