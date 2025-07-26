#ifndef WEAPON_MANAGER_H
#define WEAPON_MANAGER_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class WeaponManager : public Node3D {
    GDCLASS(WeaponManager, Node3D)

private:
    // Weapon sway settings
    double sway_intensity = 1.0;      // How much the weapon sways
    double sway_smoothness = 5.0;     // How smooth/fast the sway interpolation is
    double max_sway_distance = 0.1;   // Maximum distance weapon can sway
    
    // Current sway state
    Vector2 target_sway = Vector2(0.0, 0.0);    // Where weapon should sway to
    Vector2 current_sway = Vector2(0.0, 0.0);   // Current weapon sway position
    
    // Store original positions for all weapons
    Array weapon_children;           // All Node3D children
    Array original_positions;        // Their original positions
    
    // Mouse input tracking
    Vector2 mouse_input = Vector2(0.0, 0.0);

public:
    WeaponManager();
    ~WeaponManager();

    static void _bind_methods();

    void _ready() override;
    void _process(double delta) override;
    
    // Weapon sway methods
    void update_weapon_sway(double delta);
    void apply_mouse_input(Vector2 mouse_delta);
    void store_weapon_positions();
    
    // Weapon management
    int get_weapon_count() const { return weapon_children.size(); }
    
    // Property getters and setters
    double get_sway_intensity() const { return sway_intensity; }
    void set_sway_intensity(double p_intensity) { sway_intensity = p_intensity; }
    
    double get_sway_smoothness() const { return sway_smoothness; }
    void set_sway_smoothness(double p_smoothness) { sway_smoothness = p_smoothness; }
    
    double get_max_sway_distance() const { return max_sway_distance; }
    void set_max_sway_distance(double p_distance) { max_sway_distance = p_distance; }
};

}

#endif // WEAPON_MANAGER_H
