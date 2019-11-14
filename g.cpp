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

//(git::treebuilder_new ^ r ^ nullptr |
// [&](auto o, git_treebuilder *bld) {
//   (
//       git::blob_lookup ^ r ^ git_tree_entry_id(e) |
//       [](auto o, git_blob *blob) {
//         o(git_blob_rawcontent(blob), git_blob_rawsize(blob));
//       } |
//       unzip |
//       [&](auto o, auto n, auto b, auto s) {
//         (git::blob_create_frombuffer ^ r ^ b ^ s | [&](auto,
//                                                        git_oid *id) {
//           git_treebuilder_insert(
//               nullptr, bld, std::string(n).c_str(), id, git::BLOB);
//         })(o);
//       })(o);
//   // git::treebuilder_write;
// })(o);

constexpr inline auto ls = [](auto o, git_tree *tree) {
  const auto rec = [=](auto rec, size_t i) {
    if (i-- < 1)
      return;
    const auto e = git_tree_entry_byindex(tree, i);
    o(git_tree_entry_name(e), git_tree_entry_id(e), git_tree_entry_filemode(e));
    rec(rec, i);
  };
  rec(rec, git_tree_entrycount(tree));
};

auto main() -> int {
  git_libgit2_init();
  auto see = pin{[](auto o, int a, int b, int c) { o(a + b + c); },
                 [](auto o) { o(1, 2, 6); }};
  see(_o_{[](int err) { exit(err); }, [](auto x) { std::cout << x << "\n"; }});

  auto pith =
      git::repository_open ^ "." | [](auto o, git_repository *r) {
        const auto ptreeoid =
            git::index_write_tree ^ (git::repository_index ^ r);

        (git::tree_bark{[&](auto o, auto r) {
           o(git::tree_lookup ^ r ^ ptreeoid | ls |
             [](auto o, const char *a, const git_oid *b, git_filemode_t c) {
               o(a, b, c);
             });
         }} ^ r |
         [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); })(o);

      };
  pith(_o_{[](int err) { exit(err); }, [](auto x) { std::cout << x << "\n"; }});
  return 3;
}
#endif
