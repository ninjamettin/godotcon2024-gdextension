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
    WeaponManager::_ready();
    
    // Set pistol-specific properties
    set_fire_rate(8.0);                  // 8 shots per second
    set_animation_speed(1.0);            // Normal animation speed multiplier
    
    UtilityFunctions::print("Pistol: Setup complete with fire rate ", fire_rate);
}

void Pistol::setup_first_person_position() {
    // TODO: Position pistol for first-person view
    // This might include specific offset and rotation
    set_position(Vector3(0.297, -0.135, -0.303)); // Right-handed position
}
