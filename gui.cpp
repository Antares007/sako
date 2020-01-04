#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <png.h>

#include "pith.hpp"
#include "stb_truetype.h"
#include <iostream>
#include <math.h>

#define A constexpr inline auto

typedef int(glSwapInterval_t)(Display *dpy, GLXDrawable drawable, int interval);
// static glSwapInterval_t *glSwapIntervalEXT;

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

// Some constants for symbolic naming of Pixels
static const pixel WHITE(255, 255, 255), GREY(192, 192, 192),
    DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64), RED(255, 0, 0),
    DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0), YELLOW(255, 255, 0),
    DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0), GREEN(0, 255, 0),
    DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0), CYAN(0, 255, 255),
    DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64), BLUE(0, 0, 255),
    DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64), MAGENTA(255, 0, 255),
    DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64), BLACK(0, 0, 0),
    BLANK(0, 0, 0, 0);

template <int X, int Y, int W, int H, typename Pith> struct frame {
  Pith pith;
  MP()() {
    pith([&](auto x, auto y) {
      o(x > X + W ? X + W : x < X ? X : x, y > Y + H ? Y + H : y < Y ? Y : y);
    });
  }
};
template <int X, int Y, int W, int H, typename Pith>
constexpr frame<X, Y, W, H, Pith> make_frame(Pith &&pith) {
  return frame<X, Y, W, H, Pith>{static_cast<Pith &&>(pith)};
}
static const char *fontData =
    "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000"
    "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400"
    "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000"
    "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000"
    "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000"
    "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000"
    "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000"
    "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000"
    "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000"
    "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000"
    "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000"
    "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000"
    "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000"
    "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0"
    "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000"
    "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";
static uint32_t fontSprite[128 * 48] = {0xFF000000};

static void initFont() {
  int px = 0, py = 0;
  for (int b = 0; b < 1024; b += 4) {
    uint32_t sym1 = (uint32_t)fontData[b + 0] - 48;
    uint32_t sym2 = (uint32_t)fontData[b + 1] - 48;
    uint32_t sym3 = (uint32_t)fontData[b + 2] - 48;
    uint32_t sym4 = (uint32_t)fontData[b + 3] - 48;
    uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;
    for (int i = 0; i < 24; i++) {
      int k = r & (1 << i) ? 255 : 0;
      fontSprite[px + py * 128] = k << 24 | k << 16 | k << 8 | k;
      if (++py == 48) {
        px++;
        py = 0;
      }
    }
  }
}

A loopB = [](auto pith) {
  return P()(auto f) {
    bool active = true;
    while (active) {
      f(rays{o, [&](bool b) { active = b; }});
    }
  }
  ^pith;
};

A windowB = [](auto pith, Display *display, Window windowRoot, int x, int y,
               int width, int height) {
  return P(=)() {
    int attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    const auto visualinfo = glXChooseVisual(display, 0, attribs);
    if (!visualinfo)
      return o(error_ray_v, -1);
    const auto colourmap =
        XCreateColormap(display, windowRoot, visualinfo->visual, AllocNone);
    if (colourmap == BadAlloc || colourmap == BadMatch ||
        colourmap == BadValue || colourmap == BadWindow)
      return o(error_ray_v, -2);
    XSetWindowAttributes attributes;
    attributes.colormap = colourmap;
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPressMask | ButtonReleaseMask |
                            PointerMotionMask | FocusChangeMask |
                            StructureNotifyMask;
    const auto window = XCreateWindow(
        display, windowRoot, x, y, width * 3, height * 3, 0, visualinfo->depth,
        InputOutput, visualinfo->visual, CWColormap | CWEventMask, &attributes);
    if (window == BadAlloc || window == BadMatch || window == BadValue ||
        window == BadWindow)
      return o(error_ray_v, -3);
    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, window, &wmDelete, 1);
    XMapWindow(display, window);
    XStoreName(display, window, "ABO");
    const auto glDeviceContext =
        glXCreateContext(display, visualinfo, nullptr, GL_TRUE);
    glXMakeCurrent(display, window, glDeviceContext);
    pith(o, display, window);
  };
};

A pixelB = [](auto pith) {
  return P(=)(auto display, auto window) {
    XWindowAttributes gwa;
    XGetWindowAttributes(display, window, &gwa);
    glViewport(0, 0, gwa.width, gwa.height);
    GLuint glBuffer;
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &glBuffer);
    glBindTexture(GL_TEXTURE_2D, glBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    auto buff = new uint32_t[gwa.width * gwa.height];
    for (int i = 0; i < gwa.width * gwa.height; i++)
      buff[i] = 0xff000000;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, gwa.width, gwa.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, buff);
    (P(&)(auto pith) {
      o([&](auto o) {
        glClear(GL_COLOR_BUFFER_BIT);

        pith(rays{
            o, [&](int x, int y, pixel p) { buff[x + y * gwa.width] = p.n; }});

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gwa.width, gwa.height, GL_RGBA,
                        GL_UNSIGNED_BYTE, buff);
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 1.0);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glTexCoord2f(0.0, 0.0);
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0, 0.0);
        glVertex3f(1.0f, 1.0f, 0.0f);
        glTexCoord2f(1.0, 1.0);
        glVertex3f(1.0f, -1.0f, 0.0f);
        glEnd();
        glXSwapBuffers(display, window);
      });
    } ^
     pith)(o);
    delete[] buff;
  };
};
#include <fstream>

int main() {
  initFont();
  stbtt_fontinfo info;
  std::string a;
  std::ifstream file("/home/antares/.local/share/fonts/Hack-Regular.ttf");
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());

  stbtt_InitFont(&info, (const uint8_t *)content.data(), 0);

  unsigned char temp_bitmap[128 * 48];
  stbtt_bakedchar cdata[33];
  stbtt_BakeFontBitmap((const uint8_t *)content.data(), 0, 16, temp_bitmap, 128,
                       48, 0x10D0, 33, cdata);

  auto display = XOpenDisplay(NULL);
  auto windowRoot = DefaultRootWindow(display);

  size_t framecounter = 0;

  auto sample = loopB(
      windowB(pixelB(P(&)() {
                o(P(&)() {
                  o(++framecounter < 200);
                  // for (int i = 0; i < 128; i++)
                  //  for (int j = 0; j < 128; j++)
                  //    o(i, j, pixel(rand() % 256, rand() % 256, rand() %
                  //    256));

                  for (int i = 0; i < 128; i++)
                    for (int j = 0; j < 48; j++) {
                      o(i, j, pixel(0xFF000000 | fontSprite[i + j * 128]));
                    }
                  for (int i = 0; i < 128; i++)
                    for (int j = 0; j < 48; j++) {
                      auto p = temp_bitmap[i + j * 128];
                      if (p)
                        o(i, j + 48, pixel(p, p, p));
                    }
                  //                    return true;
                });
              }),
              display, windowRoot, 0, 0, 128, 128));

  sample(rays{[](error_ray *, auto err) { std::cerr << err << std::endl; },
              [](auto x) { std::cout << x << std::endl; }});
  return 9;
}
