#pragma once
#include "_o_.hpp"
#include <string_view>
#include <vector>
#include <zlib.h>

template <class T>
static inline auto v =
    [](const char *buf) -> T { return *reinterpret_cast<const T *>(buf); };

constexpr inline auto unzip = [](auto o, const void *in, const size_t size) {
  const char *buf = reinterpret_cast<const char *>(in);
  const char *eocd = buf + size - 22;
  if (size < 22 || v<uint32_t>(eocd + 0) != 0x06054b50)
    return o(-7);
  auto read_entries = rec{[&o, &buf](auto read_entries, const char *cde,
                                     const size_t count) {
    if (v<uint32_t>(cde) != 0x02014b50)
      return o(-7);
    const auto n = v<uint16_t>(cde + 28);
    const auto name =
        std::string_view(reinterpret_cast<const char *>(cde + 46), n);
    const auto csize = v<uint32_t>(cde + 20);
    const auto usize = v<uint32_t>(cde + 24);
    const auto offset = v<uint32_t>(cde + 42);
    const auto method = v<uint16_t>(cde + 10);

    const char *p = buf + offset;
    if (v<uint32_t>(p) != 0x04034b50)
      o(-71);
    else {
      const auto n = v<uint16_t>(p + 26);
      const auto m = v<uint16_t>(p + 28);
      const void *cdata = p + 30 + n + m;
      if (method == 0) {
        o(name, cdata, usize);
      } else if (method == 8) {
        z_stream s = {};
        std::vector<uint8_t> buff(usize + 1, '\0');
        s.next_in = (Bytef *)(cdata);
        s.avail_in = csize;
        s.next_out = static_cast<Bytef *>(buff.data());
        s.avail_out = usize + 1;
        if (int err = inflateInit2(&s, -15) != Z_OK)
          return o(-72);
        if (inflate(&s, Z_FINISH) == Z_STREAM_END)
          o(name, reinterpret_cast<const void *>(buff.data()), usize);
        else
          o(-73);
        inflateEnd(&s);
      } else {
        o(-74);
      }
    }
    if (count)
      read_entries(cde + 46 + n + v<uint16_t>(cde + 30) + v<uint16_t>(cde + 32),
                   count - 1);
  }};
  read_entries(buf + v<uint32_t>(eocd + 16), v<uint16_t>(eocd + 10) - 1);
};
