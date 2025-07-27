#ifndef PISTOL_H
#define PISTOL_H

#include "../weapon_manager.hpp"

namespace godot {

class Pistol : public WeaponManager {
    GDCLASS(Pistol, WeaponManager)

private:
    double fire_rate = 5.0; // Default fire rate
public:
    Pistol();
    ~Pistol();

    static void _bind_methods();
    void _ready() override;
    
    // Pistol-specific methods
    void setup_first_person_position();
    
    // Fire rate property
    double get_fire_rate() const { return fire_rate; }
    void set_fire_rate(double rate) { fire_rate = rate; }


};

} // namespace godot

#endif
