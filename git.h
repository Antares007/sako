#ifndef GIT_H
#define GIT_H 1

#include <named_type.hpp>
#include <variant>
#include <vector>
#include <tuple>

namespace git
{

using Error = fluent::NamedType<std::string, struct ErrorTag>;
using Sha1 = fluent::NamedType<std::string, struct Sha1Tag>;
using Repo = fluent::NamedType<void *, struct RepoTag>;

enum Mode
{
    UNREADABLE = 0000000,
    TREE = 0040000,
    BLOB = 0100644,
    BLOB_EXECUTABLE = 0100755,
    LINK = 0120000,
    COMMIT = 0160000,
};

struct treeBark
{
    using O = std::tuple<std::string, Mode, Sha1>;
    using Pith = void (*)(
        const std::function<void(const O &&)>,
        const std::vector<O> &,
        const Repo &,
        const treeBark &);
    Sha1 operator()(Sha1, Pith) const;
    Sha1 operator()(Pith) const;
};

} // namespace git

#endif