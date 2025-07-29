#include "pistol.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

using namespace godot;

Pistol::Pistol() {
}

Pistol::~Pistol() {
}

void Pistol::_bind_methods() {
    // Bind fire rate methods
    ClassDB::bind_method(D_METHOD("get_fire_rate"), &Pistol::get_fire_rate);
    ClassDB::bind_method(D_METHOD("set_fire_rate", "rate"), &Pistol::set_fire_rate);
    
    // Expose fire_rate property to the editor
    ClassDB::add_property("Pistol", PropertyInfo(Variant::FLOAT, "fire_rate", PROPERTY_HINT_RANGE, "1.0,20.0,0.1"), "set_fire_rate", "get_fire_rate");
}

void Pistol::_ready() {
    WeaponManager::_ready();
    
    fire_rate = 8.0;
    set_animation_speed(1.0);
}

void Pistol::setup_first_person_position() {
    set_position(Vector3(0.297, -0.135, -0.303));
}
