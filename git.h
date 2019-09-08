#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include "nt.h"
#include <git2/types.h>
#include <string>
#include <vector>
template <typename T> struct NamedArgument {
  struct argument {
    template <typename U> T operator=(U &&value) const {
      return T(std::forward<U>(value));
    }
    argument() = default;
    argument(argument const &) = delete;
    argument(argument &&) = delete;
    argument &operator=(argument const &) = delete;
    argument &operator=(argument &&) = delete;
  };
};
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

struct Bark {
  struct Ray {
    const std::vector<Entry> entries;
    const Bark &bark;
    void operator()(Name, Mode, lr::LR<Id>) const;
  };
  using Pith = void (*)(Ray &&);
  lr::LR<Id> operator()(Id, Pith) const;
  lr::LR<Id> operator()(Pith) const;

private:
  git_repository *repo;
};

constexpr static Name::argument name;
constexpr static Id::argument id;
constexpr static NamedArgument<Mode>::argument mode;

} // namespace git

#endif