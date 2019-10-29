#pragma once
#include "_o_.hpp"
#include <string_view>
#include <vector>
#include <zlib.h>

namespace zip {
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

template <class O> void unzip(const void *in, const size_t size, O &&o) {
  const uint8_t *buf = reinterpret_cast<const uint8_t *>(in);
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
} // namespace zip
