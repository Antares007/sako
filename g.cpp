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

template <size_t N = 10, typename O>
constexpr inline void map(O o, git_repository *r, const git_oid *id) {
  if constexpr (N == 0)
    o(-1);
  else
    purry{[&](auto o, git_treebuilder *bld, git_tree *source) {
            purry{
                [&](auto o, const char *a, const git_oid *b, git_filemode_t c) {
                  if (c == git::TREE)
                    map<N - 1>(_o_{[o](int err) { o(err); },
                                   [a, bld](const git_oid *oid) {
                                     git_treebuilder_insert(
                                         nullptr, bld,
                                         (std::string("A ") + a).c_str(), oid,
                                         git::TREE);
                                   }},
                               r, b);
                  else
                    git_treebuilder_insert(nullptr, bld, a, b, c);
                },
                git::ls ^ source}(o);
            git::treebuilder_write(o, bld);
          },
          git::treebuilder_new ^ r ^ nullptr, git::tree_lookup ^ r ^ id}(o);
}
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
template <typename F> rec_(F) -> rec_<F>;

int main() {
  git_libgit2_init();

  auto pith = git::repository_open ^ "." | +[](out o, git_repository *r) {
    o("ABO");
    constexpr auto rez = fib(8);
    o([&](auto o) { o(rez); });

    auto step1 = git::tree_bark{[](auto self, auto o, auto r, auto tree) {
      o(git::ls ^ tree | [&](auto o, auto name, auto oid, auto mode) {
        if (mode == git::TREE)
          o(self ^ (git::tree_lookup ^ r ^ oid) | [&](auto o, auto oid) {
            o((std::string("A ") + name).c_str(), oid, git::TREE);
          });
        else
          o(name, oid, mode);
      });
    }};

    o(step1 ^ r ^
          (git::tree_lookup ^ r ^
           (git::index_write_tree ^ (git::repository_index ^ r))) |
      [](auto o, git_oid *id) { o(git_oid_tostr_s(id)); });

    auto toid = git::index_write_tree ^ (git::repository_index ^ r) |
                [&](auto o, auto id) { map(o, r, id); };
    toid(out{});
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
