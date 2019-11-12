#include "git.hpp"
#include "zip.hpp"

#ifndef NVIM

#include <iostream>
#include <string>

constexpr inline auto diff = [](auto o, git_tree *lhs, git_tree *rhs) { //
  const size_t rc = git_tree_entrycount(rhs);
  const size_t lc = git_tree_entrycount(lhs);
  size_t li = 0;
  size_t ri = 0;
  while (li < lc && ri < rc) {
    const auto le = git_tree_entry_byindex(lhs, li);
    const auto re = git_tree_entry_byindex(rhs, ri);
    const auto rez = git_tree_entry_cmp(le, re);
    if (rez == 0) {
      if (git_tree_entry_filemode(le) != git_tree_entry_filemode(re) ||
          git_oid_cmp(git_tree_entry_id(le), git_tree_entry_id(re))) {
        o(le, re);
      }
      li++;
      ri++;
    } else if (rez < 0) {
      o(le, rez);
      li++;
    } else {
      o(rez, re);
      ri++;
    }
  }
  while (li < lc)
    o(git_tree_entry_byindex(lhs, li++), -1);
  while (ri < rc)
    o(1, git_tree_entry_byindex(rhs, ri++));
};

constexpr inline auto map = [](git_tree *tree) {
  return git::tree_bark{[=](auto o, auto r) {
    const auto ec = git_tree_entrycount(tree);

    constexpr auto ls = [&](auto ls, const size_t i = 0) {
      if (i >= ec)
        return;

      const auto e = git_tree_entry_byindex(tree, i);
      const auto mode = git_tree_entry_filemode(e);

      if (mode == 0100644 &&
          std::string_view(git_tree_entry_name(e)).ends_with(".xlsx"))
        git::blob_lookup(
            _o_{o,
                [&](git_blob *blob) {
                  auto buff = git_blob_rawcontent(blob);
                  size_t size = git_blob_rawsize(blob);
                  o(git_tree_entry_name(e), git::tree_bark{[&](auto o) {
                      unzip(
                          _o_{o,
                              [&](std::string_view name, auto &&p) {
                                p(_o_{o, [&](const void *buff, size_t size) {
                                        o(name, buff, size);
                                      }});
                              }},
                          buff, size);

                      ls(ls, i + 1);
                    }});
                }},
            r, git_tree_entry_id(e));
      else if (mode == 040000)
        ;
    };
    ls(ls);
  }};
};
auto main() -> int {
  git_libgit2_init();
  pin{[](auto o, git_repository *r) {
        const auto pcommitoid = git::reference_name_to_id ^ r ^ "HEAD";
        const auto ptreeoid =
            git::index_write_tree ^ (git::repository_index ^ r);

        pin{[&](auto o, auto treeoid, auto commitoid) {
              auto ctree =
                  git::commit_tree ^ (git::commit_lookup ^ r ^ commitoid);
              auto tree = git::tree_lookup ^ r ^ treeoid;
              auto f = [=] {
                auto log = [](auto m, auto l, auto r) {
                  std::cout
                      << m << std::oct << (int)git_tree_entry_filemode(l) << " "
                      << (int)git_tree_entry_filemode(r) << " "
                      << std::string_view(git_oid_tostr_s(git_tree_entry_id(l)),
                                          5)
                      << " "
                      << std::string_view(git_oid_tostr_s(git_tree_entry_id(r)),
                                          5)
                      << " " << git_tree_entry_name(l) << "\n";
                };
                pin{diff, ctree,
                    tree}(_o_{[&](auto l, auto r) { log("~", l, r); },
                              [&](int, auto e) { log("+", e, e); },
                              [&](auto e, int) { log("-", e, e); },
                              [&](int i) { o(i); }});
              };
              f();
              std::cout << "tree  : " << git_oid_tostr_s(treeoid) << "\n";
              std::cout << "commit: " << git_oid_tostr_s(commitoid) << "\n";
              o(99);
            },
            ptreeoid, pcommitoid}(
            _o_{o, [](git_oid *id) {
                  std::cout << "\n\ntree: " << git_oid_tostr_s(id) << "\n\n";
                }});
      },
      git::repository_open ^
          "."}(_o_{[](int err) { exit(err); }, [](auto...) {}});

  constexpr auto repo = (git::repository_open //
                         ^ ".");
  (git::blob_lookup    //
   ^ repo              //
   ^ (git::oid_fromstr //
      ^ "080ca003cef9e73967ff818672c3b15e26fe0817"))(
      _o_{[](int err) { std::cout << "bbb" << err << "\n"; },
          [](git_blob *blob) {
            auto buff = git_blob_rawcontent(blob);
            size_t size = git_blob_rawsize(blob);
            unzip(
                _o_{[](int err) { std::cout << err << "\n"; },
                    [](std::string_view name, auto &&p) {
                      p(_o_{[&](auto err) { std::cout << err << "\n"; },
                            [&](auto, auto size) {
                              std::cout << name << " - " << size << "\n";
                              // << std::string_view(content, size) << "\n";
                            }});
                    }},
                buff, size);
            std::cout << buff << " - " << size << "aaa\n";
          }});

  return 3;
}
#endif
