#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <memory>
#include <vector>

namespace git {

using Name = nt::NamedType<std::string_view, struct NameTag>;

using Id = nt::NamedType<git_oid, struct IdTag>;
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

struct Bark {
  struct Ray {
    const std::vector<Entry> &entries;
    const Bark &bark;
    void operator()(lr::LR<Entry>) const noexcept;
    explicit Ray(std::vector<Entry> &&entries, const Bark &bark)
        : entries(std::forward<std::vector<Entry>>(entries)), bark(bark) {}
  };
  using Pith = void (*)(Ray &&);
  lr::LR<TreeId> operator()(TreeId, Pith) const;
  lr::LR<TreeId> operator()(Pith) const;
  Bark(UPtr<git_repository> &&repo);

private:
  UPtr<git_repository> repo;
};

lr::LR<UPtr<git_repository>> open(const char *);
lr::LR<UPtr<git_tree>> lookup(const UPtr<git_repository> &, const TreeId &);
std::vector<Entry> getEntries(const UPtr<git_tree> &);

} // namespace git

#endif