#include "git.h"
#include <cstdio>
#include <git2.h>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

namespace git {

void D::operator()(git_repository *repo) const noexcept {
  git_repository_free(repo);
}

lr::LR<Repo> open(const char *path) {
  static int initCount = git_libgit2_init();
  if (initCount < 1)
    return lr::L{"initCount < 1"};
  git_repository *repo;
  if (git_repository_open(&repo, path) < 0)
    return lr::L{"git_repository_open(&repo, path) < 0"};
  return std::unique_ptr<git_repository, D>(repo, D());
}

Bark::Bark(Repo &&repo) : repo(std::forward<Repo>(repo)) {}
void Bark::Ray::operator()(Name n, Mode, lr::LR<Id>) const {
  std::cout << n.get() << &this->bark.repo << '\n';
}
lr::LR<Id> Bark::operator()(Id, Pith) const { return id = git_oid{}; }

lr::LR<Id> Bark::operator()(Pith pith) const {
  auto ray = Bark::Ray(std::vector<Entry>(), *this);
  pith(std::move(ray));
  return id = git_oid{};
}

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
