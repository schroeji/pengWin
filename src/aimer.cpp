#include "typedef.hpp"
#include "aimer.hpp"
#include "manager.hpp"
#include "clicker.hpp"


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <iostream>

using namespace std;
Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())){

  display= XOpenDisplay(NULL);
  root_window = XRootWindow(display, 0);
}

void Aimer::setAim(EntityType* enemy) {
}

// needs m_rawinput 0
void Aimer::xSetAim(EntityType* enemy) {
  if (enemy == nullptr)
    return;
  EntityType* local_player = csgo.getLocalPlayer();
  Vector player_pos = local_player->m_vecOrigin;
  Vector enemy_pos = enemy->m_vecOrigin;
  normalize_vector(&player_pos);
  normalize_vector(&enemy_pos);
  Vector dist = getDist(&player_pos, &enemy_pos);
  XSelectInput(display, root_window, KeyReleaseMask);
  XWarpPointer(display, None, root_window, 0, 0, 0, 0, 1000, 540);
  XFlush(display);
}
