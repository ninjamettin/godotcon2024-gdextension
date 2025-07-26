#include "weapon_manager.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

WeaponManager::WeaponManager() {
    // Constructor - initialize default values if needed
}

WeaponManager::~WeaponManager() {
    // Destructor
}

void WeaponManager::_bind_methods() {
    // Bind the public methods
    ClassDB::bind_method(D_METHOD("apply_mouse_input", "mouse_delta"), &WeaponManager::apply_mouse_input);
    ClassDB::bind_method(D_METHOD("get_weapon_count"), &WeaponManager::get_weapon_count);
    ClassDB::bind_method(D_METHOD("store_weapon_positions"), &WeaponManager::store_weapon_positions);
    
    // Bind property getters and setters
    ClassDB::bind_method(D_METHOD("get_sway_intensity"), &WeaponManager::get_sway_intensity);
    ClassDB::bind_method(D_METHOD("set_sway_intensity", "intensity"), &WeaponManager::set_sway_intensity);
    ClassDB::bind_method(D_METHOD("get_sway_smoothness"), &WeaponManager::get_sway_smoothness);
    ClassDB::bind_method(D_METHOD("set_sway_smoothness", "smoothness"), &WeaponManager::set_sway_smoothness);
    ClassDB::bind_method(D_METHOD("get_max_sway_distance"), &WeaponManager::get_max_sway_distance);
    ClassDB::bind_method(D_METHOD("set_max_sway_distance", "distance"), &WeaponManager::set_max_sway_distance);
    
    // Add properties to the inspector
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sway_intensity"), "set_sway_intensity", "get_sway_intensity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sway_smoothness"), "set_sway_smoothness", "get_sway_smoothness");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_sway_distance"), "set_max_sway_distance", "get_max_sway_distance");
}

void WeaponManager::_ready() {
    // Find all Node3D children and store their positions
    UtilityFunctions::print("WeaponManager: Found ", get_child_count(), " children");
    
    store_weapon_positions();
}

void WeaponManager::store_weapon_positions() {
    weapon_children.clear();
    original_positions.clear();
    
    for (int i = 0; i < get_child_count(); i++) {
        Node3D* child = Object::cast_to<Node3D>(get_child(i));
        if (child) {
            UtilityFunctions::print("WeaponManager: Found weapon child: ", child->get_name());
            weapon_children.append(child);
            original_positions.append(child->get_position());
        }
    }
    
    UtilityFunctions::print("WeaponManager: Tracking ", weapon_children.size(), " weapons for sway");
}

void WeaponManager::_process(double delta) {
    // Update weapon sway every frame
    update_weapon_sway(delta);
}

void WeaponManager::update_weapon_sway(double delta) {
    if (weapon_children.size() == 0) return;
    
    // Smoothly interpolate current sway towards target sway
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    // Apply sway to ALL weapon children
    Vector3 sway_offset = Vector3(current_sway.x, -current_sway.y, 0.0);
    
    for (int i = 0; i < weapon_children.size(); i++) {
        Node3D* weapon = Object::cast_to<Node3D>(weapon_children[i]);
        Vector3 original_pos = original_positions[i];
        
        if (weapon) {
            Vector3 new_position = original_pos + sway_offset;
            weapon->set_position(new_position);
        }
    }
    
    // Debug output (remove this later)
    static double debug_timer = 0.0;
    debug_timer += delta;
    if (debug_timer > 1.0) { // Print every second
        //UtilityFunctions::print("Sway - Target: ", target_sway, " Current: ", current_sway, " Weapons: ", weapon_children.size());
        debug_timer = 0.0;
    }
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    // Debug: Check if mouse input is reaching this function
    //UtilityFunctions::print("WeaponManager: Received mouse input: ", mouse_delta);
    
    // Calculate target sway based on mouse input
    // Weapon sways in OPPOSITE direction of mouse movement
    Vector2 sway_input = -mouse_delta * sway_intensity * 0.001; // Scale down mouse input
    
    // Clamp sway to maximum distance
    if (sway_input.length() > max_sway_distance) {
        sway_input = sway_input.normalized() * max_sway_distance;
    }
    
    target_sway = sway_input;
}
