#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

constexpr inline auto out =
    _o_{[](error_ray *, int err) { std::cerr << err << '\n'; },
        [](const char *cstr) { std::cout << cstr << '\n'; },
        [](const git_oid *oid) { std::cout << git_oid_tostr_s(oid) << '\n'; }};

constexpr inline int (*fib)(int) = +[](int n) {
  return n < 2 ? n : fib(n - 2) + fib(n - 1);
};

constexpr inline auto unzip_pith = OB()(auto repo, auto blob) {
  o(OB(&)(auto name_, auto buffer, auto size) {
    o(OB(&)(git_oid * id) {
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

template <size_t N = 3> struct mapPith {
  MOB()(git_repository *r, const git_oid *id) { //
    o(OB(&)(auto name, auto oid, auto mode) {
      if (mode == git::TREE) {
        if constexpr (N > 0)
          o(OB(&)(auto oid) {
            if (std::string_view(git_oid_tostr_s(oid)) !=
                "4b825dc642cb6eb9a060e54bf8d69288fbee4904")
              o(name, oid, git::TREE);
          } ^
            (git::tree_ring(mapPith<N - 1>{} ^ r ^ oid) ^ r));
      } else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
        o(OB(&)(auto oid) { o(name, oid, git::TREE); } ^
          (git::tree_ring(unzip_pith ^ r ^ (git::blob_lookup ^ r ^ oid)) ^ r));
    } ^
      (git::ls ^ (git::tree_lookup ^ r ^ id)));
  }
};

int main() {
  git_libgit2_init();

  auto pith = OB()(git_repository * r) {
    o("ABO");
    auto treeId = git::index_write_tree ^ (git::repository_index ^ r);
    o(git::tree_ring(mapPith{} ^ r ^ treeId) ^ r);
  }
  ^(git::repository_open ^ "../Downloads/2020");

  pith(out);
  return 3;
}
