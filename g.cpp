#include "git.hpp"
#include "zip.hpp"

#ifndef NVIM

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

#include <iostream>
#include <string>

auto main() -> int {
  git_libgit2_init();

  auto pith = (git::repository_open ^ ".") | [](auto o, git_repository *r) {
    o("ABO");
    const auto ptreeoid = git::index_write_tree ^ (git::repository_index ^ r);
    auto u = git::tree_bark{[&](auto o, auto, const git_tree *tree) {
               git::ls ^ tree;
               o(git::ls ^ tree |
                 [](auto o, const char *a, const git_oid *b, git_filemode_t c) {
                   if (c == git::TREE)
                     o((std::string("a") + a).c_str(), b, c);
                   else
                     o(a, b, c);
                 });
             }} ^
             r ^ (git::tree_lookup ^ r ^ ptreeoid);
    (u | [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); })(o);
  };
  pith(_o_{[](int err) { exit(err); }, [](auto x) { std::cout << x << "\n"; }});
  return 3;
}
#endif

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

// inline auto test = rec{[](auto, git_oid *id) {
//  return git::tree_bark{[=](auto o, auto r, auto) {
//    o(git::tree_lookup ^ r ^ id | git::ls |
//      [&](auto o, const char *a, const git_oid *b, git_filemode_t c) {
//        if (c == git::TREE)
//          o(git::tree_lookup ^ r ^ b | git::ls);
//        else
//          o(a, b, c);
//      });
//  }};
//}};
