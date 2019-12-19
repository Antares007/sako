#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>

int main() {
  //
  auto text = "abo";
  (parsec::str{"a"} & parsec::str{"b"} & (parsec::str{"c"} | parsec::str{"o"}))(
      _o_{[](int err) { //
            std::cout << err << '\n';
          },
          [](parsec::rez_ray *, int len) { //
            std::cout << len << '\n';
          }},
      text);
  return 9;
}
