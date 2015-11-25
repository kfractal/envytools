// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-


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
// note: the "use-group" element is handled differentely as needed since it isn't a token


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
#define enum_ELEMENT_ATTRS()		ATTR(bare) ATTR(inline) ATTR(name) ATTR(varset)
#define group_ELEMENT_ATTRS()		ATTR(name)
#define import_ELEMENT_ATTRS()		ATTR(file)
#define li_ELEMENT_ATTRS()
#define license_ELEMENT_ATTRS()
#define nick_ELEMENT_ATTRS()		ATTR(name)
#define reg16_ELEMENT_ATTRS()		ATTR(name) ATTR(offset)
#define reg32_ELEMENT_ATTRS()		ATTR(access) ATTR(align) ATTR(length) ATTR(max) ATTR(min) \
									ATTR(name) ATTR(offset) ATTR(shr) ATTR(stride) ATTR(type) \
									ATTR(variants) ATTR(varset)
#define reg64_ELEMENT_ATTRS()		ATTR(length) ATTR(name) ATTR(offset) ATTR(shr) ATTR(variants)
#define reg8_ELEMENT_ATTRS()		ATTR(access) ATTR(length) ATTR(name) ATTR(offset) ATTR(shr) \
									ATTR(type) ATTR(variants)
#define spectype_ELEMENT_ATTRS()	ATTR(name) ATTR(type)
#define stripe_ELEMENT_ATTRS()		ATTR(length) ATTR(name) ATTR(offset) ATTR(prefix) ATTR(stride) \
									ATTR(variants) ATTR(varset)
#define ul_ELEMENT_ATTRS()
#define use_group_ELEMENT_ATTRS()	ATTR(name)
#define value_ELEMENT_ATTRS()		ATTR(name) ATTR(value) ATTR(variants) ATTR(varset)

// need regexp here? (whitespace and the like?)
static inline uint32_t from_width(const QStringRef &s) {
	return s.toInt();
}
static inline uint32_t from_offset(const QStringRef &s){
	return s.toInt();
}
static inline uint32_t from_shr(const QStringRef &s){
	return s.toInt();
}
static inline uint32_t from_stride(const QStringRef &s){
	return s.toInt();
}
static inline uint32_t from_length(const QStringRef &s){
	return s.toInt();
}

// union-esque thing which records which attrs at any
// given element have been specified (vs. inherited)
struct attr_spec_t {
	struct bool_spec_t {
		bool t;
		bool b;
		void from(const QStringRef &s) {
			b = true;
			if ( ( 0 == s.compare("yes", Qt::CaseInsensitive) )  ||
				 ( 0 == s.compare("true", Qt::CaseInsensitive) ) ||
				 ( s == "1" ) )
				t = true;
			t = false;
		}
		bool_spec_t() : t(false), b(false) { }
		bool_spec_t(const bool_spec_t &o) : t(o.t), b(false) { }
	};
	struct string_spec_t {
		QString s;
		bool b;
		void from(const QStringRef &r) { s = r.toString(); b = true; }
		// void from(struct string_spec_t &o) { s = o._s; b = false; }
		string_spec_t() : s(QString()), b(false) { }
		string_spec_t(const string_spec_t &o) : s(o.s), b(false) { }
	};
	struct uint32_spec_t {
		uint32_t n;
		bool b;
		void from(const QStringRef &r) { n = r.toInt(); b = true; }
		uint32_spec_t() : n(0), b(false) { }
		uint32_spec_t(const uint32_spec_t &o) : n(o.n), b(false) { }
	};

	bool_spec_t _bare;
	bool_spec_t _inline; // boo

	string_spec_t _access;
	string_spec_t _name;
	string_spec_t _variants;
	string_spec_t _type;
	string_spec_t _varset;
	string_spec_t _prefix;
	string_spec_t _year;
	string_spec_t _email;
	string_spec_t _value;

	uint32_spec_t _offset;
	uint32_spec_t _shr;
	uint32_spec_t _size;
	uint32_spec_t _stride;
	uint32_spec_t _length;
	uint32_spec_t _width;
	uint32_spec_t _align;
	uint32_spec_t _max;
	uint32_spec_t _min;
	uint32_spec_t _add;
	uint32_spec_t _pos;
	uint32_spec_t _radix;

	uint32_spec_t _high;
	uint32_spec_t _low;

	attr_spec_t() { }
	attr_spec_t(const attr_spec_t &o) :
		_bare(o._bare), _inline(o._inline),
		_access(o._access), _name(o._name), _variants(o._variants),
		_type(o._type), _varset(o._varset), _prefix(o._prefix),
		_year(o._year), _email(o._email), _value(o._value),
		_offset(o._offset), _shr(o._shr), _size(o._size), _stride(o._stride),
		_length(o._length), _width(o._width), _align(o._align),
		_max(o._max), _min(o._min), _add(o._add), _pos(o._pos), _radix(o._radix),
		_high(o._high), _low(o._low)
	{ }
};
