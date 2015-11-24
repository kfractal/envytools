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
