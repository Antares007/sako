#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <memory>
#include <vector>

namespace git {

using Name = nt::NamedType<std::string, struct NameTag>;

using TreeId = nt::NamedType<git_oid, struct TreeIdTag>;
using BlobId = nt::NamedType<git_oid, struct BlobIdTag>;
using ExecId = nt::NamedType<git_oid, struct ExecIdTag>;
using LinkId = nt::NamedType<git_oid, struct LinkIdTag>;
using CommId = nt::NamedType<git_oid, struct CommIdTag>;

using Entry =
    std::tuple<Name, std::variant<TreeId, BlobId, ExecId, LinkId, CommId>>;

template <typename T> struct D {
  using Deleter = void (*)(T *);
  explicit D(Deleter f) noexcept : f(f) {}
  void operator()(T *t) const noexcept { f(t); }

private:
  Deleter f;
};
template <typename T> using UPtr = std::unique_ptr<T, D<T>>;

using Repo = UPtr<git_repository>;
using Tree = UPtr<git_tree>;

lr::LR<Repo> open(const char *);
lr::LR<Tree> lookup(const UPtr<git_repository> &, const TreeId &);
std::vector<Entry> getEntries(const Tree &);

struct writeConvertableTostring {
  void operator()(const Name &, std::string &&);
};
template <typename Pith> constexpr decltype(auto) bark2(Pith pith) {
  return [pith](const Repo &repo) {
    pith(overloaded{writeConvertableTostring{}, [](const Entry &) {},
                    [&repo](auto b) { b(repo); }},
         repo);
    return TreeId(git_oid{});
  };
}
} // namespace git

#endif

// struct Bark {
//   struct Ray {
//     const std::vector<Entry> &entries;
//     const Bark &bark;
//     void operator()(lr::LR<Entry>) const noexcept;
//     explicit Ray(std::vector<Entry> &&entries, const Bark &bark)
//         : entries(std::forward<std::vector<Entry>>(entries)), bark(bark) {}
//   };
//   using Pith = void (*)(Ray &&);
//   lr::LR<TreeId> operator()(TreeId, Pith) const;
//   lr::LR<TreeId> operator()(Pith) const;
//   Bark(UPtr<git_repository> &&repo);

// private:
//   UPtr<git_repository> repo;
// };