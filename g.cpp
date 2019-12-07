#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

#ifndef NVIM
constexpr inline auto out =
    _o_{[](int err) { std::cerr << err << '\n'; },
        [](const char *cstr) { std::cout << cstr << '\n'; },
        [](const git_oid *oid) { std::cout << git_oid_tostr_s(oid) << '\n'; }};

constexpr inline int (*fib)(int) = +[](int n) {
  return n < 2 ? n : fib(n - 2) + fib(n - 1);
};

struct L;

constexpr inline L *left = nullptr;

// constexpr inline auto xlsx2tree = git::tree_bark{[](auto o, auto r, auto
// blob) {
//   o(purry{unzip} ^ git_blob_rawcontent(blob) ^ git_blob_rawsize(blob) |
//     [&](auto o, auto n, auto b, auto s) {
//       o(git::blob_create_frombuffer ^ r ^ b ^ s | [&](auto o, git_oid *id) {
//         auto name = std::string(n);
//         for (size_t i = 0; i < name.size(); i++)
//           if (name[i] == '/')
//             name[i] = '_';
//         o(name.c_str(), id, git::BLOB);
//       });
//     });
// }};
struct mapPith {
  template <typename O>
  void operator()(O o, git_repository *r, const git_oid *id) const { //
    o(git::ls ^ (git::tree_lookup ^ r ^ id) |
      [&](auto o, auto name, auto oid, auto mode) {
        auto aaa = purry{[](auto o, auto r, auto blob) {
          o(purry{unzip} ^ git_blob_rawcontent(blob) ^ git_blob_rawsize(blob) |
            [&](auto o, auto n, auto b, auto s) {
              o(git::blob_create_frombuffer ^ r ^ b ^ s |
                [&](auto o, git_oid *id) {
                  auto name = std::string(n);
                  for (size_t i = 0; i < name.size(); i++)
                    if (name[i] == '/')
                      name[i] = '_';
                  o(name.c_str(), id, git::BLOB);
                });
            });
        }};
        if (mode == git::TREE)
          ;
        // o(git::tree_ring(purry{*this} ^ r ^ oid) ^ r | [&](auto o, auto oid)
        // {
        //  if (std::string_view(git_oid_tostr_s(oid)) !=
        //      "4b825dc642cb6eb9a060e54bf8d69288fbee4904")
        //    o(name, oid, git::TREE);
        //});
        else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
          o(git::tree_ring(aaa ^ r ^ (git::blob_lookup ^ r ^ oid)) ^ r |
            [&](auto o, auto oid) { o(name, oid, git::TREE); });
      });
  }
};

int main() {
  git_libgit2_init();

  auto pith = git::repository_open ^ "." | [](auto o, git_repository *r) {
    o("ABO");
    o([&](auto o) {
      constexpr auto x = fib(8);
      o(x);
    });
    auto treeId = git::index_write_tree ^ (git::repository_index ^ r);
    o(git::tree_ring(purry{mapPith{}} ^ r ^ treeId) ^ r);
    // o(git::tree_bark([](auto) {}) ^ r);

    /*
    auto step1 = git::tree_bark{[&](auto self, auto o, auto r, auto treeid) {
      o(git::ls ^ (git::tree_lookup ^ r ^ treeid) | [&](auto o, auto name,
                                                        auto oid, auto mode) {
        if (mode == git::TREE)
          o(self ^ r ^ oid | [&](auto o, auto oid) {
            if (std::string_view(git_oid_tostr_s(oid)) !=
                "4b825dc642cb6eb9a060e54bf8d69288fbee4904")
              o(name, oid, git::TREE);
          });
        else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
          o(purry{xlsx2tree} ^ r ^ (git::blob_lookup ^ r ^ oid) |
            [&](auto o, auto oid) { o(name, oid, git::TREE); });
      });
    }};
      */
    // o(step1 ^ r ^ (treeId) |
    //  [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); });
  };

  pith(out);
  auto l = [](auto o) {
    if (true)
      o(left);
    else
      o(1);
  };
  l(_o_{[&](L *) { std::cout << "left\n"; },
        [](int) { std::cout << "right\n"; }});

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
