#include "player.hpp"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input_event_key.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_jump_velocity"), &Player::get_jump_velocity);
    ClassDB::bind_method(D_METHOD("set_jump_velocity", "jump_velocity"), &Player::set_jump_velocity);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
    ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &Player::set_gravity);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_velocity"), "set_jump_velocity", "get_jump_velocity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity", "get_gravity");
}

void Player::_physics_process(double delta) {
    Vector3 velocity = get_velocity();

    // Gravity
    if (!is_on_floor())
        velocity.y -= gravity * delta;

    // Movement input - use direct keyboard input to avoid InputMap errors
    Input* input = Input::get_singleton();
    Vector2 input_dir = Vector2(
        (input->is_key_pressed(KEY_D) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_A) ? 1.0f : 0.0f),
        (input->is_key_pressed(KEY_S) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_W) ? 1.0f : 0.0f)
    ).normalized();

    Vector3 direction = get_global_transform().basis.xform(Vector3(input_dir.x, 0, input_dir.y)).normalized();
    velocity.x = direction.x * speed;
    velocity.z = direction.z * speed;

    // Jump - use direct Space key input
    if (is_on_floor() && input->is_key_pressed(KEY_SPACE)) {
        velocity.y = jump_velocity;
    }

    set_velocity(velocity);
    move_and_slide();
}
