#pragma once
#include "mb.hpp"
// clang-format off
namespace grammar {
RG ( plus   )(b[0] == '+' && b[1] != '=' && b[1] != '+' ? 1 : -1);
RG ( mul    )(b[0] == '*' && b[1] != '=' ? 1 : -1);
RG ( lparen )(b[0] == '(' ? 1 : -1);
RG ( rparen )(b[0] == ')' ? 1 : -1);
RGB( id     )(o = 0; for (; 'a' <= b[o] && b[o] <= 'z'; ++o); o = o ? o : -1;);
RG ( a      )(b[0] == 'a' ? 1 : -1);
RG ( b      )(b[0] == 'b' ? 1 : -1);
RGB( Є      )((void(b)); o = 0;);
RG ( dollar )(-b[0]);
namespace E41 {
v_( E )(L2(L3( E{},      plus{}, T{}      ), 
           L1( T{}                        )))
v_( T )(L2(L3( T{},      mul{},  F{}      ), 
           L1( F{}                        )))_v
v_( F )(L2(L3( lparen{}, E{},    rparen{} ), 
           L1( id{}                       )))_v _v
}
namespace E43 {
v_( E )(L4(L3( E{},      plus{}, E{}      ),
           L3( E{},      mul{},  E{}      ),
           L3( lparen{}, E{},    rparen{} ),
           L1( id{}                       )))_v
}
namespace ll_k_problem {
v_( S )(L2(L2( S{}, A{} ), 
           L1( Є{}      )))
v_( A )(L1(L1( a{}      )))_v _v
}
namespace aabb {
v_( S )(L1(L2( A{}, A{} )))
v_( A )(L2(L2( a{}, A{} ), 
           L1( b{}      )))_v _v
} 
namespace aabb2 {
v_( S )(
    L2(
      L2( A{}, A{} ),
      L1( mul{} )
      )
    )
v_( A )(
    L2(
      L1( B{} ),
      L1( plus{} )
      )
    )_v
v_( B )(
    L2(
      L2( a{}, A{} ),
      L1( b{} )
      )
    )_v _v
}
}
/*
S   S
AA  AA
A+  aAA
B+  aBA
aA+ abA
aB+ ab+
ab+
  */
