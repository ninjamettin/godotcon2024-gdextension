#ifndef WEAPON_MANAGER_H
#define WEAPON_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

// Simple Weapon class for recoil animation
class Weapon : public Node3D {
    GDCLASS(Weapon, Node3D)

private:
    // Pistol part references
    Node3D* pistol_slide = nullptr;
    Node3D* pistol_hammer = nullptr;
    Node3D* pistol_trigger = nullptr;
    Node3D* pistol_root = nullptr;
    
    // Animation system
    AnimationPlayer* animation_player = nullptr;
    
    // Recoil system
    double recoil_amplifier = 1.0;
    bool is_in_recoil = false;
    double current_recoil_time = 0.0;
    
    // Base recoil values
    Vector3 base_slide_distance = Vector3(0, 0, -0.03);
    Vector3 base_hammer_rotation = Vector3(-3.0, 0, 0);
    Vector3 base_trigger_pull = Vector3(0, 0, -0.008);
    Vector3 base_weapon_kick = Vector3(1.5, 0, 0);
    
    double recoil_duration = 0.3;

public:
    Weapon();
    ~Weapon();
    
    static void _bind_methods();
    void _ready() override;
    void _process(double delta) override;
    
    void fire();
    void setup_pistol_parts();
    void play_recoil_animation();
    void update_recoil(double delta);
    void reset_parts();
    void play_fire_animation();
    
    // Recoil control
    double get_recoil_amplifier() const { return recoil_amplifier; }
    void set_recoil_amplifier(double amplifier) { recoil_amplifier = amplifier; }
};

// Simple WeaponManager for sway, bob, and recoil control
class WeaponManager : public Node3D {
    GDCLASS(WeaponManager, Node3D)

private:
    // Sway settings
    double sway_intensity = 1.0;
    double sway_smoothness = 5.0;
    bool enable_sway = true;
    
    // Bob settings
    double bob_intensity = 0.005;
    double bob_frequency = 0.8;
    bool enable_bob = true;
    
    // Current state
    Vector2 target_sway = Vector2(0.0, 0.0);
    Vector2 current_sway = Vector2(0.0, 0.0);
    double bob_time = 0.0;
    double bob_offset = 0.0;
    bool is_moving = false;
    
    // Weapons
    Array weapon_children;
    Array original_positions;

public:
    WeaponManager();
    ~WeaponManager();
    
    static void _bind_methods();
    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;
    
    // Core methods
    void apply_mouse_input(Vector2 mouse_delta);
    void set_movement_state(bool moving);
    void handle_shoot_input(bool pressed);
    void set_weapon_recoil_amplifier(int weapon_index, double amplifier);
    
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

private:
    void update_sway(double delta);
    void update_bob(double delta);
    void store_positions();
};

}

#endif // WEAPON_MANAGER_H
