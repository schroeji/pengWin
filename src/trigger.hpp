#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"
#include "settings.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

class Trigger {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  Settings settings;
  Display* display;
  Window rootWindow;

public:
  Trigger(GameManager& csgo);
  void triggerCheck();
};
