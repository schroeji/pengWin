#pragma once
#include "settings.hpp"
#include "manager.hpp"
#include "hotkey_manager_interface.hpp"

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>
#include <boost/thread.hpp>
#include <map>

class WaylandHotkeyManager : public HotkeyManagerInterface {
public:
  WaylandHotkeyManager(GameManager& csgo) : csgo_{csgo}, settings{Settings::getInstance()}, HotkeyManagerInterface(){}
  ~WaylandHotkeyManager();
  void bind(string, HotkeyPressedCallback);
  void unbind(string);
  void startListen();
  void stopListen();
  bool isListening();

private:
  void keyPressListen();
  void mousePressListen();

  boost::thread keyListener;
  std::map<unsigned int, HotkeyPressedCallback> bindings;
  std::map<unsigned int, bool> holding_key;
  std::map<unsigned int, boost::thread> threads;
  boost::thread keyPressListener;
  boost::thread mousePressListener;
  const unsigned int mouseListOffset = 0x160;
  bool listening;

  wl_display *display;
  wl_registry *registry;
  wl_seat *seat;
  wl_keyboard *keyboard;
  // xkb_context *xkb_context;
  // xkb_keymap *xkb_keymap;

  GameManager& csgo_;
  Settings& settings;
};
