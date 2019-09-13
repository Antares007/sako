#include "git.h"
#include <cstdio>
#include <git2.h>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

namespace git {

lr::LR<UPtr<git_repository>> open(const char *path) {
  static int initCount = git_libgit2_init();
  if (initCount < 1)
    return lr::L{"initCount < 1"};
  git_repository *repo;
  if (git_repository_open(&repo, path) < 0)
    return lr::L{"git_repository_open(&repo, path) < 0"};
  return UPtr<git_repository>(repo, D(git_repository_free));
}

Bark::Bark(UPtr<git_repository> &&repo)
    : repo(std::forward<UPtr<git_repository>>(repo)) {}
void Bark::Ray::operator()(Name n, Mode, lr::LR<Id>) const {
  std::cout << n.get() << &this->bark.repo << '\n';
}
lr::LR<Id> Bark::operator()(Id, Pith) const { return id = git_oid{}; }

lr::LR<Id> Bark::operator()(Pith pith) const {
  auto ray = Bark::Ray(std::vector<Entry>(), *this);
  pith(std::move(ray));
  return id = git_oid{};
}
lr::LR<int> lookup(const UPtr<git_repository> &repo, const git_oid &oid) {
  git_tree *tree = NULL;
  int error = git_tree_lookup(&tree, &*repo, &oid);
  git_tree_free(tree);
  return 42;
}
// void test() {
//  auto repo = open(".");
//  lr::map(
//      [](UPtr<git_repository> &repo) {
//        auto oid = git_oid{};
//        git_tree *tree = NULL;
//        int error = git_tree_lookup(&tree, &*repo, &oid);
//      },
//      std::move(repo));
//}

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
