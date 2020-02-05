#include "lalr/mb.hpp"
#include "purry.hpp"
#include <iostream>
struct h;
struct t;
#define L(x) static_cast<x *>(nullptr)
struct b {
  int &i;
  void operator()(const auto &o) const {
    o(L(h), i++);
    return o(L(t), b{i});
  }
};
struct p {
  void operator()(h *, int i) const {
    if (i % 1000 == 0)
      std::cout << i << ' ';
    return;
  }
  void operator()(t *, const auto &tail) const { tail(p{}); }
};
int main() {
  int i = 0;
  b{i}(p{});
  return 9;
}
