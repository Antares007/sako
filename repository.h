#ifndef REPOSITORY_HPPA
#define REPOSITORY_HPPA 1
#include <git2/repository.h>
#include <memory>

namespace git {

template <typename T, typename D> std::unique_ptr<T, D> make(T *p, D &&d) {
  return std::unique_ptr<T, D>(p, d);
}

struct D {
  void operator()(git_repository *p) { git_repository_free(p); }
};

static std::unique_ptr<git_repository, D> openR(const char *path) {
  git_repository *p;
  git_repository_open(&p, path);
  return std::unique_ptr<git_repository, D>(p, D());
}

} // namespace git
#endif