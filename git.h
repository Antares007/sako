#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <memory>
#include <type_traits>
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
  explicit D(Deleter f) noexcept : fn(f) {}
  void operator()(T *t) const noexcept { fn(t); }

private:
  Deleter fn;
};
template <typename T> using UPtr = std::unique_ptr<T, D<T>>;

using Repo = UPtr<git_repository>;
using Tree = UPtr<git_tree>;
using Builder = UPtr<git_treebuilder>;

lr::LR<Repo> open(const char *);
lr::LR<Builder> makeBulder(const Repo &);
lr::LR<TreeId> writeTree(const Builder &);
lr::LR<Tree> lookup(const UPtr<git_repository> &, const TreeId &);
std::vector<Entry> getEntries(const Tree &);

struct O {
  O(const Builder &);

private:
  const Builder &builder;
};

struct I {
  I(const Repo &r) : repo(r) {}

private:
  const Repo &repo;
};

template <typename Pith> struct Bark {
  template <typename U,
            typename = std::enable_if_t<std::is_invocable_r_v<void, U, O, I>>>
  Bark(U &&u) : pith(std::forward<U>(u)) {}

  lr::LR<TreeId> operator()(const Repo &repo) const {
    return lr::flatMap(
        [&](const Builder &builder) {
          pith(O{builder}, I{repo});
          return writeTree(builder);
        },
        makeBulder(repo));
  }

private:
  Pith pith;
};
template <typename... Pith> Bark(Pith...)->Bark<Pith...>;

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