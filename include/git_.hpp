#ifndef AB_GIT_H
#define AB_GIT_H
#include "lr.hpp"
#include "newtype.hpp"
#include <memory>
namespace abo {
using namespace lr;

NewType(Repo, std::unique_ptr<void *, void (*)(void *)>);
NewType(TreeSha, std::string);

LR<Repo> open(const char *);

struct O {};

template <typename Pith> struct Bark {
  Pith pith;
  LR<TreeSha> operator()(const Repo &) const {
    const auto o = O{};
    pith(o);
  }
};

template <typename Pith> Bark(Pith)->Bark<Pith>;

} // namespace abo
#endif
