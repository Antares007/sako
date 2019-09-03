#include "git.h"
#include "lr.h"
#include "overloaded.h"
#include <iostream>
#include <string>
#include <utility>

void test()
{
    auto see = lr::LR<std::string, std::pair<int, float>>("1");

    std::visit(lr::overloaded{
                   [](std::pair<int, float> &) {},
                   [](std::string &) {},
                   [](lr::L &) {},
               },
               see);
}
