#include "git.h"
#include "lr.h"
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
            return b + a;
        },
        lr::LR<int>(1));
}
