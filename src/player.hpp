#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Player : public CharacterBody3D {
    GDCLASS(Player, CharacterBody3D)

private:
    double speed = 5.0;
    double gravity = 9.82;
    double jump_velocity = 4.5;

public:
    Player() {}
    ~Player() {}

    static void _bind_methods();

    void _physics_process(double delta) override;
    
    // Property getters and setters
    double get_speed() const { return speed; }
    void set_speed(double p_speed) { speed = p_speed; }
    
    double get_jump_velocity() const { return jump_velocity; }
    void set_jump_velocity(double p_jump_velocity) { jump_velocity = p_jump_velocity; }
    
    double get_gravity() const { return gravity; }
    void set_gravity(double p_gravity) { gravity = p_gravity; }
};

}

#endif // PLAYER_H
