#include "git.h"
#include "lr.h"
#include <iostream>
#include <string>
#include <utility>
template <typename... Ts>
struct print;
void test()
{
    auto see = lr::map(overloaded{[](int &a) {
                           return a;
                       }},
                       lr::LR<int>(1));
}
