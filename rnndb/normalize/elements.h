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
