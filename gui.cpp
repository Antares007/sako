#include <GL/gl.h>
#include <GL/glx.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <png.h>
typedef int(glSwapInterval_t)(Display *dpy, GLXDrawable drawable, int interval);
// static glSwapInterval_t *glSwapIntervalEXT;

static void window_create(Display *display, Window windowRoot, int x, int y,
                          int width, int height, uint32_t color) {
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
  XStoreName(display, window, "OneLoneCoder.com - Pixel Game Engine");

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
    buff[i] = color;
  }

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, buff);
  delete[] buff;

  XEvent xev;
  while (true) {
    while (XPending(display)) {
      XNextEvent(display, &xev);
    }
    // glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height,
    //                GL_RGBA, GL_UNSIGNED_BYTE, s1.GetData());
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
  }
}
#include <iostream>

int main() {
  auto display = XOpenDisplay(NULL);
  auto windowRoot = DefaultRootWindow(display);

  window_create(display, windowRoot, 0, 0, 100, 100, 0xFF0000FF);
  std::cout << "hello";
  return 9;
}
