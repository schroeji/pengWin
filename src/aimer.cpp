#include "typedef.hpp"
#include "aimer.hpp"
#include "manager.hpp"
#include "clicker.hpp"

Aimer::Aimer(GameManager& csgo) : csgo(csgo),
                                  mem(csgo.getMemoryAccess()),
                                  clicker(Clicker(csgo.getMemoryAccess())){}

void Aimer::setAim(EntityType*) {


}
