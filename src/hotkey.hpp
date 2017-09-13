#include "settings.hpp"
#include "manager.hpp"

#include <boost/thread.hpp>
#include <X11/Xlib.h>
#include <map>

class HotkeyManager {
public:
  HotkeyManager(GameManager);
  ~HotkeyManager();
  void bind(string, boost::function<void(unsigned int)>);
  void startListen();

private:
  boost::thread keyListener;
  std::map<unsigned int, boost::function<void(unsigned int)>> bindings;
  std::map<unsigned int, bool> holding_key;
  std::map<unsigned int, boost::thread> threads;
  boost::thread keyPressListener;
  boost::thread mousePressListener;
  const unsigned int mouseListOffset = 0x160;

  Display* display;
  Window rootWindow;
  GameManager csgo;
  Settings settings;

  void callLoop(unsigned int, boost::function<void(unsigned int)>);
  void keyPressListen();
  void mousePressListen();
  void forwardEvent(XEvent event);
  unsigned int eventCodeToMouseButton(unsigned int);
};
