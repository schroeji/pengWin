#include "settings.hpp"

#include <boost/thread.hpp>
#include <X11/Xlib.h>
#include <map>

class HotkeyManager {
public:
  HotkeyManager(Settings);
  ~HotkeyManager();
  void bind(KeySym, void*);
  void startListen();

private:
  boost::thread keyListener;
  std::map<unsigned int, void*> bindings;
  Display* display;
  Window rootWindow;
  Settings settings;
};
