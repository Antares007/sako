#include "git.hpp"
#include "zip.hpp"

#ifndef NVIM

#include <iostream>

auto main() -> int {
  git_libgit2_init();

  static constexpr auto mainpith = [](auto o, git_repository *r,
                                      git_oid *commit_id) {
    git::commit_lookup(_o_{o,
                           [](git_commit *cid) {
                             auto m = git_commit_message(cid);
                             std::cout << m << "\n";

                             auto pid = git_commit_parent_id(cid, 0);
                             auto pc = git_commit_parentcount(cid);
                             //
                           }},
                       r, commit_id);
    /* main pith */
    o(-1);
    o(r);
    o(commit_id);
  };

  constexpr auto p = pin{mainpith, (git::repository_open ^ ".")};

  auto id = git_oid{};
  p([](auto...) {}, &id);

  constexpr auto repo = (git::repository_open //
                         ^ ".");
  (git::blob_lookup    //
   ^ repo              //
   ^ (git::oid_fromstr //
      ^ "080ca003cef9e73967ff818672c3b15e26fe0817"))(
      _o_{[](int err) { std::cout << "bbb" << err << "\n"; },
          [](git_blob *blob) {
            auto buff = git_blob_rawcontent(blob);
            size_t size = git_blob_rawsize(blob);
            unzip{buff, size}(_o_{
                [](int err) { std::cout << err << "\n"; },
                [](std::string_view name, auto &&p) {
                  p(_o_{[&](auto err) { std::cout << err << "\n"; },
                        [&](auto content, auto size) {
                          std::cout << name << " - " << size << "\n"
                                    << std::string_view(content, size) << "\n";
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
