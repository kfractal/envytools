%{
#include <iostream>
#include "expr.h"
using namespace std;
%}

%option yyclass="FooLexer"
%option noyywrap
%option never-interactive
%s IN_COMMENT
%s IN_SSTRING
%s IN_DSTRING

HEXDIGIT  [a-zA-Z0-9]
DIGIT     [0-9]
WS        [\t ]
NL        \n
LOP       :
UOP       (\+\+|\-\-)
MOP       [\+\-\*/]
GROUP     [()]
IDENT     [_a-zA-Z0-9]
SIDENT    [_a-zA-Z]
HASHOP    #
KEYWORD   (defined|define|ifndef|ifdef|endif|else)
SQUOT     '
DQUOT     \"
BSLASH    \\
%%

<INITIAL>{
"/*"              BEGIN(IN_COMMENT);
{DQUOT}           BEGIN(IN_DSTRING);
{SQUOT}           BEGIN(IN_SSTRING);
}

<IN_DSTRING>{
{BSLASH}{DQUOT}   // not done
{DQUOT}           push_dquot_string(YYText()); BEGIN(INITIAL);
}

<IN_SSTRING>{
{BSLASH}{SQUOT}   // not done
{SQUOT}          push_squot_string(YYText()); BEGIN(INITIAL);
}

<IN_COMMENT>{
"*/"      push_comment(YYText()); BEGIN(INITIAL);
[^*\n]+   // eat comment in chunks
"*"       // eat the lone star
\n        yylineno++;
}

{BSLASH}{NL}     { push_whitespace(); }
{NL}+            { push_newline(); }
{WS}+            { push_whitespace(); }
{HASHOP}         { push_hashop(); }
{KEYWORD}        { push_keyword(YYText()); }
"0x"{HEXDIGIT}+  { push_hex_literal(YYText()); }
{DIGIT}+         { push_dec_literal(YYText()); }
{SIDENT}{IDENT}* { push_identifier(YYText()); }
{GROUP}          { push_grouping(YYText()); }
{LOP}            { push_lop(YYText()); }
{UOP}            { push_uop(YYText()); }
{MOP}            { push_mop(YYText()); }
.                { push_unk(YYText()); }
%%

