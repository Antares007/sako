#include "git_.hpp"
#include <cstdio>
#include <iostream>

struct S {
  S() noexcept { puts("S()"); }
  S(S const &) noexcept { puts("S(const S &)"); }
  S(S &&) noexcept { puts("S(S &&)"); }
  S &operator=(const S &) noexcept {
    puts("S &operator = (const S &)");
    return *this;
  }
  S &operator=(S &&) noexcept {
    puts("S &operator = (S &&)");
    return *this;
  }
  ~S() noexcept { puts("~S()"); }
};

struct aiterator {
  using difference_type = int;
  using value_type = std::tuple<std::string, abo::TreeSha>;
  using pointer = value_type *;
  using reference = value_type &;
  using iterator_category = std::input_iterator_tag;
  aiterator &operator++();
  aiterator operator++(int);
  bool operator==(aiterator other) const;
  bool operator!=(aiterator other) const;
  pointer operator*();
  reference operator->();
};
template <typename T> struct tree_iterator {
  aiterator begin() { return aiterator{}; }
  aiterator end() { return aiterator{}; }
};

NewType(TreeA, std::string, tree_iterator);

int main() {
  for (auto i : TreeA{""})
    std::cout << i << '\n';
  auto sha = abo::TreeSha{""};
}
