#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <memory>
#include <string>
#include <vector>

namespace git {

using Name = nt::NamedType<std::string_view, struct NameTag>;

using Id = nt::NamedType<git_oid, struct IdTag>;
using TreeId = nt::NamedType<git_oid, struct TreeIdTag>;
using BlobId = nt::NamedType<git_oid, struct BlobIdTag>;
using ExecId = nt::NamedType<git_oid, struct ExecIdTag>;
using LinkId = nt::NamedType<git_oid, struct LinkIdTag>;
using CommId = nt::NamedType<git_oid, struct CommIdTag>;

enum Mode {
  UNREADABLE = 0000000,
  TREE = 0040000,
  BLOB = 0100644,
  BLOB_EXECUTABLE = 0100755,
  LINK = 0120000,
  COMMIT = 0160000,
};

using Entry = std::tuple<Name, Mode, Id>;

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
    void operator()(Name, Mode, lr::LR<Id>) const;
    void operator()(
        lr::LR<std::tuple<
            Name, std::variant<TreeId, BlobId, ExecId, LinkId, CommId>>>) const
        noexcept;
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

constexpr static Name::argument name;
constexpr static TreeId::argument id;
constexpr static NamedArgument<Mode>::argument mode;

lr::LR<UPtr<git_repository>> open(const char *);

} // namespace git

#endif