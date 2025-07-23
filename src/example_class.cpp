#include "example_class.h"
#include "godot_cpp/variant/utility_functions.hpp"

ExampleClass::ExampleClass() {
	planet_position = Vector3(0, 0, 0);
	planet_mass = 5000.0f;
	planet_radius = 0.6f;  // Slightly larger than visual radius
	min_distance = planet_radius + 0.1f;  // Minimum safe distance
	satellite_position = Vector3(5, 0, 0);
	satellite_velocity = Vector3(0, 0, 3.16f);  // Circular orbit velocity
	satellite_mass = 1.0f;
	gravitational_constant = 50.0f;
}

void ExampleClass::_bind_methods() {
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
