#include <type_traits>
struct head_ray;
struct tail_ray;

constexpr inline auto head_ray_v = static_cast<head_ray *>(nullptr);
constexpr inline auto tail_ray_v = static_cast<tail_ray *>(nullptr);

struct list_pith {
  template <typename H> void operator()(head_ray *, H &&) const {}
  template <typename T> void operator()(tail_ray *, T &&tail) const {
    tail(*this);
  }
};

constexpr inline auto sample_list = [](auto o) {
  o(head_ray_v, 3);
  o(tail_ray_v, [](auto o) {
    o(head_ray_v, 6);
    o(tail_ray_v, [](auto o) { o(head_ray_v, 9); });
  });
};
template <typename...> struct list;
template <typename... Args> list(Args...) -> list<Args...>;

template <typename H> struct list<H> {
  H h;
  template <typename O> void operator()(const O &o) const { o(head_ray_v, h); }
};
template <typename H, typename T> struct list<H, list<T>> {
  H h;
  list<T> t;
  template <typename O> void operator()(const O &o) const {
    o(head_ray_v, h);
    o(tail_ray_v, t);
  }
};
template <typename H, typename T, typename... Rest>
void test(H h, T t, Rest... rest) {}
int main() {
  list{1, list{1}}(list_pith{});
  return 9;
}
