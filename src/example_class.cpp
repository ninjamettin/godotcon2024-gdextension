#include "example_class.h"
#include "godot_cpp/variant/utility_functions.hpp"

ExampleClass::ExampleClass() {
	// TODO: Initialize your variables here
	// Example: position = Vector3(0, 0, 0);
	//          velocity = Vector3(0, 0, 0);
	//          mass = 1.0f;
}

void ExampleClass::_bind_methods() {
	// TODO: Bind your methods to Godot here
	// Example: godot::ClassDB::bind_method(D_METHOD("update_physics", "delta_time"), &ExampleClass::update_physics);
	//          godot::ClassDB::bind_method(D_METHOD("get_position"), &ExampleClass::get_position);
	//          godot::ClassDB::bind_method(D_METHOD("set_position", "pos"), &ExampleClass::set_position);
}

// TODO: Implement your physics methods here
// Example:
// void ExampleClass::update_physics(float delta_time) {
//     // Your physics calculations here
// }
//
// Vector3 ExampleClass::get_position() const {
//     return position;
// }
//
// void ExampleClass::set_position(Vector3 pos) {
//     position = pos;
// }
