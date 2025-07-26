#ifndef WEAPON_MANAGER_H
#define WEAPON_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

// Base Weapon class - to be inherited by specific weapon types
class Weapon : public Node3D {
    GDCLASS(Weapon, Node3D)

private:
    // Muzzle flash system
    Node3D* muzzle_flash_point = nullptr;  // Where muzzle flash appears
    MeshInstance3D* muzzle_flash_effect = nullptr; // The actual flash effect
    double muzzle_flash_duration = 0.1;    // How long flash lasts
    double muzzle_flash_timer = 0.0;       // Current flash timer
    bool muzzle_flash_active = false;      // Is flash currently showing
    
    // Recoil system
    Vector3 original_position = Vector3(0, 0, 0);
    double current_recoil_time = 0.0;      // Current recoil animation time
    bool is_in_recoil = false;             // Is weapon currently recoiling
    
    // Animation control
    String fire_animation_name = "fire";
    String reload_animation_name = "reload";
    bool interrupt_recoil_on_fire = true;  // Should new shots interrupt recoil

protected:
    // Weapon stats - to be set by individual weapon scripts
    double damage = 10.0;
    double fire_rate = 1.0;        // Shots per second
    int max_ammo = 30;
    int current_ammo = 30;
    double reload_time = 2.0;
    double projectile_speed = 50.0;
    double range = 100.0;
    
    // Recoil settings - to be customized per weapon
    Vector3 recoil_offset = Vector3(0, 0, 0.05);  // How far weapon moves back
    double recoil_duration = 0.15;        // How long recoil animation takes
    double recoil_recovery_speed = 8.0;    // How fast weapon returns to position
    
    // State
    bool is_firing = false;
    bool is_reloading = false;
    double fire_timer = 0.0;
    double reload_timer = 0.0;
    
    // Components
    MeshInstance3D* weapon_mesh = nullptr;
    AnimationPlayer* animation_player = nullptr;
    AudioStreamPlayer3D* audio_player = nullptr;

public:
    Weapon();
    ~Weapon();
    
    static void _bind_methods();
    
    void _ready() override;
    void _process(double delta) override;
    
    // Core weapon functions
    virtual bool can_fire() const;
    virtual void fire();
    virtual void reload();
    virtual void stop_firing();
    
    // Weapon positioning for first-person
    virtual void setup_first_person_position();
    virtual void play_fire_animation();
    virtual void play_reload_animation();
    
    // Getters/Setters
    double get_damage() const { return damage; }
    void set_damage(double p_damage) { damage = p_damage; }
    double get_fire_rate() const { return fire_rate; }
    void set_fire_rate(double p_rate) { fire_rate = p_rate; }
    int get_current_ammo() const { return current_ammo; }
    int get_max_ammo() const { return max_ammo; }
    void set_max_ammo(int p_ammo) { max_ammo = p_ammo; current_ammo = p_ammo; }
    bool get_is_reloading() const { return is_reloading; }
    double get_recoil_duration() const { return recoil_duration; }
    void set_recoil_duration(double p_duration) { recoil_duration = p_duration; }
    Vector3 get_recoil_offset() const { return recoil_offset; }
    void set_recoil_offset(Vector3 p_offset) { recoil_offset = p_offset; }

    // Muzzle flash methods
    void create_muzzle_flash();
    void update_muzzle_flash(double delta);
    Node3D* get_muzzle_position() const { return muzzle_flash_point; }
    
    // Recoil methods
    void start_recoil_animation();
    void update_recoil_animation(double delta);
    void interrupt_recoil();
    
    // Projectile firing method
    void fire_projectile();
};

// WeaponManager class - handles global weapon behaviors like sway and bob
class WeaponManager : public Node3D {
    GDCLASS(WeaponManager, Node3D)

private:
    // Weapon sway settings - global for all weapons
    double sway_intensity = 1.5;      // How much the weapon sways
    double sway_smoothness = 5.0;     // How smooth/fast the sway interpolation is
    double max_sway_distance = 0.1;   // Maximum distance weapon can sway
    
    // Weapon bob settings - global for all weapons
    double bob_intensity = 0.005;      // How much the weapon bobs up/down
    double bob_frequency = 0.8;       // How fast the bobbing oscillates (cycles per second)
    bool enable_bob = true;           // Whether bobbing is enabled
    
    // Current sway state
    Vector2 target_sway = Vector2(0.0, 0.0);    // Where weapon should sway to
    Vector2 current_sway = Vector2(0.0, 0.0);   // Current weapon sway position
    
    // Current bob state
    double bob_time = 0.0;            // Time accumulator for bob animation
    double bob_offset = 0.0;          // Current vertical bob offset
    bool is_moving = false;           // Whether player is currently moving
    
    // Store original positions for all weapons
    Array weapon_children;           // All Node3D children
    Array original_positions;        // Their original positions
    
    // Shooting state
    bool is_shooting = false;
    double last_shot_time = 0.0;
    
    // Mouse input tracking
    Vector2 mouse_input = Vector2(0.0, 0.0);

public:
    WeaponManager();
    ~WeaponManager();

    static void _bind_methods();

    void _ready() override;
    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;
    
    // Weapon sway methods
    void update_weapon_sway(double delta);
    void apply_mouse_input(Vector2 mouse_delta);
    
    // Weapon bob methods
    void update_weapon_bob(double delta);
    void set_movement_state(bool moving);
    void store_weapon_positions();
    
    // Weapon management
    int get_weapon_count() const { return weapon_children.size(); }
    Weapon* get_current_weapon() const;
    void handle_shoot_input(bool pressed);
    
    // Property getters and setters for sway
    double get_sway_intensity() const { return sway_intensity; }
    void set_sway_intensity(double p_intensity) { sway_intensity = p_intensity; }
    
    double get_sway_smoothness() const { return sway_smoothness; }
    void set_sway_smoothness(double p_smoothness) { sway_smoothness = p_smoothness; }
    
    double get_max_sway_distance() const { return max_sway_distance; }
    void set_max_sway_distance(double p_distance) { max_sway_distance = p_distance; }
    
    // Property getters and setters for bob
    double get_bob_intensity() const { return bob_intensity; }
    void set_bob_intensity(double p_intensity) { bob_intensity = p_intensity; }
    
    double get_bob_frequency() const { return bob_frequency; }
    void set_bob_frequency(double p_frequency) { bob_frequency = p_frequency; }
    
    bool get_enable_bob() const { return enable_bob; }
    void set_enable_bob(bool p_enable) { enable_bob = p_enable; }
};

}

#endif // WEAPON_MANAGER_H
