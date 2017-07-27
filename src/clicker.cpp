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
  unsigned int stop_attack = 0x05;
  mem.write((void*) (mem.attack_addr), &attack, sizeof(attack));
  this_thread::sleep_for(chrono::milliseconds(5));
  mem.write((void*) (mem.attack_addr), &stop_attack, sizeof(stop_attack));
}

void Clicker::xClick() {
  Display *display = XOpenDisplay(NULL);
  XEvent event;
  event.type = ButtonPress;
  event.xbutton.button = button1;
  event.xbutton.same_screen = True;
  XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  event.xbutton.subwindow = event.xbutton.window;
  while(event.xbutton.subwindow) {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
  }
  XSendEvent(display, PointerWindow, True, 0xfff, &event)
  XFlush(display);
  this_thread::sleep_for(chrono::milliseconds(5));
  event.type = ButtonRelease;
  event.xbutton.state = 0x100;
  XSendEvent(display, PointerWindow, True, 0xfff, &event);
  XFlush(display);
  XCloseDisplay(display);
}
