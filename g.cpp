#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

#ifndef NVIM
struct out {
  void operator()(int err) const { std::cerr << err << '\n'; }
  void operator()(const char *cstr) const { std::cout << cstr << '\n'; }
  void operator()(const git_oid *oid) const {
    std::cout << git_oid_tostr_s(oid) << '\n';
  }
};
int main() {
  git_libgit2_init();
  auto ooo = out{};

  auto pith = (git::repository_open ^ ".") | +[](out o, git_repository *r) {
    o("ABO");
    auto u = git::tree_bark{[&](auto, auto o, auto r, const git_tree *tree) {
               git::tree_bark{[](auto p, auto, auto r, auto source) {
                 if (source == nullptr)
                   return;
                 p([](auto) {}, r, nullptr);
               }}([](auto) {}, r, tree);

               o(git::ls ^ tree |
                 [](auto o, const char *a, const git_oid *b, git_filemode_t c) {
                   if (c == git::TREE)
                     o((std::string("A ") + a).c_str(), b, c);

                   else
                     o(a, b, c);
                 });
             }} ^
             r ^
             (git::tree_lookup ^ r ^
              (git::index_write_tree ^ (git::repository_index ^ r)));
    (u | [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); })(o);
  };

  pith(ooo);
  // if (source == nullptr)
  //  return;
  //(git::ls ^ source |
  // [&](git::R<out> o, auto a, const git_oid *b, git_filemode_t c) {
  //   if (c == git::TREE)
  //     ; // p(o, r, nullptr);
  //   //(
  //   //    git::tree_lookup ^ r ^ b |
  //   //    [&](auto o, auto source) {
  //   //      p->operator()([](auto) {}, r, source);
  //   //    } |
  //   //    [&](auto o, auto oid) {
  //   //      o((std::string("A ") + a).c_str(), oid, c);
  //   //    })(o);
  //   else
  //     o(a, b, c);
  // })(o);

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
