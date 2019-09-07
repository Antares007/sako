#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include <git2.h>
#include <named_type.hpp>
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

using Sha1 = fluent::NamedType<std::string, struct Sha1Tag>;
using Name = fluent::NamedType<const char *, struct NameTag>;
constexpr static Name::argument name;
using Id = fluent::NamedType<git_oid, struct IdTag>;
constexpr static Id::argument id;

enum Mode {
  UNREADABLE = 0000000,
  TREE = 0040000,
  BLOB = 0100644,
  BLOB_EXECUTABLE = 0100755,
  LINK = 0120000,
  COMMIT = 0160000,
};
constexpr static NamedArgument<Mode>::argument mode;
// using Mode = fluent::NamedType<git_filemode_t, struct ModeTag>;
// constexpr static Mode::argument mode;

struct treeBark {
  using Entry = std::tuple<Name, Mode, Id>;
  struct Ray {
    const std::vector<Entry> entries;
    const treeBark &bark;
    void operator()(Name, Mode, Id) const;
  };
  using Pith = void (*)(const Ray &);
  lr::LR<Sha1> operator()(const Sha1 &, Pith) const;
  lr::LR<Sha1> operator()(Pith) const;
};

} // namespace git

#endif