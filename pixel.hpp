#pragma once

#include "pith.hpp"
#include <GL/glx.h>

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

constexpr inline auto pixelB = [](auto pith) {
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
