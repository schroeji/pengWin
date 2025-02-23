#pragma once
#include "settings.hpp"
#include "manager.hpp"
#include "boost/function.hpp"

class HotkeyManagerInterface {
public:
  using HotkeyPressedCallback = boost::function<void(unsigned int)>;
  HotkeyManagerInterface() = default;
  virtual ~HotkeyManagerInterface() = default;
  virtual void bind(string, HotkeyPressedCallback) = 0;
  virtual void unbind(string) = 0;
  virtual void startListen() = 0;
  virtual void stopListen() = 0;
  virtual bool isListening() = 0;
};
