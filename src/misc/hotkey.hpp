#pragma once
#include "settings.hpp"
#include "manager.hpp"
#include "hotkey_manager_interface.hpp"

#include <boost/thread.hpp>
#include <X11/Xlib.h>
#include <map>

class X11HotkeyManager : public HotkeyManagerInterface {
public:
  X11HotkeyManager(GameManager&);
  ~X11HotkeyManager();
  void bind(string, HotkeyPressedCallback) override;
  void unbind(string) override;
  void startListen() override;
  void stopListen() override;
  bool isListening() override;

private:
  boost::thread keyListener;
  std::map<unsigned int, HotkeyPressedCallback> bindings;
  std::map<unsigned int, bool> holding_key;
  std::map<unsigned int, boost::thread> threads;
  boost::thread keyPressListener;
  boost::thread mousePressListener;
  const unsigned int mouseListOffset = 0x160;
  bool listening;

  Display* display;
  Window rootWindow;
  Window csWindow;
  GameManager& csgo;
  Settings& settings;

  Window findCSWindow();
  Window activeWindow();
  Window findWindowRecursive(Window);
  void callLoop(unsigned int, boost::function<void(unsigned int)>);
  void keyPressListen();
  void mousePressListen();
  void forwardEvent(XEvent event);
  unsigned int eventCodeToMouseButton(unsigned int);
};
