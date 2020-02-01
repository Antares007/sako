#pragma once
#include "mb.hpp"

RG(plus)(b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
RG(mul)(b[0] == '*' && b[1] != '=' ? 1 : -1);
RG(lparen)(b[0] == '(' ? 1 : -1);
RG(rparen)(b[0] == ')' ? 1 : -1);
RGB(id)
(int i = 0; while ('a' <= b[i] && b[i] <= 'z') i++; o(i > 0 ? i : -1););

v_(E)(L2(L3(E{}, plus{}, T{}), //
         L1(T{})));
v_(T)(L2(L3(T{}, mul{}, F{}), //
         L1(F{})));
v_(F)(L2(L3(lparen{}, E{}, rparen{}), //
         L1(id{})));
_v _v _v;

namespace grammar {
RG(a)(b[0] == 'a');
RGB(Є)((void(b), o(0)););
v_(S)(L2(L2(S{}, A{}), //
         L1(Є{})));
v_(A)(L1(L1(a{})));
_v _v
} // namespace grammar
namespace grammar_aabb {
RG(a)(b[0] == 'a' ? 1 : -1);
RG(b)(b[0] == 'b' ? 1 : -1);
v_(S)(L1(L2(A{}, A{})));
v_(A)(L2(L2(a{}, A{}), //
         L1(b{})));
_v _v
} // namespace grammar_aabb
