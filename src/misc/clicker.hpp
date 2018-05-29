#pragma once
#include "memory_access.hpp"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class Clicker {
private:
  MemoryAccess& mem;

  Display* displayClick;
  XEvent event;

public:
  Clicker(MemoryAccess& mem);
  // clicks using X window system
  void xClick();
  // clicks using memory write
  void memClick();
};
