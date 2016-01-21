// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#include <QDebug>

//
// valid XML elements
//
#define ELEMENTS()								\
	ELEMENT(array)								\
	ELEMENT(author)								\
	ELEMENT(b)									\
	ELEMENT(bitfield)							\
	ELEMENT(bitset)								\
	ELEMENT(brief)								\
	ELEMENT(copyright)							\
	ELEMENT(database)							\
	ELEMENT(doc)								\
	ELEMENT(domain)								\
	ELEMENT(enum)								\
	ELEMENT(group)								\
	ELEMENT(import)								\
	ELEMENT(li)									\
	ELEMENT(license)							\
	ELEMENT(nick)								\
	ELEMENT(reg16)								\
	ELEMENT(reg32)								\
	ELEMENT(reg64)								\
	ELEMENT(reg8)								\
	ELEMENT(spectype)							\
	ELEMENT(stripe)								\
	ELEMENT(ul)									\
	ELEMENT(use_group)							\
	ELEMENT(value)
// note: the "use-group" element is handled separately since it isn't a token


#define array_ELEMENT_ATTRS()		ATTR(length) ATTR(name) ATTR(offset) ATTR(stride) ATTR(variants)
#define author_ELEMENT_ATTRS()		ATTR(email) ATTR(name)
#define b_ELEMENT_ATTRS()
#define bitfield_ELEMENT_ATTRS()	ATTR(add) ATTR(align) ATTR(high) ATTR(low) ATTR(max) ATTR(min) \
									ATTR(name) ATTR(pos) ATTR(radix) ATTR(shr)	ATTR(type) ATTR(variants)
#define bitset_ELEMENT_ATTRS()		ATTR(inline) ATTR(name) ATTR(prefix) ATTR(variants) ATTR(varset)
#define brief_ELEMENT_ATTRS()
#define copyright_ELEMENT_ATTRS()	ATTR(year)
#define database_ELEMENT_ATTRS()	ATTR(schemaLocation)
#define doc_ELEMENT_ATTRS()
#define domain_ELEMENT_ATTRS()		ATTR(bare) ATTR(name) ATTR(prefix) ATTR(size) ATTR(variants) \
									ATTR(varset) ATTR(width)
#define enum_ELEMENT_ATTRS()		ATTR(bare) ATTR(inline) ATTR(name) ATTR(prefix) ATTR(varset)
#define group_ELEMENT_ATTRS()		ATTR(name)
#define import_ELEMENT_ATTRS()		ATTR(file)
#define li_ELEMENT_ATTRS()
#define license_ELEMENT_ATTRS()
#define nick_ELEMENT_ATTRS()		ATTR(name)
#define reg16_ELEMENT_ATTRS()		ATTR(name) ATTR(offset) ATTR(access) ATTR(type) ATTR(shr) ATTR(varset) ATTR(variants) ATTR(stride) ATTR(length) ATTR(align) ATTR(max) ATTR(min)
#define reg32_ELEMENT_ATTRS()		ATTR(name) ATTR(offset) ATTR(access) ATTR(type) ATTR(shr) ATTR(varset) ATTR(variants) ATTR(stride) ATTR(length) ATTR(align) ATTR(max) ATTR(min)
#define reg64_ELEMENT_ATTRS()		ATTR(name) ATTR(offset) ATTR(access) ATTR(type) ATTR(shr) ATTR(varset) ATTR(variants) ATTR(stride) ATTR(length) ATTR(align) ATTR(max) ATTR(min)
#define reg8_ELEMENT_ATTRS()		ATTR(name) ATTR(offset) ATTR(access) ATTR(type) ATTR(shr) ATTR(varset) ATTR(variants) ATTR(stride) ATTR(length) ATTR(align) ATTR(max) ATTR(min)
#define spectype_ELEMENT_ATTRS()	ATTR(name) ATTR(type)
#define stripe_ELEMENT_ATTRS()		ATTR(length) ATTR(name) ATTR(offset) ATTR(prefix) ATTR(stride) \
									ATTR(variants) ATTR(varset)
#define ul_ELEMENT_ATTRS()
#define use_group_ELEMENT_ATTRS()	ATTR(name)
#define value_ELEMENT_ATTRS()		ATTR(name) ATTR(value) ATTR(variants) ATTR(varset)

// each attribute type is a value and a "was it specified" bit 'b'.
template<class T> struct spec_t {
	T v;
	bool b;
	void from(const T &a)             { b = true; v = a;    }
	void from(const QStringRef &r)    { b = true; v  = r;   }
	void from(const spec_t &a)        { b = true; v  = a.v; }
	void concat_from(const spec_t &a, const QString sep=QString()) {
		b = true;
		if ( v.length() )
			v = v + sep + a.v;
		else
			v = a.v;
	}
	void add_from(const spec_t &a)    { b = true; v += a.v; }

};

// conversion specializations
template<> void spec_t<bool>    ::from(const QStringRef &r);
template<> void spec_t<QString> ::from(const QStringRef &r);
template<> void spec_t<uint32_t>::from(const QStringRef &r);

typedef spec_t<QString> file_attr_spec_t;
typedef spec_t<QString> schemaLocation_attr_spec_t;
typedef spec_t<bool>   bare_attr_spec_t;
typedef spec_t<bool>   inline_attr_spec_t;

typedef spec_t<QString> access_attr_spec_t;
typedef spec_t<QString> name_attr_spec_t;
typedef spec_t<QString> variants_attr_spec_t;
typedef spec_t<QString> type_attr_spec_t;
typedef spec_t<QString> varset_attr_spec_t;
typedef spec_t<QString> prefix_attr_spec_t;
typedef spec_t<QString> year_attr_spec_t;
typedef spec_t<QString> email_attr_spec_t;
typedef spec_t<QString> value_attr_spec_t;

typedef spec_t<uint32_t> offset_attr_spec_t;
typedef spec_t<uint32_t> shr_attr_spec_t;
typedef spec_t<uint32_t> size_attr_spec_t;
typedef spec_t<uint32_t> stride_attr_spec_t;
typedef spec_t<uint32_t> length_attr_spec_t;
typedef spec_t<uint32_t> width_attr_spec_t;
typedef spec_t<uint32_t> align_attr_spec_t;
typedef spec_t<uint32_t> max_attr_spec_t;
typedef spec_t<uint32_t> min_attr_spec_t;
typedef spec_t<uint32_t> add_attr_spec_t;
typedef spec_t<uint32_t> pos_attr_spec_t;
typedef spec_t<uint32_t> radix_attr_spec_t;
typedef spec_t<uint32_t> high_attr_spec_t;
typedef spec_t<uint32_t> low_attr_spec_t;


#if 0
// unused as of yet.  but just in case... the following
// can be generated to provide a struct to hold any
// particular element's attrs.
struct import_attrs_t {
	import_ELEMENT_ATTRS()
};
struct array_attrs_t {
	array_ELEMENT_ATTRS()
};
struct author_attrs_t {
	author_ELEMENT_ATTRS()
};
// ^+-- vs. automatic below --+v
#define ELEMENT(X) 	struct X##_attrs_t { X##_ELEMENT_ATTRS() };
#define ATTR(X) X ## _attr_spec_t _##X;
ELEMENTS()
#undef ELEMENT
#endif

// records all possible attr specifications
struct attr_spec_t {
	bare_attr_spec_t   _bare;
	inline_attr_spec_t _inline;

	access_attr_spec_t _access;
	name_attr_spec_t _name;
	variants_attr_spec_t _variants;
	type_attr_spec_t _type;
	varset_attr_spec_t _varset;
	prefix_attr_spec_t _prefix;
	year_attr_spec_t _year;
	email_attr_spec_t _email;
	value_attr_spec_t _value;

	offset_attr_spec_t _offset;
	shr_attr_spec_t _shr;
	size_attr_spec_t _size;
	stride_attr_spec_t _stride;
	length_attr_spec_t _length;
	width_attr_spec_t _width;
	align_attr_spec_t _align;
	max_attr_spec_t _max;
	min_attr_spec_t _min;
	add_attr_spec_t _add;
	pos_attr_spec_t _pos;
	radix_attr_spec_t _radix;
	high_attr_spec_t _high;
	low_attr_spec_t _low;

	void zap_spec_bits() {
		// TBD: make a closed-form version instead of this bug-prone mess.
		_bare.b = _inline.b =
			false;

		_access.b =
			_name.b =
			_variants.b =
			_type.b =
			_varset.b =
			_prefix.b =
			_year.b =
			_email.b =
			_value.b =
			false;

		_offset.b =
			_shr.b =
			_size.b =
			_stride.b =
			_length.b =
			_width.b =
			_align.b =
			_max.b =
			_min.b =
			_add.b =
			_pos.b =
			_radix.b =
			_high.b =
			_low.b =
			false;
	}
	attr_spec_t() { zap_spec_bits(); }
};


//
// representation of the xml content of a file.
//
class xml_node_t {
public:
	QXmlStreamReader::TokenType t;
	QList<xml_node_t *> nodes;
	xml_node_t (QXmlStreamReader::TokenType t) : t(t) { }
	virtual ~xml_node_t(){ }
	virtual void write(QXmlStreamWriter &ox) {
		for ( auto n : nodes ) {
			n->write(ox);
		}
	}
};

class xml_document_node_t : public xml_node_t {
public:
	xml_document_node_t() : xml_node_t(QXmlStreamReader::StartDocument) { }
	virtual ~xml_document_node_t(){ }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeStartDocument();
		for (auto n : nodes ) {
			n->write(ox);
		}
		ox.writeEndDocument();
	}
};

class xml_element_node_t : public xml_node_t {
public:
	QString name;
	QXmlStreamAttributes attrs;
	xml_element_node_t(const QString &name, const QXmlStreamAttributes &attrs) :
		xml_node_t(QXmlStreamReader::StartElement), name(name), attrs(attrs) { }
	virtual ~xml_element_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeStartElement(QString(), name);
		ox.writeAttributes(attrs);
		for (auto n: nodes) {
			n->write(ox);
		}
		ox.writeEndElement();
	}
};

class xml_chars_node_t : public xml_node_t {
public:
	bool isCDATA;
	QString text;
	xml_chars_node_t(const QString &text, bool isCDATA) :
		xml_node_t(QXmlStreamReader::Characters), text(text), isCDATA(isCDATA) { }
	virtual ~xml_chars_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeCharacters(text);
	}
};

class xml_comment_node_t : public xml_node_t {
public:
	QString text;
	xml_comment_node_t(const QString &text) : xml_node_t(QXmlStreamReader::Comment), text(text) { }
	virtual ~xml_comment_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeComment(text);
	}
};

class xml_dtd_node_t : public xml_node_t {
public:
	QXmlStreamNotationDeclarations n_decls;
	QXmlStreamEntityDeclarations  e_decls;
	QString text;
	xml_dtd_node_t(const QString &text, const QXmlStreamNotationDeclarations &n,
				   const QXmlStreamEntityDeclarations &e) :
		xml_node_t(QXmlStreamReader::DTD), text(text), n_decls(n), e_decls(e) { }

	virtual ~xml_dtd_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeDTD(text);
	}
};

class xml_entity_reference_node_t : public xml_node_t {
public:
	QString ref;
	QString text;
	xml_entity_reference_node_t(const QString &text, const QString &ref) :
		xml_node_t(QXmlStreamReader::EntityReference), text(text), ref(ref) { }
	virtual ~xml_entity_reference_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeEntityReference(text);
	}
};

class xml_processing_instruction_node_t : public xml_node_t {
public:
	QString target;
	QString data;
	xml_processing_instruction_node_t(const QString &target, const QString &data) :
		xml_node_t(QXmlStreamReader::ProcessingInstruction), target(target), data(data) { }
	virtual ~xml_processing_instruction_node_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeProcessingInstruction(target, data);
	}
};

class xml_import_element_t : public xml_element_node_t {
public:
	xml_import_element_t(const QString &filel, QXmlStreamAttributes &attrs) :
		xml_element_node_t("import", attrs)
	{

	}
	virtual ~xml_import_element_t() { }
	virtual void write(QXmlStreamWriter &ox) {
		ox.writeStartElement(QString(), "import");
		for (auto a : attrs ) {
			ox.writeAttribute(a);
		}
		for (auto n : nodes) { 
			n->write(ox); // e.g. chars
		}
		ox.writeEndElement();
	}
};
