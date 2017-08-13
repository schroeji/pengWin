#include "memory_access.hpp"
#include "manager.hpp"
#include "clicker.hpp"
#include "typedef.hpp"
#include "settings.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <boost/thread.hpp>

class Trigger {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Clicker clicker;
  Settings settings;
  Display* display;
  Window rootWindow;
  boost::thread triggerThread;
  bool holding_hotkey;

public:
  Trigger(GameManager& csgo);
  ~Trigger();
  void triggerCheck();
  void triggerLoop();
};
