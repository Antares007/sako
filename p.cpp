#include "parsec.hpp"
#include "xml.hpp"
#include <iostream>
using namespace parsec;
using namespace parsec::xml;

int main() {
  //
  auto text = "ა";

  (Name ^ text)(_o_{[](error_ray *, int err) { //
                      std::cout << "error: " << err << '\n';
                    },
                    [](int len) { //
                      std::cout << "len: " << len << '\n';
                    }});
  return 9;
}
