#pragma once

#include <GL/glx.h>

#include "pith.hpp"

constexpr inline auto windowB = [](auto pith, Display *display,
                                   Window windowRoot, int x, int y, int width,
                                   int height) {
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
        display, windowRoot, x, y, width, height, 0, visualinfo->depth,
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
