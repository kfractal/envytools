#pragma once

#include <list>

class FooLexer : public yyFlexLexer {
public:
    enum class token_type_e {
        comment, keyword,
            newline,
            hashop, hex_literal, dec_literal,
            identifier, grouping,
            squot_string, dquot_string,
            lop, uop, mop, whitespace, unk
    };

    struct token_t {
        token_type_e _t;
        std::string _s;
        token_t(token_type_e t, std::string s) : _t(t), _s(s) { }
    };

    inline bool is_whitespace(const std::list<token_t>::iterator i) {
        return i->_t == token_type_e::whitespace; }
    inline bool is_newline(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::newline; }
    inline bool is_comment(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::comment; }
    inline bool is_hashop(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::hashop; }
    inline bool is_keyword(const std::list<token_t>::iterator i, const std::string &c) {
        return (i->_t == token_type_e::keyword) && (i->_s == c); }
    inline bool is_grouping(const std::list<token_t>::iterator i, const std::string &c) {
        return (i->_t == token_type_e::grouping) && (i->_s == c); }
    inline bool is_identifier(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::identifier; }
    inline bool is_hex_literal(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::hex_literal; }
    inline bool is_dec_literal(const std::list<token_t>::iterator i){
        return i->_t == token_type_e::dec_literal; }


    std::list<token_t> _tokens;
    std::string token_str(std::list<token_t>::iterator i) { return i->_s; }
    std::list<token_t>::iterator begin() { return _tokens.begin();}
    std::list<token_t>::iterator end() { return _tokens.end(); }
    FooLexer(std::istream *in, std::ostream *out) : yyFlexLexer(in, out) { }
    void doit(const char *it) { std::cout << "[" << it << "]"; }
    virtual int yylex();

    void push_comment(const char *s);
    void push_hashop();
    void push_keyword(const char *s);
    void push_newline();
    void push_whitespace();
    void push_hex_literal(const char *s);
    void push_dec_literal(const char *s);
    void push_identifier(const char *s);
    void push_grouping(const char *s);
    void push_lop(const char *s);
    void push_uop(const char *s);
    void push_mop(const char *s);
    void push_unk(const char *s);
    void push_dquot_string(const char *s);
    void push_squot_string(const char *s);
    std::string simplified();
};