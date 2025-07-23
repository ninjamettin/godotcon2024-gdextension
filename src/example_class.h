#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

class ExampleClass : public RefCounted {
	GDCLASS(ExampleClass, RefCounted)

private:
	// TODO: Add your physics variables here
	// Example: Vector3 position, Vector3 velocity, float mass, etc.

protected:
	static void _bind_methods();

public:
	ExampleClass();
	~ExampleClass() override = default;

	// TODO: Add your physics methods here
	// Example: void update_physics(float delta_time);
	//          Vector3 get_position() const;
	//          void set_position(Vector3 pos);
};
