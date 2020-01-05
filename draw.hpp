#pragma once

#include <GL/glx.h>

#include "purry.hpp"

constexpr inline auto drawB = [](auto o, auto pith, auto display, auto window) {
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
  o([&](auto o) {
    glClear(GL_COLOR_BUFFER_BIT);

    pith(rays{o,
              [&](int x, int y, uint32_t p) { buff[x + y * gwa.width] = p; }});

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
  delete[] buff;
};
