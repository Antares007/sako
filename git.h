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

enum Mode {
  UNREADABLE = 0000000,
  TREE = 0040000,
  BLOB = 0100644,
  BLOB_EXECUTABLE = 0100755,
  LINK = 0120000,
  COMMIT = 0160000,
};

using Entry = std::tuple<Name, Mode, Id>;

struct D {
  void operator()(git_repository *) const noexcept;
};

using Repo = std::unique_ptr<git_repository, D>;

struct Bark {
  struct Ray {
    const std::vector<Entry> &entries;
    const Bark &bark;
    void operator()(Name, Mode, lr::LR<Id>) const;
    explicit Ray(std::vector<Entry> &&entries, const Bark &bark)
        : entries(std::forward<std::vector<Entry>>(entries)), bark(bark) {}
  };
  using Pith = void (*)(Ray &&);
  lr::LR<Id> operator()(Id, Pith) const;
  lr::LR<Id> operator()(Pith) const;
  Bark(Repo &&repo);

private:
  Repo repo;
};

constexpr static Name::argument name;
constexpr static Id::argument id;
constexpr static NamedArgument<Mode>::argument mode;

lr::LR<Repo> open(const char *);

} // namespace git

#endif