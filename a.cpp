#include <iostream>

int main(int c = 0) { //
  if (c < 1000) {
    std::cout << "გამარჯობა ანდრია!";
    main(c + 1);
  }
}
