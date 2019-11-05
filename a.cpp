#include "parsec.hpp"
namespace parsec::xml {
#define C constexpr auto
C BaseChar = u8cp([](auto cp) {
  return (0x41 <= cp && cp <= 0x5A) || (0x61 <= cp && cp <= 0x7A) ||
         (0xC0 <= cp && cp <= 0xD6) || (0xD8 <= cp && cp <= 0xF6) ||
         (0xF8 <= cp && cp <= 0xFF) || (0x100 <= cp && cp <= 0x131) ||
         (0x134 <= cp && cp <= 0x13E) || (0x141 <= cp && cp <= 0x148) ||
         (0x14A <= cp && cp <= 0x17E) || (0x180 <= cp && cp <= 0x1C3) ||
         (0x1CD <= cp && cp <= 0x1F0) || (0x1F4 <= cp && cp <= 0x1F5) ||
         (0x1FA <= cp && cp <= 0x217) || (0x250 <= cp && cp <= 0x2A8) ||
         (0x2BB <= cp && cp <= 0x2C1) || cp == 0x386 ||
         (0x388 <= cp && cp <= 0x38A) || cp == 0x38C ||
         (0x38E <= cp && cp <= 0x3A1) || (0x3A3 <= cp && cp <= 0x3CE) ||
         (0x3D0 <= cp && cp <= 0x3D6) || cp == 0x3DA || cp == 0x3DC ||
         cp == 0x3DE || cp == 0x3E0 || (0x3E2 <= cp && cp <= 0x3F3) ||
         (0x401 <= cp && cp <= 0x40C) || (0x40E <= cp && cp <= 0x44F) ||
         (0x451 <= cp && cp <= 0x45C) || (0x45E <= cp && cp <= 0x481) ||
         (0x490 <= cp && cp <= 0x4C4) || (0x4C7 <= cp && cp <= 0x4C8) ||
         (0x4CB <= cp && cp <= 0x4CC) || (0x4D0 <= cp && cp <= 0x4EB) ||
         (0x4EE <= cp && cp <= 0x4F5) || (0x4F8 <= cp && cp <= 0x4F9) ||
         (0x531 <= cp && cp <= 0x556) || cp == 0x559 ||
         (0x561 <= cp && cp <= 0x586) || (0x5D0 <= cp && cp <= 0x5EA) ||
         (0x5F0 <= cp && cp <= 0x5F2) || (0x621 <= cp && cp <= 0x63A) ||
         (0x641 <= cp && cp <= 0x64A) || (0x671 <= cp && cp <= 0x6B7) ||
         (0x6BA <= cp && cp <= 0x6BE) || (0x6C0 <= cp && cp <= 0x6CE) ||
         (0x6D0 <= cp && cp <= 0x6D3) || cp == 0x6D5 ||
         (0x6E5 <= cp && cp <= 0x6E6) || (0x905 <= cp && cp <= 0x939) ||
         cp == 0x93D || (0x958 <= cp && cp <= 0x961) ||
         (0x985 <= cp && cp <= 0x98C) || (0x98F <= cp && cp <= 0x990) ||
         (0x993 <= cp && cp <= 0x9A8) || (0x9AA <= cp && cp <= 0x9B0) ||
         cp == 0x9B2 || (0x9B6 <= cp && cp <= 0x9B9) ||
         (0x9DC <= cp && cp <= 0x9DD) || (0x9DF <= cp && cp <= 0x9E1) ||
         (0x9F0 <= cp && cp <= 0x9F1) || (0xA05 <= cp && cp <= 0xA0A) ||
         (0xA0F <= cp && cp <= 0xA10) || (0xA13 <= cp && cp <= 0xA28) ||
         (0xA2A <= cp && cp <= 0xA30) || (0xA32 <= cp && cp <= 0xA33) ||
         (0xA35 <= cp && cp <= 0xA36) || (0xA38 <= cp && cp <= 0xA39) ||
         (0xA59 <= cp && cp <= 0xA5C) || cp == 0xA5E ||
         (0xA72 <= cp && cp <= 0xA74) || (0xA85 <= cp && cp <= 0xA8B) ||
         cp == 0xA8D || (0xA8F <= cp && cp <= 0xA91) ||
         (0xA93 <= cp && cp <= 0xAA8) || (0xAAA <= cp && cp <= 0xAB0) ||
         (0xAB2 <= cp && cp <= 0xAB3) || (0xAB5 <= cp && cp <= 0xAB9) ||
         cp == 0xABD || cp == 0xAE0 || (0xB05 <= cp && cp <= 0xB0C) ||
         (0xB0F <= cp && cp <= 0xB10) || (0xB13 <= cp && cp <= 0xB28) ||
         (0xB2A <= cp && cp <= 0xB30) || (0xB32 <= cp && cp <= 0xB33) ||
         (0xB36 <= cp && cp <= 0xB39) || cp == 0xB3D ||
         (0xB5C <= cp && cp <= 0xB5D) || (0xB5F <= cp && cp <= 0xB61) ||
         (0xB85 <= cp && cp <= 0xB8A) || (0xB8E <= cp && cp <= 0xB90) ||
         (0xB92 <= cp && cp <= 0xB95) || (0xB99 <= cp && cp <= 0xB9A) ||
         cp == 0xB9C || (0xB9E <= cp && cp <= 0xB9F) ||
         (0xBA3 <= cp && cp <= 0xBA4) || (0xBA8 <= cp && cp <= 0xBAA) ||
         (0xBAE <= cp && cp <= 0xBB5) || (0xBB7 <= cp && cp <= 0xBB9) ||
         (0xC05 <= cp && cp <= 0xC0C) || (0xC0E <= cp && cp <= 0xC10) ||
         (0xC12 <= cp && cp <= 0xC28) || (0xC2A <= cp && cp <= 0xC33) ||
         (0xC35 <= cp && cp <= 0xC39) || (0xC60 <= cp && cp <= 0xC61) ||
         (0xC85 <= cp && cp <= 0xC8C) || (0xC8E <= cp && cp <= 0xC90) ||
         (0xC92 <= cp && cp <= 0xCA8) || (0xCAA <= cp && cp <= 0xCB3) ||
         (0xCB5 <= cp && cp <= 0xCB9) || cp == 0xCDE ||
         (0xCE0 <= cp && cp <= 0xCE1) || (0xD05 <= cp && cp <= 0xD0C) ||
         (0xD0E <= cp && cp <= 0xD10) || (0xD12 <= cp && cp <= 0xD28) ||
         (0xD2A <= cp && cp <= 0xD39) || (0xD60 <= cp && cp <= 0xD61) ||
         (0xE01 <= cp && cp <= 0xE2E) || cp == 0xE30 ||
         (0xE32 <= cp && cp <= 0xE33) || (0xE40 <= cp && cp <= 0xE45) ||
         (0xE81 <= cp && cp <= 0xE82) || cp == 0xE84 ||
         (0xE87 <= cp && cp <= 0xE88) || cp == 0xE8A || cp == 0xE8D ||
         (0xE94 <= cp && cp <= 0xE97) || (0xE99 <= cp && cp <= 0xE9F) ||
         (0xEA1 <= cp && cp <= 0xEA3) || cp == 0xEA5 || cp == 0xEA7 ||
         (0xEAA <= cp && cp <= 0xEAB) || (0xEAD <= cp && cp <= 0xEAE) ||
         cp == 0xEB0 || (0xEB2 <= cp && cp <= 0xEB3) || cp == 0xEBD ||
         (0xEC0 <= cp && cp <= 0xEC4) || (0xF40 <= cp && cp <= 0xF47) ||
         (0xF49 <= cp && cp <= 0xF69) || (0x10A0 <= cp && cp <= 0x10C5) ||
         (0x10D0 <= cp && cp <= 0x10F6) || cp == 0x1100 ||
         (0x1102 <= cp && cp <= 0x1103) || (0x1105 <= cp && cp <= 0x1107) ||
         cp == 0x1109 || (0x110B <= cp && cp <= 0x110C) ||
         (0x110E <= cp && cp <= 0x1112) || cp == 0x113C || cp == 0x113E ||
         cp == 0x1140 || cp == 0x114C || cp == 0x114E || cp == 0x1150 ||
         (0x1154 <= cp && cp <= 0x1155) || cp == 0x1159 ||
         (0x115F <= cp && cp <= 0x1161) || cp == 0x1163 || cp == 0x1165 ||
         cp == 0x1167 || cp == 0x1169 || (0x116D <= cp && cp <= 0x116E) ||
         (0x1172 <= cp && cp <= 0x1173) || cp == 0x1175 || cp == 0x119E ||
         cp == 0x11A8 || cp == 0x11AB || (0x11AE <= cp && cp <= 0x11AF) ||
         (0x11B7 <= cp && cp <= 0x11B8) || cp == 0x11BA ||
         (0x11BC <= cp && cp <= 0x11C2) || cp == 0x11EB || cp == 0x11F0 ||
         cp == 0x11F9 || (0x1E00 <= cp && cp <= 0x1E9B) ||
         (0x1EA0 <= cp && cp <= 0x1EF9) || (0x1F00 <= cp && cp <= 0x1F15) ||
         (0x1F18 <= cp && cp <= 0x1F1D) || (0x1F20 <= cp && cp <= 0x1F45) ||
         (0x1F48 <= cp && cp <= 0x1F4D) || (0x1F50 <= cp && cp <= 0x1F57) ||
         cp == 0x1F59 || cp == 0x1F5B || cp == 0x1F5D ||
         (0x1F5F <= cp && cp <= 0x1F7D) || (0x1F80 <= cp && cp <= 0x1FB4) ||
         (0x1FB6 <= cp && cp <= 0x1FBC) || cp == 0x1FBE ||
         (0x1FC2 <= cp && cp <= 0x1FC4) || (0x1FC6 <= cp && cp <= 0x1FCC) ||
         (0x1FD0 <= cp && cp <= 0x1FD3) || (0x1FD6 <= cp && cp <= 0x1FDB) ||
         (0x1FE0 <= cp && cp <= 0x1FEC) || (0x1FF2 <= cp && cp <= 0x1FF4) ||
         (0x1FF6 <= cp && cp <= 0x1FFC) || cp == 0x2126 ||
         (0x212A <= cp && cp <= 0x212B) || cp == 0x212E ||
         (0x2180 <= cp && cp <= 0x2182) || (0x3041 <= cp && cp <= 0x3094) ||
         (0x30A1 <= cp && cp <= 0x30FA) || (0x3105 <= cp && cp <= 0x312C) ||
         (0xAC00 <= cp && cp <= 0xD7A3);
});
C Ideographic = u8cp([](auto cp) {
  return (0x4E00 <= cp && cp <= 0x9FA5) || cp == 0x3007 ||
         (0x3021 <= cp && cp <= 0x3029);
});
C CombiningChar = u8cp([](auto cp) {
  return (0x300 <= cp && cp <= 0x345) || (0x360 <= cp && cp <= 0x361) ||
         (0x483 <= cp && cp <= 0x486) || (0x591 <= cp && cp <= 0x5A1) ||
         (0x5A3 <= cp && cp <= 0x5B9) || (0x5BB <= cp && cp <= 0x5BD) ||
         cp == 0x5BF || (0x5C1 <= cp && cp <= 0x5C2) || cp == 0x5C4 ||
         (0x64B <= cp && cp <= 0x652) || cp == 0x670 ||
         (0x6D6 <= cp && cp <= 0x6DC) || (0x6DD <= cp && cp <= 0x6DF) ||
         (0x6E0 <= cp && cp <= 0x6E4) || (0x6E7 <= cp && cp <= 0x6E8) ||
         (0x6EA <= cp && cp <= 0x6ED) || (0x901 <= cp && cp <= 0x903) ||
         cp == 0x93C || (0x93E <= cp && cp <= 0x94C) || cp == 0x94D ||
         (0x951 <= cp && cp <= 0x954) || (0x962 <= cp && cp <= 0x963) ||
         (0x981 <= cp && cp <= 0x983) || cp == 0x9BC || cp == 0x9BE ||
         cp == 0x9BF || (0x9C0 <= cp && cp <= 0x9C4) ||
         (0x9C7 <= cp && cp <= 0x9C8) || (0x9CB <= cp && cp <= 0x9CD) ||
         cp == 0x9D7 || (0x9E2 <= cp && cp <= 0x9E3) || cp == 0xA02 ||
         cp == 0xA3C || cp == 0xA3E || cp == 0xA3F ||
         (0xA40 <= cp && cp <= 0xA42) || (0xA47 <= cp && cp <= 0xA48) ||
         (0xA4B <= cp && cp <= 0xA4D) || (0xA70 <= cp && cp <= 0xA71) ||
         (0xA81 <= cp && cp <= 0xA83) || cp == 0xABC ||
         (0xABE <= cp && cp <= 0xAC5) || (0xAC7 <= cp && cp <= 0xAC9) ||
         (0xACB <= cp && cp <= 0xACD) || (0xB01 <= cp && cp <= 0xB03) ||
         cp == 0xB3C || (0xB3E <= cp && cp <= 0xB43) ||
         (0xB47 <= cp && cp <= 0xB48) || (0xB4B <= cp && cp <= 0xB4D) ||
         (0xB56 <= cp && cp <= 0xB57) || (0xB82 <= cp && cp <= 0xB83) ||
         (0xBBE <= cp && cp <= 0xBC2) || (0xBC6 <= cp && cp <= 0xBC8) ||
         (0xBCA <= cp && cp <= 0xBCD) || cp == 0xBD7 ||
         (0xC01 <= cp && cp <= 0xC03) || (0xC3E <= cp && cp <= 0xC44) ||
         (0xC46 <= cp && cp <= 0xC48) || (0xC4A <= cp && cp <= 0xC4D) ||
         (0xC55 <= cp && cp <= 0xC56) || (0xC82 <= cp && cp <= 0xC83) ||
         (0xCBE <= cp && cp <= 0xCC4) || (0xCC6 <= cp && cp <= 0xCC8) ||
         (0xCCA <= cp && cp <= 0xCCD) || (0xCD5 <= cp && cp <= 0xCD6) ||
         (0xD02 <= cp && cp <= 0xD03) || (0xD3E <= cp && cp <= 0xD43) ||
         (0xD46 <= cp && cp <= 0xD48) || (0xD4A <= cp && cp <= 0xD4D) ||
         cp == 0xD57 || cp == 0xE31 || (0xE34 <= cp && cp <= 0xE3A) ||
         (0xE47 <= cp && cp <= 0xE4E) || cp == 0xEB1 ||
         (0xEB4 <= cp && cp <= 0xEB9) || (0xEBB <= cp && cp <= 0xEBC) ||
         (0xEC8 <= cp && cp <= 0xECD) || (0xF18 <= cp && cp <= 0xF19) ||
         cp == 0xF35 || cp == 0xF37 || cp == 0xF39 || cp == 0xF3E ||
         cp == 0xF3F || (0xF71 <= cp && cp <= 0xF84) ||
         (0xF86 <= cp && cp <= 0xF8B) || (0xF90 <= cp && cp <= 0xF95) ||
         cp == 0xF97 || (0xF99 <= cp && cp <= 0xFAD) ||
         (0xFB1 <= cp && cp <= 0xFB7) || cp == 0xFB9 ||
         (0x20D0 <= cp && cp <= 0x20DC) || cp == 0x20E1 ||
         (0x302A <= cp && cp <= 0x302F) || cp == 0x3099 || cp == 0x309A;
});
C Digit = u8cp([](auto cp) {
  return (0x30 <= cp && cp <= 0x39) || (0x660 <= cp && cp <= 0x669) ||
         (0x6F0 <= cp && cp <= 0x6F9) || (0x966 <= cp && cp <= 0x96F) ||
         (0x9E6 <= cp && cp <= 0x9EF) || (0xA66 <= cp && cp <= 0xA6F) ||
         (0xAE6 <= cp && cp <= 0xAEF) || (0xB66 <= cp && cp <= 0xB6F) ||
         (0xBE7 <= cp && cp <= 0xBEF) || (0xC66 <= cp && cp <= 0xC6F) ||
         (0xCE6 <= cp && cp <= 0xCEF) || (0xD66 <= cp && cp <= 0xD6F) ||
         (0xE50 <= cp && cp <= 0xE59) || (0xED0 <= cp && cp <= 0xED9) ||
         (0xF20 <= cp && cp <= 0xF29);
});
C Extender = u8cp([](auto cp) {
  return cp == 0xB7 || cp == 0x2D0 || cp == 0x2D1 || cp == 0x387 ||
         cp == 0x640 || cp == 0xE46 || cp == 0xEC6 || cp == 0x3005 ||
         (0x3031 <= cp && cp <= 0x3035) || (0x309D <= cp && cp <= 0x309E) ||
         (0x30FC <= cp && cp <= 0x30FE);
});

// Letter         ::=  BaseChar | Ideographic
C Letter = BaseChar | Ideographic;

C Char = anyof<0x9, 0xA, 0xD>() | u8cp([](auto cp) {
           return (0x20 <= cp && cp <= 0xD7FF) ||
                  (0xE000 <= cp && cp <= 0xFFFD) ||
                  (0x10000 <= cp && cp <= 0x10FFFF);
         });

C S = many1(anyof<0x20, 0x9, 0xD, 0xA>());

// Eq           ::=  S? '=' S?
C Eq = opt(S) & "=" & opt(S);

// NameChar  ::=  Letter | Digit |  '.' | '-' | '_' | ':' |  CombiningChar |
// Extender
C NameChar =
    Letter | Digit | anyof<'.', '-', '_', ':'>() | CombiningChar | Extender;

// Name      ::=  (Letter | '_' | ':') (NameChar)*
C Name = (Letter | anyof<'_', ':'>()) & many0(NameChar);
C prolog = str("");
C Misc = str("");

C digit = u8cp([](auto cp) { return '0' <= cp && cp <= '9'; });
C hexdigit = digit | u8cp([](auto cp) {
               return ('a' <= cp && cp <= 'f') || ('A' <= cp && cp <= 'F');
             });

// CharRef	   ::=   	'&#' [0-9]+ ';' | '&#x' [0-9a-fA-F]+ ';'
C CharRef = "&#" & digit & ";" | "&#x" & hexdigit & ";";

C EntityRef = "&" & Name & ";";

// Reference    ::=  EntityRef | CharRef
C Reference = EntityRef | CharRef;

// AttValue       ::=  '"' ([^<&"] | Reference)* '"'
//                 |  "'" ([^<&'] | Reference)* "'"
C AttValue = "\"" & many0(noneof<'<', '&', '"'>() | Reference) & "\"" |
             "'" & many0(noneof<'<', '&', '\''>() | Reference) & "'";

// Attribute     ::=  Name Eq AttValue
C Attribute = Name & Eq & AttValue;

// EmptyElemTag  ::=  '<' Name (S Attribute)* S? '/>'
C EmptyElemTag = "<" & Name & many0(S & Attribute) & opt(S) & "/>";

// STag          ::=  '<' Name (S Attribute)* S? '>'
C STag = "<" & Name & many0(S & Attribute) & opt(S) & ">";

// ETag          ::=  '</' Name S? '>'
C ETag = "</" & Name & opt(S) & ">";

// CharData  ::=  [^<&]* - ([^<&]* ']]>' [^<&]*)  wtf! how to substract one
// from / many ? C CharData = many0{nchr<'<', '&'>{}};

C Comment = str("");
C CDSect = str("");
C PI = str("");

// struct element {
//  M()(const char *in, size_t avail) {
//    // content       ::=  CharData?
//    //                   ((element | Reference | CDSect | PI | Comment)
//    //                   CharData?)*
//    auto content =
//        opt(CharData) &
//        many((*this | Reference | CDSect | PI | Comment) & opt(CharData));
//    (EmptyElemTag | STag & content & ETag)(in, avail, o);
//  }
//};
//
//// document  ::=  prolog element Misc*
// C document = prolog & element{} & many(Misc);
//
} // namespace parsec::xml

#include <iostream>

auto main() -> int {
  using namespace parsec;
  auto run = [](const char *in, const auto &parser) {
    size_t Size = 0;
    while (in[Size] != '\0')
      Size++;
    size_t pos = 0;
    parser(
        [&](int x) {
          if (x < 0)
            std::cout << "error: " << x << "\n";
          else {
            pos += x;
            std::cout << "[" << std::string_view(in, pos) << "] [" << in + pos
                      << "]\n";
          }
        },
        in, Size);
  };

  run("<Tag n =    'abo' />", xml::EmptyElemTag);
}

