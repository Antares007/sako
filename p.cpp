#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>

int main() {
  //
  auto text = "abo";
  auto parser = parsec::str{"a"} & parsec::str{"b"} &
                (parsec::str{"c"} | parsec::str{"o"});

  parser(_o_{[](error_ray *, int err) { //
               std::cout << err << '\n';
             },
             [](int len) { //
               std::cout << len << '\n';
             }},
         text);
  return 9;
}
