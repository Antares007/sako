#include "git.h"
#include "git2.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

namespace git {

void Bark::Ray::operator()(Name n, Mode, lr::LR<Id>) const {
  std::cout << n.get() << '\n';
};
lr::LR<Id> Bark::operator()(Id, Pith) const { return id = git_oid{}; };

lr::LR<Id> Bark::operator()(Pith pith) const {

  pith(Bark::Ray{std::vector<Entry>{std::make_tuple(
                     name = "file.txt", mode = BLOB, id = git_oid{})},
                 *this});

  return id = git_oid{};
};

} // namespace git

// struct S {
//   S() noexcept { puts("S()"); }
//   S(S const &) noexcept { puts("S(const S &)"); }
//   S(S &&) noexcept { puts("S(S &&)"); }
//   S &operator=(const S &) noexcept {
//     puts("S &operator = (const S &)");
//     return *this;
//   }
//   S &operator=(S &&) noexcept {
//     puts("S &operator = (S &&)");
//     return *this;
//   }
//   ~S() noexcept { puts("~S()"); }
// };
