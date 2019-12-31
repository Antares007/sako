#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <png.h>

#include "ob.hpp"
#include <iostream>
#include <math.h>

typedef int(glSwapInterval_t)(Display *dpy, GLXDrawable drawable, int interval);
// static glSwapInterval_t *glSwapIntervalEXT;

template <typename Pith> struct pixel {
  Pith pith;
  template <typename O> void operator()(const O &o) const {
    pith([&](unsigned char r, unsigned char g, unsigned char b,
             unsigned char a) { o(r, g, b, a); });
  }
};
template <typename Pith> pixel(Pith)->pixel<Pith>;

template <int X, int Y, int W, int H, typename Pith> struct frame {
  Pith pith;
  template <typename O> void operator()(const O &o) const {
    pith([&](auto x, auto y) {
      o(x > X + W ? X + W : x < X ? X : x, y > Y + H ? Y + H : y < Y ? Y : y);
    });
  }
};
template <int X, int Y, int W, int H, typename Pith>
constexpr frame<X, Y, W, H, Pith> make_frame(Pith &&pith) {
  return frame<X, Y, W, H, Pith>{static_cast<Pith &&>(pith)};
}

template <typename Pith> struct window_bark {
  Pith pith;
  MOB()
  (Display *display, Window windowRoot, int x, int y, int width, int height,
   uint32_t color) {
    int attribs[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
    auto visualinfo = glXChooseVisual(display, 0, attribs);
    auto colourmap =
        XCreateColormap(display, windowRoot, visualinfo->visual, AllocNone);
    XSetWindowAttributes attributes;
    attributes.colormap = colourmap;

    // Register which events we are interested in receiving
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                            ButtonPressMask | ButtonReleaseMask |
                            PointerMotionMask | FocusChangeMask |
                            StructureNotifyMask;

    auto window = XCreateWindow(
        display, windowRoot, x, y, width, height, 0, visualinfo->depth,
        InputOutput, visualinfo->visual, CWColormap | CWEventMask, &attributes);

    Atom wmDelete = XInternAtom(display, "WM_DELETE_WINDOW", true);
    XSetWMProtocols(display, window, &wmDelete, 1);

    XMapWindow(display, window);
    XStoreName(display, window, "ABO");

    auto glDeviceContext =
        glXCreateContext(display, visualinfo, nullptr, GL_TRUE);
    glXMakeCurrent(display, window, glDeviceContext);

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

    auto buff = new uint32_t[width * height];

    for (int i = 0; i < width * height; i++) {
      buff[i] = 0xFF000000 | (i % 256);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, buff);
    size_t frameIndex = 0;
    XEvent xev;
    while (1) {
      while (XPending(display)) {
        XNextEvent(display, &xev);
        std::cout << xev.type << std::endl;
      }
      // for (int i = 0; i < width * height; i++) {
      //  buff[i] = 0xFF000000 |
      //            (uint8_t)(sinf((frameIndex % (314 * 2)) / 5.f) * 100 + 100);
      //}
      // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA,
      //                GL_UNSIGNED_BYTE, buff);
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
      frameIndex++;
    }
    delete[] buff;
    o(10);
  }
};
template <typename Pith> window_bark(Pith)->window_bark<Pith>;

int main() {
  auto display = XOpenDisplay(NULL);
  auto windowRoot = DefaultRootWindow(display);

  auto z = OB()() { o(1); };

  window_bark{z}([](auto...) {}, display, windowRoot, 0, 0, 400, 240,
                 0xFF0000CC);

  make_frame<-1, -1, 2, 2>(OB()() {
    o(9, 9);
    o(-9, -9);
    o(-9, 9);
    o(9, -9);
  })([](float x, float y) { std::cout << x << " " << y << std::endl; });
  return 9;
}
