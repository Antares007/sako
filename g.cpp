#include "git.hpp"
#include "zip.hpp"
#include <iostream>
#include <string>

constexpr inline auto out =
    rays{[](error_ray *, int err) { std::cerr << err << '\n'; },
         [](const char *cstr) { std::cout << cstr << '\n'; },
         [](const git_oid *oid) { std::cout << git_oid_tostr_s(oid) << '\n'; }};

constexpr inline auto unzip_pith = [](auto o, auto repo, auto blob) {
  ([&](auto o, auto name_, auto buffer, auto size) {
    ([&](auto o, git_oid *id) {
      auto name = std::string(name_);
      for (size_t i = 0; i < name.size(); i++)
        if (name[i] == '/')
          name[i] = '_';
      o(name.c_str(), id, git::BLOB);
    } ^
     (git::blob_create_frombuffer, repo, buffer, size))(o);
  } ^
   (unzip, git_blob_rawcontent(blob), git_blob_rawsize(blob)))(o);
};

template <size_t N = 3> struct mapPith {
  template <typename O>
  void operator()(O o, git_repository *r, const git_oid *id) const { //
    ([&](auto o, auto name, auto oid, auto mode) {
      if (mode == git::TREE) {
        if constexpr (N > 0)
          ([&](auto o, auto oid) {
            if (git_oid_streq(oid, "4b825dc642cb6eb9a060e54bf8d69288fbee4904"))
              o(name, oid, git::TREE);
          } ^
           (git::tree_ring, r, (mapPith<N - 1>{}, r, oid)))(o);
      } else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
        ([&](auto o, auto oid) { o(name, oid, git::TREE); } ^
         (git::tree_ring, r, (unzip_pith, r) ^ (git::blob_lookup, r, oid)))(o);
    } ^
     (git::ls ^ (git::tree_lookup, r, id)))(o);
  }
};

int main() {
  git_libgit2_init();

  auto pith = [&](auto o, git_repository *r) {
    o("ABO");
    auto treeId = git::index_write_tree ^ (git::repository_index, r);
    (git::tree_ring, r, ((mapPith{}, r) ^ treeId))(o);
  } ^ (git::repository_open, "../Downloads/2020");

  pith(out);
  return 3;
}
