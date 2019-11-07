#include "main.hpp"

auto main() -> int {
  using namespace parsec;
  auto run = [](const char *in, const auto &parser) {
    size_t Size = 0;
    while (in[Size] != '\0')
      Size++;
    size_t pos = 0;
    parser(
        [&](int x) {
          if (x < 0)
            std::cout << "error: " << x << "\n";
          else {
            pos += x;
            std::cout << "[" << std::string_view(in, pos) << "] [" << in + pos
                      << "]\n";
          }
        },
        in, Size);
  };

  //  run("<Tag n =    'abo' />", xml::EmptyElemTag);
}

