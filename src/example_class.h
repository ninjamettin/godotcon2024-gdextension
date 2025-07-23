#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"

using namespace godot;

class ExampleClass : public RefCounted {
	GDCLASS(ExampleClass, RefCounted)

private:
	Vector3 planet_position;
	float planet_mass;
	Vector3 satellite_position;
	Vector3 satellite_velocity;
	float satellite_mass;
	float gravitational_constant;
	float planet_radius;
	float min_distance;

protected:
	static void _bind_methods();

public:
	ExampleClass();
	~ExampleClass() override = default;

	void set_planet(Vector3 position, float mass, float radius);
	void set_satellite(Vector3 position, Vector3 velocity, float mass);
	void set_gravitational_constant(float g);
	void update_physics(float delta_time);
	Vector3 get_satellite_position() const;
	Vector3 get_satellite_velocity() const;
	Vector3 get_planet_position() const;
	Vector3 calculate_gravitational_force() const;
	bool is_satellite_colliding() const;
	float get_distance_to_planet() const;
};
