#pragma once
#include "parsec.hpp"

#define C constexpr inline auto
#include <iostream>
namespace parsec::xml {

template <typename P> struct log {
  P p;
  template <typename O> void operator()(O o, const char *in) const {
    std::cout << "S:" << in[0] << std::endl;
    p(::rays{[&](error_ray *, int err) {
               std::cout << "E:" << err << std::endl;
               o(error_ray_v, err);
             },
             [&](int len) {
               std::cout << "L:" << len << std::endl;
               std::cout << "SS:" << in[len] << std::endl;
               o(len);
             }},
      in);
  }
};
template <typename P> log(P)->log<P>;

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

// Reference ::= EntityRef | CharRef
C Reference = [](auto o, const char *) { o(0); };

// content ::= CharData? ((element | Reference | CDSect | PI | Comment)
// CharData?)*

// AttValue ::= '"' ([^<&"] | Reference)* '"' | "'" ([^<&'] |
// Reference)* "'"
C AttValue = str{"\""} & many{0, noneOf{"<&\""}} & str{"\""} |
             str{"'"} & many{0, noneOf{"<&'"}} & str{"'"};

// Eq ::= S? '=' S?
C Eq = opt{S} & str{"="} & opt{S};

// Attribute ::= Name Eq AttValue
C Attribute = Name & Eq & AttValue;

// ETag ::= '</' Name S? '>'
// STag         ::= '<' Name (S Attribute)* S? '>'
// EmptyElemTag ::= '<' Name (S Attribute)* S? '/>'

// element ::= EmptyElemTag | STag content ETag
struct element_ {
  template <typename O> void operator()(O o, const char *in) const {
    (str{"<"} & Name & many{0, S & Attribute} & opt{S} & str{">"})(o, in);
  }
};
C element = element_{};

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
