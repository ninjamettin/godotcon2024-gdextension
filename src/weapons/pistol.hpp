#ifndef PISTOL_H
#define PISTOL_H

#include "weapon_manager.hpp"

namespace godot {

class Pistol : public Weapon {
    GDCLASS(Pistol, Weapon)

public:
    Pistol();
    ~Pistol();
    
    static void _bind_methods();
    void _ready() override;
    
    // Override weapon behavior if needed
    void setup_first_person_position() override;
};

}

#endif
