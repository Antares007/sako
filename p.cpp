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
struct zheader {
  std::uint16_t version = 20;
  std::uint16_t flags = 0;
  std::uint16_t compression_type = 8;
  std::uint16_t stamp_date = 0;
  std::uint16_t stamp_time = 0;
  std::uint32_t crc = 0;
  std::uint32_t compressed_size = 0;
  std::uint32_t uncompressed_size = 0;
  std::string filename;
  std::string comment;
  std::vector<std::uint8_t> extra;
  std::uint32_t header_offset = 0;
};

template <class T>
constexpr inline auto read_int = [](std::istream &stream) -> T {
  T value;
  stream.read(reinterpret_cast<char *>(&value), sizeof(T));
  return value;
};

constexpr inline auto read_header = [](std::istream &istream,
                                       auto &&o) -> void {
  zheader header;
  auto sig = read_int<std::uint32_t>(istream);
  if (sig != 0x02014b50)
    return o("missing global header signature");
  read_int<std::uint16_t>(istream); // version made by
  header.version = read_int<std::uint16_t>(istream);
  header.flags = read_int<std::uint16_t>(istream);
  header.compression_type = read_int<std::uint16_t>(istream);
  header.stamp_date = read_int<std::uint16_t>(istream);
  header.stamp_time = read_int<std::uint16_t>(istream);
  header.crc = read_int<std::uint32_t>(istream);
  header.compressed_size = read_int<std::uint32_t>(istream);
  header.uncompressed_size = read_int<std::uint32_t>(istream);
  const auto filename_length = read_int<std::uint16_t>(istream);
  const auto extra_length = read_int<std::uint16_t>(istream);
  const std::uint16_t comment_length = read_int<std::uint16_t>(istream);
  /*std::uint16_t disk_number_start = */ read_int<std::uint16_t>(istream);
  /*std::uint16_t int_file_attrib = */ read_int<std::uint16_t>(istream);
  /*std::uint32_t ext_file_attrib = */ read_int<std::uint32_t>(istream);
  header.header_offset = read_int<std::uint32_t>(istream);
  header.filename.resize(filename_length, '\0');
  istream.read(&header.filename[0], filename_length);
  header.extra.resize(extra_length, 0);
  istream.read(reinterpret_cast<char *>(header.extra.data()), extra_length);
  header.comment.resize(comment_length, '\0');
  istream.read(&header.comment[0], comment_length);
  o(std::move(header));
};

template <class T>
inline auto read =
    [](const uint8_t *buf) -> T { return *reinterpret_cast<const T *>(buf); };

template <class O>
void read_entries(const uint8_t *cde, const size_t count, const O &o) {
  if (read<uint32_t>(cde) != 0x02014b50)
    return o("not a central dir entry");
  const auto n = read<uint16_t>(cde + 28 /* File name length (n) */);
  o(std::string_view(reinterpret_cast<const char *>(cde + 46 /* File name */),
                     n),
    read<uint32_t>(cde + 42 /* Relative offset of local file header. */));
  if (count)
    read_entries(cde + 46 + n +
                     read<uint16_t>(cde + 30 /* Extra field length (m) */) +
                     read<uint16_t>(cde + 32 /* File comment length (k) */),
                 count - 1, o);
}

template <class O> void unzip(const uint8_t *buf, const size_t size, O &&o) {
  const uint8_t *eocd = buf + size - 22;
  if (size < 22 || read<uint32_t>(eocd + 0) != 101010256)
    return o("not zip");
  const auto count =
      read<uint16_t>(eocd + 10 /* Total number of central directory records */);
  auto entries = std::vector<std::pair<std::string_view, const uint8_t *>>();
  auto ok = true;
  read_entries(buf + read<uint32_t>(
                         eocd + 16 /* Offset of start of central directory */),
               count - 1,
               _o_{[&](const char *err) {
                     ok = false;
                     o(err);
                   },
                   [&](auto &&v, auto offset) { ///
                     entries.emplace_back(v, buf + offset);
                   }});

  if (ok)
    o(entries);
};

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
            [](std::vector<std::pair<std::string_view, const uint8_t *>> &v) {
              for (auto [name, ep] : v)
                std::cout << name << " " << ep << "\n";
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
