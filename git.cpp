#include "git.h"
#include <iostream>
#include <string>
#include <utility>

template <typename... Ts>
struct print;
void test()
{
    auto b = 4;
    auto see = lr::map(
        [&](int &a) {
            std::cout << a << '\n';
            return b + a;
        },
        lr::LR<int>(11));
    std::cout << &see << '\n';
}
