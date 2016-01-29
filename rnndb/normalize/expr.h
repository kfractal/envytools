/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

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
