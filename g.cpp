#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

constexpr inline auto out =
    _o_{[](left_ray *, int err) { std::cerr << err << '\n'; },
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

constexpr inline auto aaa = [](auto o, auto repo, auto blob) {
  o([&](auto o, auto name_, auto buffer, auto size) {
    o([&](auto o, git_oid *id) {
      auto name = std::string(name_);
      for (size_t i = 0; i < name.size(); i++)
        if (name[i] == '/')
          name[i] = '_';
      o(name.c_str(), id, git::BLOB);
    } ^
      (git::blob_create_frombuffer ^ repo ^ buffer ^ size));
  } ^
    (unzip ^ git_blob_rawcontent(blob) ^ git_blob_rawsize(blob)));
};

struct mapPith {
  template <typename O>
  void operator()(O o, git_repository *r, const git_oid *id) const { //
    o([&](auto o, auto name, auto oid, auto mode) {
      if (mode == git::TREE)
        ;
      // o(git::tree_ring(purry{*this} ^ r ^ oid) ^ r | [&](auto o, auto oid)
      // {
      //  if (std::string_view(git_oid_tostr_s(oid)) !=
      //      "4b825dc642cb6eb9a060e54bf8d69288fbee4904")
      //    o(name, oid, git::TREE);
      //});
      else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
        o([&](auto o, auto oid) { o(name, oid, git::TREE); } ^
          (git::tree_ring(aaa ^ r ^ (git::blob_lookup ^ r ^ oid)) ^ r));
    } ^
      (git::ls ^ (git::tree_lookup ^ r ^ id)));
  }
};

int main() {
  git_libgit2_init();

  auto pith = [](auto o, git_repository *r) {
    o("ABO");
    auto treeId = git::index_write_tree ^ (git::repository_index ^ r);
    o(git::tree_ring(mapPith{} ^ r ^ treeId) ^ r);
  } ^ (git::repository_open ^ ".");

  pith(out);
  return 3;
}
