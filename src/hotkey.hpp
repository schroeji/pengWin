#include "settings.hpp"
#include "manager.hpp"

#include <boost/thread.hpp>
#include <X11/Xlib.h>
#include <map>

class HotkeyManager {
public:
  HotkeyManager(GameManager);
  ~HotkeyManager();
  void bind(KeySym, boost::function<void()>);
  void startListen();

private:
  boost::thread keyListener;
  std::map<unsigned int, boost::function<void()>> bindings;
  std::map<unsigned int, bool> holding_key;
  std::map<unsigned int, boost::thread> threads;
  boost::thread keyPressListener;

  Display* display;
  Window rootWindow;
  GameManager csgo;
  Settings settings;

  void callLoop(unsigned int, boost::function<void()>);
  void keyPressListen();
  void forwardEvent(XEvent event);
};
