#pragma once
#include "parsec.hpp"

#define C constexpr inline auto

namespace parsec::xml {

// NameStartChar::= ":" | [A-Z] | "_" | [a-z] | [#xC0-#xD6] |
// [#xD8-#xF6] | [#xF8-#x2FF] | [#x370-#x37D] | [#x37F-#x1FFF] | [#x200C-#x200D]
// | [#x2070-#x218F] | [#x2C00-#x2FEF] | [#x3001-#xD7FF] | [#xF900-#xFDCF] |
// [#xFDF0-#xFFFD] | [#x10000-#xEFFFF]
C NameStartChar =
    chr{[](char c) {
      return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == ':' ||
             c == '_';
    }} |
    u8cp{[](uint32_t cp) {
      return (0xC0 <= cp && cp <= 0xD6) || (0xD8 <= cp && cp <= 0xF6) ||
             (0xF8 <= cp && cp <= 0x2FF) || (0x370 <= cp && cp <= 0x37D) ||
             (0x37F <= cp && cp <= 0x1FFF) || (0x200C <= cp && cp <= 0x200D) ||
             (0x2070 <= cp && cp <= 0x218F) || (0x2C00 <= cp && cp <= 0x2FEF) ||
             (0x3001 <= cp && cp <= 0xD7FF) || (0xF900 <= cp && cp <= 0xFDCF) ||
             (0xFDF0 <= cp && cp <= 0xFFFD) || (0x10000 <= cp && cp <= 0xEFFFF);
    }};

// NameChar	 ::= NameStartChar | "-" | "." | [0-9] | #xB7 |
// [#x0300-#x036F] | [#x203F-#x2040]
C NameChar =
    NameStartChar |
    chr{[](char c) { return c == '-' || c == '.' || ('0' <= c && c <= '9'); }} |
    u8cp{[](uint32_t cp) {
      return cp == 0xB7 || (0x0300 <= cp && cp <= 0x036F) ||
             (0x203F <= cp && cp <= 0x2040);
    }};

// Name	 ::= NameStartChar (NameChar)*
C Name = NameStartChar & many{0, NameChar};

// S ::= (#x20 | #x9 | #xD | #xA)+
C S = many{1, chr{[](char c) {
             return c == 0x20 || c == 0x09 || c == 0x0D || c == 0x0A;
           }}};

// Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
C Comment =
    str{"<!--"} & (many{0, noneOf{"-"} | str{"-"} & noneOf{"-"}}) & str{"-->"};

// content ::= CharData? ((element | Reference | CDSect | PI | Comment)
// CharData?)*

// AttValue ::= '"' ([^<&"] | Reference)* '"' | "'" ([^<&'] |
// Reference)* "'"

// Attribute ::= Name Eq AttValue
// ETag ::= '</' Name S? '>'
// STag ::= '<' Name (S Attribute)* S? '>'
// EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'

// element ::= EmptyElemTag | STag content ETag
C element = [](auto o, const char *) { o(0); };

// PI ::= '<?' PITarget (S (Char* - (Char* '?>' Char*)))? '?>'
C PI = str{"<?"} & many{0, noneOf{"?"}} & str{"?>"};

// Misc ::= Comment | PI | S
C Misc = Comment | PI | S;

// XMLDecl ::= '<?xml' VersionInfo EncodingDecl? SDDecl? S? '?>'
C XMLDecl = str{"<?xml"} & many{0, noneOf{"?"}} & str{"?>"};

// doctypedecl ::='<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)?
// '>'
C doctypedecl = str{"<!DOCTYPE"} & many{0, noneOf{">"}} & str{">"};

// prolog ::= XMLDecl? Misc* (doctypedecl Misc*)?
C prolog = opt{XMLDecl} & many{0, Misc} & opt{doctypedecl & many{0, Misc}};

// document ::= prolog element Misc*
C document = prolog & element & many{0, Misc};

} // namespace parsec::xml
