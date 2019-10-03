#ifndef GIT_H
#define GIT_H 1

#include "lr.hpp"
#include "nt.hpp"
#include <git2/types.h>
#include <memory>
#include <type_traits>
#include <vector>

namespace git {

template <typename T> using UPtr = std::unique_ptr<T, void (*)(T *)>;

template <typename T, typename... Args> struct make {
  using C = int (*)(T **, Args...);
  using D = void (*)(T *);
  make(C _c, D _d) : c(_c), d(_d) {}

  lr::LR<UPtr<T>> operator()(Args &&... args) const {
    T *ptr = nullptr;
    if (c(&ptr, args...))
      return lr::L{__PRETTY_FUNCTION__};
    return UPtr<T>(ptr, d);
  }

private:
  C c;
  D d;
};

template <typename T, typename... Args> struct make2 {
  using C = int (*)(T *, Args...);
  make2(C _c) : c(_c) {}

  lr::LR<T> operator()(Args &&... args) const {
    T t;
    if (c(&t, args...))
      return lr::L{__PRETTY_FUNCTION__};
    return std::forward<T>(t);
  }

private:
  C c;
};

using TreeId = nt::NamedType<std::string, struct TreeIdTag>;
using BlobId = nt::NamedType<std::string, struct BlobIdTag>;
using TagId = nt::NamedType<std::string, struct TagIdTag>;
using ExecId = nt::NamedType<std::string, struct ExecIdTag>;
using LinkId = nt::NamedType<std::string, struct LinkIdTag>;
using CommitId = nt::NamedType<std::string, struct CommIdTag>;

using Ref = nt::NamedType<int, struct RefTag>;

struct TreeBark;

lr::LR<git_oid> commit(const char *message, lr::LR<git_oid> &&tree,
                       std::vector<lr::LR<git_oid>> &&parents);

template <typename R> using LR = lr::LR<R>;

struct TreeBark {
  struct O {
    void operator()(const git_tree_entry *) const;
  };

  const UPtr<git_repository> &repo;
  TreeBark(const UPtr<git_repository> &rhs) : repo(rhs) {}

  template <typename Pith> LR<git_oid> operator()(Pith &&pith) const {
    const auto o = O{};
    if constexpr (lr::islr_v<decltype(pith(o))>) {
      return pith(o) //
             | lr::fmap([](auto) { return LR<git_oid>(lr::L{"9"}); });
    } else {
      pith(o);
      return LR<git_oid>(lr::L{"b"});
    }
  }
};

void lookup(const TreeBark::O &o, const UPtr<git_tree> &pTree);

using Name = nt::NamedType<std::string, struct NameTag>;
using Entry =
    std::tuple<Name, std::variant<TreeId, BlobId, ExecId, LinkId, CommitId>>;
lr::LR<TreeId> writeTree(const UPtr<git_treebuilder> &);
// lr::LR<UPtr<git_tree>> lookup(const UPtr<git_repository> &, const TreeId
// &);
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
