#ifndef WEAPON_MANAGER_H
#define WEAPON_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

// Unified WeaponManager class that handles both weapon behavior and global effects
class WeaponManager : public Node3D {
    GDCLASS(WeaponManager, Node3D)

private:
    // Animation system
    AnimationPlayer* animation_player = nullptr;
    double animation_speed = 1.0;
    
    // Sway settings
    double sway_intensity = 2.0;
    double sway_smoothness = 5.0;
    bool enable_sway = true;
    // Bob settings
    double bob_intensity = 0.01;
    double bob_frequency = 1.8;
    bool enable_bob = true;
    
    // Current state
    Vector2 target_sway = Vector2(0.0, 0.0);
    Vector2 current_sway = Vector2(0.0, 0.0);
    double bob_time = 0.0;
    double bob_offset = 0.0;
    bool is_moving = false;
    double player_speed = 0.0; // Current player movement speed
    
    // Original position tracking
    Vector3 original_position;

public:
    WeaponManager();
    ~WeaponManager();
    
    static void _bind_methods();
    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;
    
    // Core weapon methods
    void fire();
    void setup_weapon_parts();
    void play_fire_animation();
    
    // Movement and input methods
    void apply_mouse_input(Vector2 mouse_delta);
    void set_movement_state(bool moving, double speed = 0.0);
    void handle_shoot_input(bool pressed);
    
    // Property getters/setters
    double get_sway_intensity() const { return sway_intensity; }
    void set_sway_intensity(double intensity) { sway_intensity = intensity; }
    double get_sway_smoothness() const { return sway_smoothness; }
    void set_sway_smoothness(double smoothness) { sway_smoothness = smoothness; }
    double get_bob_intensity() const { return bob_intensity; }
    void set_bob_intensity(double intensity) { bob_intensity = intensity; }
    double get_bob_frequency() const { return bob_frequency; }
    void set_bob_frequency(double frequency) { bob_frequency = frequency; }
    bool get_enable_sway() const { return enable_sway; }
    void set_enable_sway(bool enable) { enable_sway = enable; }
    bool get_enable_bob() const { return enable_bob; }
    void set_enable_bob(bool enable) { enable_bob = enable; }
    double get_animation_speed() const { return animation_speed; }
    void set_animation_speed(double speed) { animation_speed = speed; }

private:
    void update_sway(double delta);
    void update_bob(double delta);
    void store_position();
};

}

#endif // WEAPON_MANAGER_H
