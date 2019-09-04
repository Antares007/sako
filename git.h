#ifndef GIT_H
#define GIT_H 1

#include "lr.h"
#include <named_type.hpp>
#include <vector>

namespace git {

using Sha1 = fluent::NamedType<std::string, struct Sha1Tag>;

enum Mode {
  UNREADABLE = 0000000,
  TREE = 0040000,
  BLOB = 0100644,
  BLOB_EXECUTABLE = 0100755,
  LINK = 0120000,
  COMMIT = 0160000,
};

struct treeBark {
  using O = std::tuple<std::string_view, Mode, Sha1>;
  struct Ray {
    const std::vector<O> entries;
    const treeBark &bark;
    void operator()(lr::LR<O> &&) const;
  };
  using Pith = void (*)(const Ray &);
  lr::LR<Sha1> operator()(const Sha1 &, Pith) const;
  lr::LR<Sha1> operator()(Pith) const;
};

} // namespace git

#endif