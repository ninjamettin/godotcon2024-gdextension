#ifndef PLAYER_H
#define PLAYER_H

#include <godot_cpp/classes/character_body3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Player : public CharacterBody3D {
    GDCLASS(Player, CharacterBody3D)

private:
    double speed = 5.0;
    double gravity = 9.82;
    double jump_velocity = 4.5;
    double camera_sensitivity = 0.5;
    
    // Camera-related variables
    Camera3D* camera = nullptr;
    Vector2 mouse_rotation = Vector2(0.0, 0.0); // X = pitch, Y = yaw
    double max_pitch = 80.0; // Maximum vertical look angle in degrees

public:
    Player() {}
    ~Player() {}

    static void _bind_methods();

    void _ready() override;
    void _input(const Ref<InputEvent>& event) override;
    void _physics_process(double delta) override;
    
    // Camera methods
    void setup_camera();
    void update_camera_rotation();
    
    // Property getters and setters
    double get_speed() const { return speed; }
    void set_speed(double p_speed) { speed = p_speed; }
    
    double get_jump_velocity() const { return jump_velocity; }
    void set_jump_velocity(double p_jump_velocity) { jump_velocity = p_jump_velocity; }
    
    double get_gravity() const { return gravity; }
    void set_gravity(double p_gravity) { gravity = p_gravity; }
    
    double get_camera_sensitivity() const { return camera_sensitivity; }
    void set_camera_sensitivity(double p_sensitivity) { camera_sensitivity = p_sensitivity; }

     
};

}

#endif // PLAYER_H
