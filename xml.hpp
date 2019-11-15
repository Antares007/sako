#pragma once
#include "parsec.hpp"
namespace parsec::xml {

constexpr inline auto element = [](auto...) {};
constexpr inline auto EncodingDecl = [](auto...) {};
constexpr inline auto SDDecl = [](auto...) {};
constexpr inline auto S = [](auto...) {};
constexpr inline auto Comment = [](auto...) {};
constexpr inline auto PI = [](auto...) {};
constexpr inline auto Name = [](auto...) {};
constexpr inline auto ExternalID = [](auto...) {};
constexpr inline auto PEReference = [](auto...) {};
constexpr inline auto AttlistDecl = [](auto...) {};
constexpr inline auto EntityDecl = [](auto...) {};
constexpr inline auto NotationDecl = [](auto...) {};
constexpr inline auto Mixed = [](auto...) {};

// cp       ::= (Name | choice | seq) ('?' | '*' | '+')?
// seq      ::= '(' S? cp ( S? ',' S? cp )* S? ')'
// choice   ::= '(' S? cp ( S? '|' S? cp )+ S? ')'
// children ::= (choice | seq) ('?' | '*' | '+')?

// mul      ::= ('?' | '*' | '+')?
// cp       ::= (Name | choice | seq) mul
// seq      ::= '(' S? cp ( S? ',' S? cp )* S? ')'
// choice   ::= '(' S? cp ( S? '|' S? cp )+ S? ')'
// children ::= (choice | seq) mul

constexpr inline auto mul = [](auto o, const char *s, size_t a) {
  o(!!(a > 0 && (s[0] == '?' || s[0] == '*' || s[0] == '+')));
};
constexpr inline auto cp = Name & opt(mul);

constexpr inline auto children = [](auto...) {};

// contentspec	   ::=   	'EMPTY' | 'ANY' | Mixed | children
constexpr inline auto contentspec = "EMPTY" | Mixed | "ANY" | children;

// elementdecl	   ::=   	'<!ELEMENT' S Name S contentspec S? '>'
constexpr inline auto elementdecl =
    "<!ELEMENT" & S & Name & S & contentspec & opt(S) & ">";

// markupdecl	   ::=   	elementdecl | AttlistDecl | EntityDecl |
// NotationDecl | PI | Comment
constexpr inline auto markupdecl =
    elementdecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment;

// DeclSep	   ::=   	PEReference | S
constexpr inline auto DeclSep = PEReference | S;

// intSubset	   ::=   	(markupdecl | DeclSep)*
constexpr inline auto intSubset = many0(markupdecl | DeclSep);

// doctypedecl::='<!DOCTYPE' S Name (S ExternalID)? S? ('[' intSubset ']' S?)?
// '>'
constexpr inline auto doctypedecl = "<!DOCTYPE" & S & Name &
                                    opt(S & ExternalID) & opt(S) &
                                    opt("[" & intSubset & "]" & opt(S)) & ">";

// Misc	   ::=   	Comment | PI | S
constexpr inline auto Misc = Comment | PI | S;

// Eq	   ::=   	S? '=' S?
constexpr inline auto Eq = opt(S) & "=" & opt(S);

//	VersionNum	   ::=   	'1.' [0-9]+
constexpr inline auto VersionNum =
    "1." & many1([](auto o, const char *s, const size_t a) {
      o(!!(a > 0 && ('0' <= s[0] && s[0] <= '9')));
    });

// VersionInfo	   ::=   	S 'version' Eq ("'" VersionNum "'" | '"'
// VersionNum '"')
constexpr inline auto VersionInfo =
    S & "version" & Eq &
    (("'" & VersionNum & "'") | ("\"" & VersionNum & "\""));

// XMLDecl	   ::=   	'<?xml' VersionInfo EncodingDecl? SDDecl? S?
// '?>'
constexpr inline auto XMLDecl =
    "<?xml" & VersionInfo & opt(EncodingDecl) & opt(SDDecl) & opt(S);

// prolog	   ::=   	XMLDecl? Misc* (doctypedecl Misc*)?
constexpr inline auto prolog =
    opt(XMLDecl) & many0(Misc) & opt(doctypedecl & many0(Misc));

// document	   ::=   	prolog element Misc*
constexpr inline auto document = prolog & element & many0(Misc);

} // namespace parsec::xml
