#include <cstddef>
#include <functional>

template <class... Ts> struct _o_ : Ts... { using Ts::operator()...; };
template <class... Ts> _o_(Ts...)->_o_<Ts...>;

struct cursor {
  const char *data{};
  size_t pos = 0;
};

inline auto pstr = [](const char *str) {
  return [=]<typename O>(cursor in, O &&o) {
    size_t i = 0;
    while (char c = str[i]) {
      if (c != in.data[in.pos + i])
        return o(-1);
      i++;
    }
    o(str, cursor{in.data, in.pos + i});
  };
};

template <typename Pl, typename Pr> auto pand(Pl &&pl, Pr &&pr) {
  return [pl = std::forward<Pl>(pl),
          pr = std::forward<Pr>(pr)]<typename O>(cursor in, O &&o) {
    pl(in, _o_{o, [&](const char *v1, cursor rest) {
                 pr(rest, _o_{o, [&](const char *v2, cursor rest) {
                                o(v1, v2, rest);
                              }});
               }});
  };
}

template <typename Pl, typename Pr> auto por(Pl &&pl, Pr &&pr) {
  return [pl = std::forward<Pl>(pl),
          pr = std::forward<Pr>(pr)]<typename O>(cursor in, O &&o) {
    pl(in, _o_{[&](int) { pr(in, o); },
               [&](const char *m, cursor in) { o(m, in); }});
  };
}

#include <fstream>
#include <iostream>
#include <tuple>
#include <zlib.h>
template <class T>
inline auto v =
    [](const uint8_t *buf) -> T { return *reinterpret_cast<const T *>(buf); };

template <class O>
void read_entries(const uint8_t *cde, const size_t count, const O &o) {
  if (v<uint32_t>(cde) != 0x02014b50)
    return o("not a central dir entry");
  const auto n = v<uint16_t>(cde + 28 /* File name length (n) */);
  o(std::string_view(reinterpret_cast<const char *>(cde + 46 /* File name */),
                     n),
    v<uint32_t>(cde + 42 /* Relative offset of local file header. */));
  if (count)
    read_entries(cde + 46 + n +
                     v<uint16_t>(cde + 30 /* Extra field length (m) */) +
                     v<uint16_t>(cde + 32 /* File comment length (k) */),
                 count - 1, o);
}
struct file {
  std::string_view name;
  const uint8_t *p;
  file(std::string_view name, const uint8_t *p) : name(name), p(p) {}
  template <typename O> void operator()(const O &o) const { ///
  }
};

template <class O> void unzip(const uint8_t *buf, const size_t size, O &&o) {
  const uint8_t *eocd = buf + size - 22;
  if (size < 22 || v<uint32_t>(eocd + 0) != 101010256)
    return o("cant find end of central dir entry");
  const auto count =
      v<uint16_t>(eocd + 10 /* Total number of central directory records */);
  auto entries = std::vector<file>();
  auto ok = true;
  read_entries(
      buf + v<uint32_t>(eocd + 16 /* Offset of start of central directory */),
      count - 1,
      _o_{[&](const char *err) {
            ok = false;
            o(err);
          },
          [&](auto name, auto offset) { ///
            entries.emplace_back(name, buf + offset);
          }});

  if (ok)
    o(entries);
};

template <typename... Ts> struct print;
auto main() -> int {
  std::ifstream s("MyTest.xlsx", std::ios::binary);
  s.seekg(0, std::ios_base::end);
  const size_t size = static_cast<std::size_t>(s.tellg());
  std::vector<char> buf;
  buf.reserve(size);
  s.seekg(0);
  s.read(buf.data(), size);

  unzip(reinterpret_cast<const uint8_t *>(buf.data()), size,
        _o_{[](const char *err) { std::cout << err << "\n"; },
            [](std::vector<file> &v) {
              for (file f : v) {
                f([]() {});
                std::cout << f.name << " " << f.p << "\n";
              }
            }});

  pstr("ა")(cursor{"აბვ"}, _o_{[](int) { std::cout << "error\n"; },
                               [](const char *f, cursor rest) {
                                 std::cout << '[' << f << "] ["
                                           << rest.data + rest.pos << "]\n";
                               }});

  // auto l = pand(pstr("a"), pstr("b"));
  // l("abo", _o_{[](int) { std::cout << "error\n"; },
  //                    [](const char *f, const char *s, const char *rest) {
  //                      std::cout << '[' << f << s << "] [" << rest << "]\n";
  //                    }});

  // auto l2 = por(pstr("a"), pstr("b"));
  // l2("b", _o_{[](int) { std::cout << "error\n"; },
  //                   [](const char *f, const char *rest) {
  //                     std::cout << '[' << f << "] [" << rest << "]\n";
  //                   }});
  //
}
