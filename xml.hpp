#pragma once
#include "parsec.hpp"
namespace parsec::xml {

// Comment      ::=    '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
//
// content      ::=    CharData? ((element | Reference | CDSect | PI | Comment)
// CharData?)*
//
// AttValue     ::=    '"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] |
// Reference)* "'"
//
// Attribute    ::=    Name Eq AttValue
// ETag         ::=    '</' Name S? '>'
// STag         ::=    '<' Name (S Attribute)* S? '>'
// EmptyElemTag ::=    '<' Name (S Attribute)* S? '/>'
// element      ::=    EmptyElemTag | STag content ETag
// document     ::=    prolog element Misc*

} // namespace parsec::xml
