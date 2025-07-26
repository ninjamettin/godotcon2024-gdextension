#include "pistol.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Pistol::Pistol() {
}

Pistol::~Pistol() {
}

void Pistol::_bind_methods() {
    // Call parent bind methods - individual weapon properties 
    // are exposed through the base Weapon class
}

void Pistol::_ready() {
    // Call parent ready first
    Weapon::_ready();
    
    // Customize pistol-specific stats
    set_damage(15.0);                    // Moderate damage
    set_fire_rate(8.0);                  // 8 shots per second
    set_max_ammo(12);                    // 12 round magazine
    set_recoil_duration(0.1);            // Quick recoil
    set_recoil_offset(Vector3(0, 0, 0.03)); // Light recoil movement
    
    // Set reload time and other stats
    reload_time = 1.5;                   // Fast reload
    projectile_speed = 80.0;             // High velocity
    range = 50.0;                        // Medium range
    
    // TODO: Load pistol-specific assets
    // - Weapon mesh
    // - Animation player
    // - Audio files
    // - Muzzle flash effects
}

void Pistol::setup_first_person_position() {
    // TODO: Position pistol for first-person view
    // This might include specific offset and rotation
    set_position(Vector3(0.3, -0.2, -0.5)); // Right-handed position
    set_rotation_degrees(Vector3(0, -5, 0)); // Slight angle
}
