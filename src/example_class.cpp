#include "example_class.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cmath>

ExampleClass::ExampleClass() {
	planet_position = Vector3(0, 0, 0);
	planet_mass = 5000.0f;
	planet_radius = 0.6f;
	min_distance = planet_radius + 0.1f;
	satellite_position = Vector3(5, 0, 0);
	satellite_velocity = Vector3(0, 0, 3.16f);
	satellite_mass = 1.0f;
	gravitational_constant = 50.0f;
	
	// Initialize UI parameters
	ui_distance = 5.0f;
	ui_magnitude = 3.16f;
	ui_direction = Vector3(0, 0, 1);
	ui_velocity = ui_direction * ui_magnitude;
}

void ExampleClass::_bind_methods() {
	// Core physics methods
	godot::ClassDB::bind_method(D_METHOD("set_planet", "position", "mass", "radius"), &ExampleClass::set_planet);
	godot::ClassDB::bind_method(D_METHOD("set_satellite", "position", "velocity", "mass"), &ExampleClass::set_satellite);
	godot::ClassDB::bind_method(D_METHOD("set_gravitational_constant", "g"), &ExampleClass::set_gravitational_constant);
	godot::ClassDB::bind_method(D_METHOD("update_physics", "delta_time"), &ExampleClass::update_physics);
	godot::ClassDB::bind_method(D_METHOD("get_satellite_position"), &ExampleClass::get_satellite_position);
	godot::ClassDB::bind_method(D_METHOD("get_satellite_velocity"), &ExampleClass::get_satellite_velocity);
	godot::ClassDB::bind_method(D_METHOD("get_planet_position"), &ExampleClass::get_planet_position);
	godot::ClassDB::bind_method(D_METHOD("calculate_gravitational_force"), &ExampleClass::calculate_gravitational_force);
	godot::ClassDB::bind_method(D_METHOD("is_satellite_colliding"), &ExampleClass::is_satellite_colliding);
	godot::ClassDB::bind_method(D_METHOD("get_distance_to_planet"), &ExampleClass::get_distance_to_planet);
	
	// UI Parameter methods
	godot::ClassDB::bind_method(D_METHOD("set_ui_distance", "distance"), &ExampleClass::set_ui_distance);
	godot::ClassDB::bind_method(D_METHOD("set_ui_velocity_magnitude", "magnitude"), &ExampleClass::set_ui_velocity_magnitude);
	godot::ClassDB::bind_method(D_METHOD("set_ui_velocity_direction", "direction"), &ExampleClass::set_ui_velocity_direction);
	godot::ClassDB::bind_method(D_METHOD("apply_ui_parameters"), &ExampleClass::apply_ui_parameters);
	godot::ClassDB::bind_method(D_METHOD("get_ui_distance"), &ExampleClass::get_ui_distance);
	godot::ClassDB::bind_method(D_METHOD("get_ui_velocity_magnitude"), &ExampleClass::get_ui_velocity_magnitude);
	godot::ClassDB::bind_method(D_METHOD("get_ui_velocity_direction"), &ExampleClass::get_ui_velocity_direction);
	godot::ClassDB::bind_method(D_METHOD("get_ui_velocity"), &ExampleClass::get_ui_velocity);
	
	// Preset orbits
	godot::ClassDB::bind_method(D_METHOD("set_circular_orbit", "distance"), &ExampleClass::set_circular_orbit);
	godot::ClassDB::bind_method(D_METHOD("set_elliptical_orbit", "distance", "eccentricity"), &ExampleClass::set_elliptical_orbit);
	godot::ClassDB::bind_method(D_METHOD("set_inclined_orbit", "distance", "inclination"), &ExampleClass::set_inclined_orbit);
	godot::ClassDB::bind_method(D_METHOD("reset_to_default"), &ExampleClass::reset_to_default);
	
	// Orbital calculations
	godot::ClassDB::bind_method(D_METHOD("calculate_circular_velocity", "distance"), &ExampleClass::calculate_circular_velocity);
	godot::ClassDB::bind_method(D_METHOD("calculate_orbit_position", "distance", "angle", "inclination"), &ExampleClass::calculate_orbit_position);
}

void ExampleClass::set_planet(Vector3 position, float mass, float radius) {
	planet_position = position;
	planet_mass = mass;
	planet_radius = radius;
	min_distance = planet_radius + 0.1f;
}

void ExampleClass::set_satellite(Vector3 position, Vector3 velocity, float mass) {
	satellite_position = position;
	satellite_velocity = velocity;
	satellite_mass = mass;
}

void ExampleClass::set_gravitational_constant(float g) {
	gravitational_constant = g;
}

Vector3 ExampleClass::calculate_gravitational_force() const {
	Vector3 direction = planet_position - satellite_position;
	float distance = direction.length();
	
	// Prevent satellite from getting too close to planet surface
	if (distance < min_distance) {
		distance = min_distance;
	}
	
	float distance_squared = distance * distance;
	float force_magnitude = (gravitational_constant * planet_mass * satellite_mass) / distance_squared;
	return direction.normalized() * force_magnitude;
}

bool ExampleClass::is_satellite_colliding() const {
	float distance = (satellite_position - planet_position).length();
	return distance <= planet_radius;
}

float ExampleClass::get_distance_to_planet() const {
	return (satellite_position - planet_position).length();
}

void ExampleClass::update_physics(float delta_time) {
	float distance = get_distance_to_planet();
	
	// If satellite collides with planet, handle collision
	if (distance <= planet_radius) {
		// Move satellite to planet surface
		Vector3 direction = (satellite_position - planet_position).normalized();
		satellite_position = planet_position + direction * (planet_radius + 0.01f);
		
		// Reflect velocity for bouncing effect (optional)
		Vector3 normal = direction;
		satellite_velocity = satellite_velocity - 2.0f * satellite_velocity.dot(normal) * normal;
		satellite_velocity *= 0.7f; // Energy loss on collision
		
		return;
	}
	
	// Normal physics update
	Vector3 force = calculate_gravitational_force();
	Vector3 acceleration = force / satellite_mass;
	
	// Use smaller time steps for better stability
	float sub_delta = delta_time / 4.0f;
	for (int i = 0; i < 4; i++) {
		satellite_velocity += acceleration * sub_delta;
		satellite_position += satellite_velocity * sub_delta;
		
		// Recalculate force for next sub-step
		if (i < 3) {
			force = calculate_gravitational_force();
			acceleration = force / satellite_mass;
		}
	}
}

Vector3 ExampleClass::get_satellite_position() const {
	return satellite_position;
}

Vector3 ExampleClass::get_satellite_velocity() const {
	return satellite_velocity;
}

Vector3 ExampleClass::get_planet_position() const {
	return planet_position;
}

// UI Parameter methods
void ExampleClass::set_ui_distance(float distance) {
	ui_distance = Math::max(distance, min_distance);
}

void ExampleClass::set_ui_velocity_magnitude(float magnitude) {
	ui_magnitude = Math::max(magnitude, 0.0f);
	ui_velocity = ui_direction.normalized() * ui_magnitude;
}

void ExampleClass::set_ui_velocity_direction(Vector3 direction) {
	ui_direction = direction.normalized();
	ui_velocity = ui_direction * ui_magnitude;
}

void ExampleClass::apply_ui_parameters() {
	Vector3 start_position = Vector3(ui_distance, 0, 0);
	set_satellite(start_position, ui_velocity, satellite_mass);
}

float ExampleClass::get_ui_distance() const {
	return ui_distance;
}

float ExampleClass::get_ui_velocity_magnitude() const {
	return ui_magnitude;
}

Vector3 ExampleClass::get_ui_velocity_direction() const {
	return ui_direction;
}

Vector3 ExampleClass::get_ui_velocity() const {
	return ui_velocity;
}

// Preset orbits
void ExampleClass::set_circular_orbit(float distance) {
	ui_distance = Math::max(distance, min_distance);
	ui_magnitude = calculate_circular_velocity(ui_distance);
	ui_direction = Vector3(0, 0, 1);
	ui_velocity = ui_direction * ui_magnitude;
	apply_ui_parameters();
}

void ExampleClass::set_elliptical_orbit(float distance, float eccentricity) {
	ui_distance = Math::max(distance, min_distance);
	// For elliptical orbit, increase velocity by eccentricity factor
	ui_magnitude = calculate_circular_velocity(ui_distance) * (1.0f + eccentricity * 0.5f);
	ui_direction = Vector3(0, eccentricity * 0.3f, Math::sqrt(1.0f - (eccentricity * 0.3f) * (eccentricity * 0.3f)));
	ui_velocity = ui_direction * ui_magnitude;
	apply_ui_parameters();
}

void ExampleClass::set_inclined_orbit(float distance, float inclination) {
	ui_distance = Math::max(distance, min_distance);
	ui_magnitude = calculate_circular_velocity(ui_distance);
	// Create inclined orbit by rotating velocity vector
	float inclination_rad = inclination * Math_PI / 180.0f;
	ui_direction = Vector3(Math::sin(inclination_rad) * 0.5f, Math::sin(inclination_rad), Math::cos(inclination_rad));
	ui_velocity = ui_direction.normalized() * ui_magnitude;
	apply_ui_parameters();
}

void ExampleClass::reset_to_default() {
	ui_distance = 5.0f;
	ui_magnitude = 3.16f;
	ui_direction = Vector3(0, 0, 1);
	ui_velocity = ui_direction * ui_magnitude;
	apply_ui_parameters();
}

// Orbital calculations
float ExampleClass::calculate_circular_velocity(float distance) const {
	if (distance < min_distance) {
		distance = min_distance;
	}
	// v = sqrt(GM/r)
	return Math::sqrt(gravitational_constant * planet_mass / distance);
}

Vector3 ExampleClass::calculate_orbit_position(float distance, float angle, float inclination) const {
	float angle_rad = angle * Math_PI / 180.0f;
	float inclination_rad = inclination * Math_PI / 180.0f;
	
	// Calculate position in orbital plane
	float x = distance * Math::cos(angle_rad);
	float y = distance * Math::sin(angle_rad) * Math::sin(inclination_rad);
	float z = distance * Math::sin(angle_rad) * Math::cos(inclination_rad);
	
	return Vector3(x, y, z) + planet_position;
}
