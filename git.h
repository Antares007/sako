#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <memory>
#include <type_traits>
#include <vector>

namespace git {

template <typename T> struct D {
  using Deleter = void (*)(T *);
  D(Deleter f) : fn(f) {}
  void operator()(T *t) const noexcept { fn(t); }

private:
  Deleter fn;
};
template <typename T> using UPtr = std::unique_ptr<T, D<T>>;

template <typename T, typename... Args>
constexpr lr::LR<UPtr<T>> make(int (*f)(T **, Args...), void (*g)(T *),
                               Args... args) {
  T *p = nullptr;
  if (f(&p, args...))
    return lr::L{""};
  return UPtr<T>(p, D(g));
}

using TreeId = nt::NamedType<git_oid, struct TreeIdTag>;
using BlobId = nt::NamedType<git_oid, struct BlobIdTag>;
using ExecId = nt::NamedType<git_oid, struct ExecIdTag>;
using LinkId = nt::NamedType<git_oid, struct LinkIdTag>;
using CommitId = nt::NamedType<git_oid, struct CommIdTag>;
using Ref = nt::NamedType<std::string, struct RefTag>;

struct TreeBark;

struct CommitBark {
  struct O {
    void operator()(const TreeId &) const noexcept;
    void operator()(const CommitId &) const noexcept;
  };
  CommitBark(const UPtr<git_repository> &rhs) : repo(rhs) {}
  const UPtr<git_repository> &repo;
  CommitId operator()(const Ref, const UPtr<git_signature> &author,
                      const UPtr<git_signature> &committer,
                      const char *message_encoding, const char *message,
                      void (*o)(const O &, const TreeBark &),
                      const TreeId &tree, size_t parent_count,
                      const git_commit **parents) const noexcept;
};

struct TreeBark : CommitBark {
  struct O {
    void operator()(const char *name, const TreeId &) const noexcept;
    void operator()(const char *name, const BlobId &) const noexcept;
    void operator()(const char *name, const ExecId &) const noexcept;
    void operator()(const char *name, const LinkId &) const noexcept;
    void operator()(const char *name, const CommitId &) const noexcept;
  };
  using CommitBark::CommitBark;
  using CommitBark::operator();
  TreeId operator()(void (*o)(const O &, const TreeBark &)) const noexcept;
};

lr::LR<size_t> lookup(const UPtr<git_repository> &, const TreeId &,
                      const TreeBark::O &);

using Name = nt::NamedType<std::string, struct NameTag>;
using Entry =
    std::tuple<Name, std::variant<TreeId, BlobId, ExecId, LinkId, CommitId>>;
lr::LR<TreeId> writeTree(const UPtr<git_treebuilder> &);
// lr::LR<UPtr<git_tree>> lookup(const UPtr<git_repository> &, const TreeId &);
std::vector<Entry> getEntries(const UPtr<git_tree> &);

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