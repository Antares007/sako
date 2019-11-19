#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

#ifndef NVIM

constexpr inline auto ring_ = git::tree_bark{[](auto o, auto r, auto source) {
  o(git::ls ^ source | [&](auto o, auto a, const git_oid *b, git_filemode_t c) {
    if (c == git::TREE)
      (
          git::tree_lookup ^ r ^ b                                          //
          | [&, bark = o.bark](auto o, auto source) { bark(o, r, source); } //
          | [&](auto o,
                auto oid) { o((std::string("A ") + a).c_str(), oid, c); })(o);
    else
      o(a, b, c);
  });
}};

int main() {
  git_libgit2_init();

  auto pith = (git::repository_open ^ ".") | [](auto o, git_repository *r) {
    o("ABO");

    (git::tree_bark{[&](auto o, auto r, auto tree) {
       ring_([](auto) {}, r, tree);
       o(git::ls ^ tree |
         [](auto o, const char *a, const git_oid *b, git_filemode_t c) {
           if (c == git::TREE)
             o((std::string("A ") + a).c_str(), b, c);

           else
             o(a, b, c);
         });
     }} ^ r ^
         (git::tree_lookup ^ r ^
          (git::index_write_tree ^ (git::repository_index ^ r))) |

     [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); })(o);
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
