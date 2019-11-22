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
  template <typename Pith,
            typename = std::enable_if_t<std::is_invocable_r_v<void, Pith, out>>>
  void operator()(Pith &&pith) const {
    pith(*this);
  }
};

constexpr inline int (*fib)(int) = +[](int n) {
  return n < 2 ? n : fib(n - 2) + fib(n - 1);
};

template <typename F, size_t N = 3> struct rec_ {
  F f;
  template <typename... Us> constexpr void operator()(Us &&... us) const {
    f(rec_<F, N - 1>{f}, static_cast<Us &&>(us)...);
  }
};
template <typename F> struct rec_<F, 0> {
  F f;
  template <typename... Us> constexpr void operator()(Us &&...) const {}
};
template <typename F> rec_(F)->rec_<F>;

int main() {
  git_libgit2_init();

  auto pith = git::repository_open ^ "." | +[](out o, git_repository *r) {
    o("ABO");
    o([&](auto o) { o(fib(8)); });

    auto step1 = git::tree_bark{[](auto self, auto o, auto r, auto treeid) {
      o(git::ls ^ (git::tree_lookup ^ r ^ treeid) | [&](auto o, auto name,
                                                        auto oid, auto mode) {
        if (mode == git::TREE)
          o(self ^ r ^ oid | [&](auto o, auto oid) {
            if (std::string_view(git_oid_tostr_s(oid)) !=
                "4b825dc642cb6eb9a060e54bf8d69288fbee4904")
              o(name, oid, git::TREE);
          });
        else if (mode == git::BLOB && std::string_view(name).ends_with(".xlsx"))
          o(purry{git::tree_bark{[](auto, auto o, auto r, auto blob) {
                    o(purry{unzip, git_blob_rawcontent(blob),
                            git_blob_rawsize(blob)} |
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
                  }},
                  r, git::blob_lookup ^ r ^ oid} |
            [&](auto o, auto oid) { o(name, oid, git::TREE); });
      });
    }};

    o(step1 ^ r ^ (git::index_write_tree ^ (git::repository_index ^ r)) |
      [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); });
  };

  pith(out{});

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
