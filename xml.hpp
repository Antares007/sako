#pragma once
#include "parsec.hpp"

#define C constexpr inline auto

namespace parsec::xml {

// NameStartChar::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] |
// [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D]
// | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] |
// [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
C NameStartChar =
    chr{[](auto c) {
      return c == ':' || c == '_' || 'a' <= c && c <= 'z' ||
             'A' <= c && c <= 'Z';
    }} |
    u8cp{[](auto cp) {
      return 0xC0 <= cp && cp <= 0xD6 || 0xD8 <= cp && cp <= 0xF6 ||
             0xF8 <= cp && cp <= 0x2FF || 0x370 <= cp && cp <= 0x37D ||
             0x37F <= cp && cp <= 0x1FFF || 0x200C <= cp && cp <= 0x200D ||
             0x2070 <= cp && cp <= 0x218F || 0x2C00 <= cp && cp <= 0x2FEF ||
             0x3001 <= cp && cp <= 0xD7FF || 0xF900 <= cp && cp <= 0xFDCF ||
             0xFDF0 <= cp && cp <= 0xFFFD || 0x10000 <= cp && cp <= 0xEFFFF;
    }};

// NameChar	    ::= NameStartChar | "-" | "." | [0-9] | #xB7 |
// [#x0300-#x036F] | [#x203F-#x2040]
C NameChar =
    NameStartChar |
    chr{[](auto c) { return c == '-' || c == '.' || '0' <= c && c <= '9'; }} |
    u8cp{[](auto cp) {
      return cp == 0xB7 || 0x0300 <= cp && cp <= 0x036F ||
             0x203F <= cp && cp <= 0x2040;
    }};

// Name	        ::= NameStartChar (NameChar)*
C Name = NameStartChar & many0{NameChar};

// Comment      ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'

// content      ::= CharData? ((element | Reference | CDSect | PI | Comment)
// CharData?)*

// AttValue     ::= '"' ([^<&"] | Reference)* '"' |  "'" ([^<&'] |
// Reference)* "'"

// Attribute    ::= Name Eq AttValue
// ETag         ::= '</' Name S? '>'
// STag         ::= '<' Name (S Attribute)* S? '>'
// EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'
// element      ::= EmptyElemTag | STag content ETag
// document     ::= prolog element Misc*

} // namespace parsec::xml
