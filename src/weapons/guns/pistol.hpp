#ifndef PISTOL_H
#define PISTOL_H

#include "../weapon_manager.hpp"

namespace godot {

class Pistol : public WeaponManager {
    GDCLASS(Pistol, WeaponManager)

public:
    Pistol();
    ~Pistol();

    static void _bind_methods();
    void _ready() override;
    
    // Pistol-specific methods
    void setup_first_person_position();
};

} // namespace godot

#endif
