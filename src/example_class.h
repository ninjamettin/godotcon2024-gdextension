#pragma once

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector3.hpp"
#include "godot_cpp/variant/array.hpp"

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
	
	// UI Parameters
	float ui_distance;
	Vector3 ui_velocity;
	float ui_magnitude;
	Vector3 ui_direction;

protected:
	static void _bind_methods();

public:
	ExampleClass();
	~ExampleClass() override = default;

	// Core physics methods
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
	
	// UI Parameter methods
	void set_ui_distance(float distance);
	void set_ui_velocity_magnitude(float magnitude);
	void set_ui_velocity_direction(Vector3 direction);
	void apply_ui_parameters();
	float get_ui_distance() const;
	float get_ui_velocity_magnitude() const;
	Vector3 get_ui_velocity_direction() const;
	Vector3 get_ui_velocity() const;
	
	// Preset orbits
	void set_circular_orbit(float distance);
	void set_elliptical_orbit(float distance, float eccentricity);
	void set_inclined_orbit(float distance, float inclination);
	void reset_to_default();
	
	// Orbital calculations
	float calculate_circular_velocity(float distance) const;
	Vector3 calculate_orbit_position(float distance, float angle, float inclination) const;
};
