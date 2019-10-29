#include "git.hpp"
#include "zip.hpp"

template <typename... T> struct print;

#ifndef NVIM

#include <iostream>

auto main() -> int {
  git_libgit2_init();
  auto aaa = git::blob_lookup        //
             ^ (git::repository_open //
                ^ ".")               //
             ^ (git::oid_fromstr     //
                ^ "080ca003cef9e73967ff818672c3b15e26fe0817");

  //   pin<
  //     pin<
  //       lift<git_blob *, git_repository *, const git_oid *>,
  //       pin<lift<git_repository *, const char *>, const char *>
  //     >,
  //     pin<lift<git_oid, const char *>, const char *>
  //   >
  aaa(_o_{[](int err) { std::cout << "bbb" << err << "\n"; },
          [](git_blob *blob) {
            auto buff = git_blob_rawcontent(blob);
            auto size = git_blob_rawsize(blob);
            zip::unzip(buff, size,
                       _o_{[](const char *err) { std::cout << err << "\n"; },
                           [](std::string_view name, auto &&p) {
                             p(_o_{[&](auto err) { std::cout << err << "\n"; },
                                   [&](auto, auto size) {
                                     std::cout << name << " - " << size << "\n";
                                   }});
                           }});
            std::cout << buff << " - " << size << "aaa\n";
          }});

  git::repository_open( ///
      _o_{[](int) {},
          [](git_repository *repo) {
            const auto pid = [](const auto &o) {
              git::oid_fromstr(o, "2096476c4b64612e8db373e838078ee213527476");
            };

            git::bark([&](auto o) {
              o("Aaaaaa", git::TREE, pid);
              o("Bbbb", git::TREE, pid);
            })(_o_{[](int) {},
                   [](const git_oid *id) {
                     std::cout << git_oid_tostr_s(id) << '\n';
                   }},
               repo);
          }},
      ".");
  return 3;
}
#endif
