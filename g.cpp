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

constexpr inline auto map =
    rec{[](auto, auto o, git_repository *r, git_tree *tree) {
      const auto ec = git_tree_entrycount(tree);
      auto ls = [&](auto ls, const size_t i = 0) {
        if (i >= ec)
          return;
        const auto e = git_tree_entry_byindex(tree, i);
        const auto mode = git_tree_entry_filemode(e);
        const auto name = git_tree_entry_name(e);
        if (mode == 0100644 && std::string_view(name).ends_with(".xlsx")) {
          o(std::string(name) + " <- ");
          (git::treebuilder_new ^ r ^ nullptr |
           [&](auto o, git_treebuilder *bld) {
             (
                 git::blob_lookup ^ r ^ git_tree_entry_id(e) |
                 [](auto o, git_blob *blob) {
                   o(git_blob_rawcontent(blob), git_blob_rawsize(blob));
                 } |
                 unzip |
                 [&](auto o, auto n, auto b, auto s) {
                   (git::blob_create_frombuffer ^ r ^ b ^ s | [&](auto,
                                                                  git_oid *id) {
                     git_treebuilder_insert(
                         nullptr, bld, std::string(n).c_str(), id, git::BLOB);
                   })(o);
                 })(o);
             // git::treebuilder_write;
           })(o);

        } else if (mode == 040000)
          ;
        ls(ls, i + 1);
      };
      ls(ls);
    }};

auto main() -> int {
  git_libgit2_init();
  pin{
    [](auto o, git_repository *r) {
      const auto ptreeoid = git::index_write_tree ^ (git::repository_index ^ r);
      const auto px = pin{map, r, (git::tree_lookup ^ r ^ ptreeoid)};
      const auto b = git::tree_bark{[&](auto o) {
                       o(ptreeoid | [](auto o, git_oid *id) {
                         o("hello", id, git::TREE);
                       });
                     }} ^ r |
                     [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); };

      b(_o_{[](int err) { std::cout << err << "\n"; },
            [](auto a) { std::cout << a << "\n"; }});

      px(_o_{[](int err) { std::cout << err << "\n"; },
             [](auto a) { std::cout << a << "\n"; }});

      const auto pcommitoid = git::reference_name_to_id ^ r ^ "HEAD";

      pin{[&](auto o, auto treeoid, auto commitoid) {
            auto ctree =
                git::commit_tree ^ (git::commit_lookup ^ r ^ commitoid);
            auto tree = git::tree_lookup ^ r ^ treeoid;
            auto f = [=] {
              auto log = [](auto m, auto l, auto r) {
                std::cout << m << std::oct << (int)git_tree_entry_filemode(l)
                          << " " << (int)git_tree_entry_filemode(r) << " "
                          << std::string_view(
                                 git_oid_tostr_s(git_tree_entry_id(l)), 5)
                          << " "
                          << std::string_view(
                                 git_oid_tostr_s(git_tree_entry_id(r)), 5)
                          << " " << git_tree_entry_name(l) << "\n";
              };
              pin{diff, ctree, tree}(_o_{
                  [&](auto l, auto r) { log("~", l, r); },
                  [&](int, auto e) { log("+", e, e); },
                  [&](auto e, int) { log("-", e, e); }, [&](int i) { o(i); }});
            };
            f();
            std::cout << "tree  : " << git_oid_tostr_s(treeoid) << "\n";
            std::cout << "commit: " << git_oid_tostr_s(commitoid) << "\n";
            o(99);
          },
          ptreeoid, pcommitoid}(_o_{o, [](git_oid *id) {
                                      std::cout
                                          << "\n\ntree: " << git_oid_tostr_s(id)
                                          << "\n\n";
                                    }});
      }
      , git::repository_open ^ "."
    }(_o_{[](int err) { exit(err); }, [](auto...) {}});

    return 3;
}
#endif
