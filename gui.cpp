#include "draw.hpp"
#include "draw_string.hpp"
#include "loop.hpp"
#include "window.hpp"
#include <iostream>
#include <math.h>

template <int X, int Y, int W, int H, typename Pith> struct frame {
  Pith pith;

  template <typename O> void operator()(O o) {
    pith([&](auto x, auto y) {
      o(x > X + W ? X + W : x < X ? X : x, y > Y + H ? Y + H : y < Y ? Y : y);
    });
  }
};
template <int X, int Y, int W, int H, typename Pith>

constexpr frame<X, Y, W, H, Pith> make_frame(Pith &&pith) {
  return frame<X, Y, W, H, Pith>{static_cast<Pith &&>(pith)};
}

struct pixel {
  union {
    uint32_t n = 0xFF000000;
    struct {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      uint8_t a;
    };
  };

  pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
      : r(red), g(green), b(blue), a(alpha) {}
  pixel(uint32_t value) : n(value) {}
};

static const pixel WHITE(255, 255, 255), GREY(192, 192, 192),
    DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64), RED(255, 0, 0),
    DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0), YELLOW(255, 255, 0),
    DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0), GREEN(0, 255, 0),
    DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0), CYAN(0, 255, 255),
    DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64), BLUE(0, 0, 255),
    DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64), MAGENTA(255, 0, 255),
    DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64), BLACK(0, 0, 0),
    BLANK(0, 0, 0, 0);

int main() {
  auto display = XOpenDisplay(NULL);
  auto windowRoot = DefaultRootWindow(display);

  size_t framecounter = 0;

  auto sample =
      (loopB ^ (windowB, display, windowRoot, 0, 0, 128, 128,
                (drawB ^ (draw_string ^ [&](auto o) {
                   o([&](auto o) {
                     for (int i = 0; i < 128; i++)
                       for (int j = 0; j < 128; j++)
                         o(i, j,
                           pixel(rand() % 256, rand() % 128, rand() % 128).n);
                     o(10, 10, "ABOA\nBOABO", 0xcc0000ff, 3);
                     o(++framecounter < 200);
                     // for (int i = 0; i < 128; i++)
                     //  for (int j = 0; j < 48; j++) {
                     //    o(i, j, pixel(0xFF000000 | fontSprite[i + j
                     //    * 128]).n);
                     //  }
                     // for (int i = 0; i < 128; i++)
                     //  for (int j = 0; j < 48; j++) {
                     //    auto p = temp_bitmap[i + j * 128];
                     //    if (p)
                     //      o(i, j + 48, pixel(p, p, p).n);
                     //  }
                   });
                 }))));

  sample(rays{[](error_ray *, auto err) { std::cerr << err << std::endl; },
              [](auto x) { std::cout << x << std::endl; }});
  return 9;
}
