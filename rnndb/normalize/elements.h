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


// union-esque thing which records attr specifications
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
	};
	struct string_spec_t {
		QString s;
		bool b;
		void from(const QStringRef &r) { s = r.toString(); b = true; }
	};
	struct uint32_spec_t {
		uint32_t n;
		bool b;
		// tbd: throw or propagate conversion failure...
		void from(const QStringRef &r) {
			bool ok;
			n = r.toUInt(&ok); b = true;
			// XXX docs for QString::to*Int/Long says it spots the "0x" on its own.
			// but empirically, it does not...
			if ( !ok && r.startsWith("0x", Qt::CaseInsensitive) ) {
				n = r.toUInt(&ok, 16);
				if ( !ok ) {
					qDebug() << "error: couldn't turn this into a number:" << r;
					n = ~(uint32_t)0;
				}
			}
		}
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
};
