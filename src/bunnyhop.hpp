#include "manager.hpp"
#include "memory_access.hpp"
#include "typedef.hpp"
#include "settings.hpp"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <boost/thread.hpp>
class BunnyHopper {
private:
  GameManager& csgo;
  MemoryAccess& mem;
  Settings settings;
  void jump();
  void jumpCheck();
  Display* display;
  Window rootWindow;
  bool holding_hotkey;
  unsigned int keycode;
  boost::thread jumpThread;

public:
  BunnyHopper(GameManager& csgo);
  ~BunnyHopper();
  void jumpLoop();
};
