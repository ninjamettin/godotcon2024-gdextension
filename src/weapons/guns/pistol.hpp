#ifndef PISTOL_H
#define PISTOL_H

#include "../weapon_manager.hpp"

namespace godot {

class Pistol : public WeaponManager {
    GDCLASS(Pistol, WeaponManager)

private:
    double fire_rate = 5.0;

public:
    Pistol();
    ~Pistol();

    static void _bind_methods();
    void _ready() override;
    
    void setup_first_person_position();
    
    double get_fire_rate() const { return fire_rate; }
    void set_fire_rate(double rate) { fire_rate = rate; }


};

} // namespace godot

#endif
