#include "git.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>

template <typename Ts> struct print;

namespace git {

void treeBark::Ray::operator()(lr::LR<O> &&r) const {
  std::cout << &r << '\n';
};

lr::LR<Sha1> treeBark::operator()(const Sha1 &, treeBark::Pith) const {
  return Sha1{"3"};
};

lr::LR<Sha1> treeBark::operator()(treeBark::Pith pith) const {

  pith(treeBark::Ray{
      std::vector<treeBark::O>{std::make_tuple("", BLOB, Sha1{"1"})}, *this});

  return Sha1{"2"};
};

} // namespace git

/*

struct S
{
    S()
    noexcept
    {
        puts("S()");
    }
    S(S const &)
    noexcept
    {
        puts("S(const S &)");
    }
    S(S &&)
    noexcept
    {
        puts("S(S &&)");
    }
    S &operator=(const S &) noexcept
    {
        puts("S &operator = (const S &)");
        return *this;
    }
    S &operator=(S &&) noexcept
    {
        puts("S &operator = (S &&)");
        return *this;
    }
    ~S() noexcept
    {
        puts("~S()");
    }
};
void test()
{

    auto see = lr::map(
        [&](S &a) {
            return a;
        },
        lr::LR<S>(S{}));

    std::cout << &see << '\n';
}
*/
