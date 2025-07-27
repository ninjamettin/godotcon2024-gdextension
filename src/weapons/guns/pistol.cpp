#include "pistol.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Pistol::Pistol() {
}

Pistol::~Pistol() {
}

void Pistol::_bind_methods() {
    // No additional methods to bind for simplified pistol
}

void Pistol::_ready() {
    // Call parent ready first
    Weapon::_ready();
    
    // Set pistol-specific recoil amplifier
    set_recoil_amplifier(1.2); // Slightly higher recoil than default
    
    UtilityFunctions::print("Pistol: Ready with recoil amplifier ", get_recoil_amplifier());
}

void Pistol::setup_first_person_position() {
    // Position pistol for first-person view
    set_position(Vector3(0.3, -0.2, -0.5)); // Right-handed position
    set_rotation_degrees(Vector3(0, -5, 0)); // Slight angle
}
