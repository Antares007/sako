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
#include <chrono>
int main() {
  auto display = XOpenDisplay(NULL);
  auto windowRoot = DefaultRootWindow(display);

  size_t framecounter = 0;

  auto sample = purry(
      loopB,
      curry(windowB, display, windowRoot, 0, 0, 256, 240,
            purry(drawB, purry(draw_string, [&](auto o) {
                    auto tp1 = std::chrono::system_clock::now();
                    auto tp2 = std::chrono::system_clock::now();
                    auto fFrameTimer = 0.f;
                    auto nFrameCount = 0;
                    auto nFps = 0;

                    o([&](auto o) {
                      // Handle Timing
                      tp2 = std::chrono::system_clock::now();
                      std::chrono::duration<float> elapsedTime = tp2 - tp1;
                      tp1 = tp2;
                      // Our time per frame coefficient
                      float fElapsedTime = elapsedTime.count();
                      for (int i = 0; i < 256; i++)
                        for (int j = 0; j < 240; j++)
                          o(i, j,
                            pixel(rand() % 128, rand() % 128, rand() % 128).n);
                      o(++framecounter < 20000);
                      fFrameTimer += fElapsedTime;
                      nFrameCount++;
                      if (fFrameTimer >= 1.0f) {
                        fFrameTimer -= 1.0f;
                        nFps = nFrameCount;
                        nFrameCount = 0;
                      }
                      if (nFps > 0) {
                        char buf[16]; // need a buffer for that
                        sprintf(buf, "%d", nFps);
                        o(10, 10, buf, -1, 2);
                      }
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

  sample(o::rays{[](error_ray *, auto err) { std::cerr << err << std::endl; },
                 [](auto x) { std::cout << x << std::endl; }});
  return 9;
}
