#include "player.hpp"
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/engine.hpp>

using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    ClassDB::bind_method(D_METHOD("get_jump_velocity"), &Player::get_jump_velocity);
    ClassDB::bind_method(D_METHOD("set_jump_velocity", "jump_velocity"), &Player::set_jump_velocity);
    ClassDB::bind_method(D_METHOD("get_gravity"), &Player::get_gravity);
    ClassDB::bind_method(D_METHOD("set_gravity", "gravity"), &Player::set_gravity);
    ClassDB::bind_method(D_METHOD("get_camera_sensitivity"), &Player::get_camera_sensitivity);
    ClassDB::bind_method(D_METHOD("set_camera_sensitivity", "sensitivity"), &Player::set_camera_sensitivity);
    
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "jump_velocity"), "set_jump_velocity", "get_jump_velocity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "gravity"), "set_gravity", "get_gravity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "camera_sensitivity"), "set_camera_sensitivity", "get_camera_sensitivity");
}

void Player::_ready() {
    // Don't setup camera when in the editor
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    
    setup_camera();
    
    // Capture the mouse for first-person controls
    Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
}

void Player::_input(const Ref<InputEvent>& event) {
    // Don't process input when in the editor
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    
    // Handle mouse motion for camera rotation
    Ref<InputEventMouseMotion> mouse_motion = event;
    if (mouse_motion.is_valid()) {
        Vector2 relative = mouse_motion->get_relative();
        
        // Apply mouse sensitivity and accumulate rotation
        mouse_rotation.y -= relative.x * camera_sensitivity * 0.01; // Horizontal (yaw)
        mouse_rotation.x -= relative.y * camera_sensitivity * 0.01; // Vertical (pitch)
        
        // Clamp vertical rotation to prevent over-rotation
        mouse_rotation.x = CLAMP(mouse_rotation.x, Math::deg_to_rad(-max_pitch), Math::deg_to_rad(max_pitch));
        
        update_camera_rotation();
    }
    
    // Allow escape key to release mouse capture for testing
    Ref<InputEventKey> key_event = event;
    if (key_event.is_valid() && key_event->is_pressed() && key_event->get_keycode() == KEY_ESCAPE) {
        Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
    }
}

void Player::_physics_process(double delta) {
    // Don't process input when in the editor
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Vector3 velocity = get_velocity();

    // Gravity
    if (!is_on_floor())
        velocity.y -= gravity * delta;

    // Movement input - use direct keyboard input to avoid InputMap errors
    Input* input = Input::get_singleton();
    Vector2 input_dir = Vector2(
        (input->is_key_pressed(KEY_D) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_A) ? 1.0f : 0.0f),
        (input->is_key_pressed(KEY_W) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_S) ? 1.0f : 0.0f)
    ).normalized();

    // Use camera's forward direction for movement (only horizontal plane)
    Vector3 forward = Vector3(0, 0, -1);
    Vector3 right = Vector3(1, 0, 0);
    
    // Apply only horizontal rotation (yaw) to movement direction
    Transform3D horizontal_transform;
    horizontal_transform.basis = horizontal_transform.basis.rotated(Vector3(0, 1, 0), mouse_rotation.y);
    
    forward = horizontal_transform.basis.xform(forward);
    right = horizontal_transform.basis.xform(right);
    
    Vector3 direction = (forward * input_dir.y + right * input_dir.x).normalized();
    velocity.x = direction.x * speed;
    velocity.z = direction.z * speed;

    // Jump - use direct Space key input
    if (is_on_floor() && input->is_key_pressed(KEY_SPACE)) {
        velocity.y = jump_velocity;
    }

    set_velocity(velocity);
    move_and_slide();
}

void Player::setup_camera() {
    // Find a Camera3D child node
    camera = get_node<Camera3D>("Camera3D");
    if (!camera) {
        // If no Camera3D found, create one
        camera = memnew(Camera3D);
        add_child(camera);
        camera->set_name("Camera3D");
    }
    
    // Position camera at eye level (adjust as needed)
    camera->set_position(Vector3(0, 1.8, 0));
    
    // Initialize camera rotation
    update_camera_rotation();
}

void Player::update_camera_rotation() {
    if (!camera) return;
    
    // Apply pitch (vertical rotation) to the camera
    camera->set_rotation(Vector3(mouse_rotation.x, 0, 0));
    
    // Apply yaw (horizontal rotation) to the player body
    set_rotation(Vector3(0, mouse_rotation.y, 0));
}
