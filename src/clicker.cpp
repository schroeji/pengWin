#include "clicker.hpp"
#include "memory_access.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <chrono>
#include <thread>
#include <string.h>
#include <unistd.h>

using namespace std;

Clicker::Clicker(MemoryAccess& mem) : mem(mem) {}

void Clicker::memClick() {
  unsigned int attack = 0x05;
  unsigned int stop_attack = 0x04;
  mem.write((void*) (mem.attack_addr), &attack, sizeof(attack));
  this_thread::sleep_for(chrono::milliseconds(5));
  mem.write((void*) (mem.attack_addr), &stop_attack, sizeof(stop_attack));
}

void Clicker::xClick() {
  Display* displayClick = XOpenDisplay(NULL);
  XEvent event;
  memset(&event, 0, sizeof (event));
  event.xbutton.button = Button1;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (displayClick);
  while (event.xbutton.subwindow) {
    event.xbutton.window = event.xbutton.subwindow;
    XQueryPointer (displayClick, event.xbutton.window,
                   &event.xbutton.root, &event.xbutton.subwindow,
                   &event.xbutton.x_root, &event.xbutton.y_root,
                   &event.xbutton.x, &event.xbutton.y,
                   &event.xbutton.state);
  }
  // Press
  event.type = ButtonPress;
  XSendEvent (displayClick, PointerWindow, True, ButtonPressMask, &event);
  XFlush (displayClick);
  this_thread::sleep_for(chrono::milliseconds(1));
  // Release
  event.type = ButtonRelease;
  XSendEvent (displayClick, PointerWindow, True, ButtonReleaseMask, &event);
  XFlush (displayClick);
}
