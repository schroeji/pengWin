#include "hotkey.hpp"

#include <X11/Xlib.h>
#include <X11/keysymdef.h>
#include <X11/Xutil.h>
#include <boost/thread.hpp>

using namespace std;
HotkeyManager::HotkeyManager(Settings settings) : settings(settings) {
  display = XOpenDisplay(NULL);
  rootWindow = DefaultRootWindow(display);
}

HotkeyManager::~HotkeyManager() {}

void HotkeyManager::bind(KeySym key, void* function){
  unsigned int keycode = XKeysymToKeycode(display, key);
  bindings.insert(std::pair<unsigned int, void*>(keycode, function));
}
