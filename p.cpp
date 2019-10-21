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
  o( ///
      std::string_view(reinterpret_cast<const char *>(cde + 46),
                       n /* File name */),
      v<uint32_t>(cde + 20 /*Compressed size*/),
      v<uint32_t>(cde + 24 /*Uncompressed size*/),
      v<uint32_t>(cde + 42 /* Relative offset of local file header. */),
      v<uint16_t>(cde + 10 /* Compression method */));
  if (count)
    read_entries(cde + 46 + n +
                     v<uint16_t>(cde + 30 /* Extra field length (m) */) +
                     v<uint16_t>(cde + 32 /* File comment length (k) */),
                 count - 1, o);
}

template <class O> void unzip(const uint8_t *buf, const size_t size, O &&o) {
  const uint8_t *eocd = buf + size - 22;
  if (size < 22 || v<uint32_t>(eocd + 0) != 0x06054b50)
    o("cant find end of central dir entry");
  else
    read_entries(
        buf + v<uint32_t>(eocd + 16 /* cd offset */),
        v<uint16_t>(eocd + 10 /* cd records count */) - 1,
        _o_{o,
            [&](auto name, auto csize, auto usize, auto offset, auto method) {
              const uint8_t *p = buf + offset;
              if (v<uint32_t>(p) != 0x04034b50)
                o("not a local file header signature");
              else
                o(name, [=](auto &&o) {
                  const auto n = v<uint16_t>(p + 26 /*File name length (n)*/);
                  const auto m = v<uint16_t>(p + 28 /*Extra field length (m)*/);
                  const uint8_t *cdata = p + 30 + n + m;
                  if (method == 0) {
                    o(cdata, usize);
                  } else if (method == 8) {
                    z_stream s = {};
                    std::vector<uint8_t> buff(usize + 1, '\0');
                    s.next_in = const_cast<Bytef *>(cdata);
                    s.avail_in = csize;
                    s.next_out = static_cast<Bytef *>(buff.data());
                    s.avail_out = usize + 1;
                    if (int err = inflateInit2(&s, -15) != Z_OK)
                      return o("not Z_OK");
                    if (inflate(&s, Z_FINISH) == Z_STREAM_END)
                      o(buff.data(), usize);
                    else
                      o("cant inflate");
                    inflateEnd(&s);
                  } else {
                    o("unsupported method");
                  }
                });
            }});
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
            [](std::string_view name, auto &&p) {
              p(_o_{[&](auto err) { std::cout << err << "\n"; },
                    [&](auto buff, auto size) {
                      std::cout << name << " - " << size << "\n"
                                << buff << "\n";
                    }});
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
