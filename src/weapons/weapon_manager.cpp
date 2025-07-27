#include "weapon_manager.hpp"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

using namespace godot;

WeaponManager::WeaponManager() {
    // Initialize all properties with default values
    animation_speed = 1.0;
    fire_rate = 5.0;
    
    sway_intensity = 1.0;
    sway_smoothness = 5.0;
    enable_sway = true;
    bob_intensity = 0.005;
    bob_frequency = 0.8;
    enable_bob = true;
    player_speed = 0.0;
}

WeaponManager::~WeaponManager() {}

void WeaponManager::_bind_methods() {
    // Core weapon methods
    ClassDB::bind_method(D_METHOD("fire"), &WeaponManager::fire);
    ClassDB::bind_method(D_METHOD("setup_weapon_parts"), &WeaponManager::setup_weapon_parts);
    ClassDB::bind_method(D_METHOD("play_fire_animation"), &WeaponManager::play_fire_animation);
    
    // Movement and input methods
    ClassDB::bind_method(D_METHOD("apply_mouse_input", "mouse_delta"), &WeaponManager::apply_mouse_input);
    ClassDB::bind_method(D_METHOD("set_movement_state", "moving", "speed"), &WeaponManager::set_movement_state);
    ClassDB::bind_method(D_METHOD("handle_shoot_input", "pressed"), &WeaponManager::handle_shoot_input);
    
    // Property getters/setters
    ClassDB::bind_method(D_METHOD("get_sway_intensity"), &WeaponManager::get_sway_intensity);
    ClassDB::bind_method(D_METHOD("set_sway_intensity", "intensity"), &WeaponManager::set_sway_intensity);
    ClassDB::bind_method(D_METHOD("get_sway_smoothness"), &WeaponManager::get_sway_smoothness);
    ClassDB::bind_method(D_METHOD("set_sway_smoothness", "smoothness"), &WeaponManager::set_sway_smoothness);
    ClassDB::bind_method(D_METHOD("get_bob_intensity"), &WeaponManager::get_bob_intensity);
    ClassDB::bind_method(D_METHOD("set_bob_intensity", "intensity"), &WeaponManager::set_bob_intensity);
    ClassDB::bind_method(D_METHOD("get_bob_frequency"), &WeaponManager::get_bob_frequency);
    ClassDB::bind_method(D_METHOD("set_bob_frequency", "frequency"), &WeaponManager::set_bob_frequency);
    ClassDB::bind_method(D_METHOD("get_enable_sway"), &WeaponManager::get_enable_sway);
    ClassDB::bind_method(D_METHOD("set_enable_sway", "enable"), &WeaponManager::set_enable_sway);
    ClassDB::bind_method(D_METHOD("get_enable_bob"), &WeaponManager::get_enable_bob);
    ClassDB::bind_method(D_METHOD("set_enable_bob", "enable"), &WeaponManager::set_enable_bob);
    ClassDB::bind_method(D_METHOD("get_animation_speed"), &WeaponManager::get_animation_speed);
    ClassDB::bind_method(D_METHOD("set_animation_speed", "speed"), &WeaponManager::set_animation_speed);
    ClassDB::bind_method(D_METHOD("get_fire_rate"), &WeaponManager::get_fire_rate);
    ClassDB::bind_method(D_METHOD("set_fire_rate", "rate"), &WeaponManager::set_fire_rate);
    
    // Expose properties to the editor
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "fire_rate", PROPERTY_HINT_RANGE, "1.0,20.0,0.1"), "set_fire_rate", "get_fire_rate");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "animation_speed", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_animation_speed", "get_animation_speed");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_intensity", PROPERTY_HINT_RANGE, "0.0,15.0,0.1"), "set_sway_intensity", "get_sway_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_smoothness", PROPERTY_HINT_RANGE, "0.1,20.0,0.1"), "set_sway_smoothness", "get_sway_smoothness");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_sway"), "set_enable_sway", "get_enable_sway");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_intensity", PROPERTY_HINT_RANGE, "0.0,0.1,0.001"), "set_bob_intensity", "get_bob_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_frequency", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_bob_frequency", "get_bob_frequency");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_bob"), "set_enable_bob", "get_enable_bob");
}

void WeaponManager::_ready() {
    setup_weapon_parts();
    store_position();
}

void WeaponManager::_process(double delta) {
    if (enable_sway) update_sway(delta);
    if (enable_bob) update_bob(delta);
}

void WeaponManager::_input(const Ref<InputEvent>& event) {
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid() && mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
        handle_shoot_input(mouse_button->is_pressed());
    }
    
    // Handle mouse motion for sway
    Ref<InputEventMouseMotion> mouse_motion = event;
    if (mouse_motion.is_valid() && enable_sway) {
        apply_mouse_input(mouse_motion->get_relative());
    }
}

void WeaponManager::setup_weapon_parts() {
    UtilityFunctions::print("WeaponManager: Starting setup for node: ", get_name());
    


    // Find AnimationPlayer in P2262 child node
    Node* pistol_model = get_node_or_null("P2262");
    if (pistol_model) {
        UtilityFunctions::print("WeaponManager: Found P2262 node");
        animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
    } else {
        UtilityFunctions::print("WeaponManager: No P2262 node found");
    }
    
    if (animation_player) {
        UtilityFunctions::print("WeaponManager: Found AnimationPlayer");
        PackedStringArray animations = animation_player->get_animation_list();
        UtilityFunctions::print("WeaponManager: Available animations: ", animations);
    } else {
        UtilityFunctions::print("WeaponManager: AnimationPlayer not found");
    }
    
    UtilityFunctions::print("WeaponManager: Setup complete");
}

void WeaponManager::fire() {
    UtilityFunctions::print("WeaponManager: Fire called");
    play_fire_animation();
}

void WeaponManager::play_fire_animation() {
    if (animation_player) {
        // Calculate animation speed based on fire rate
        // Higher fire rate = faster animation
        double calculated_speed = animation_speed * (fire_rate / 5.0); // 5.0 is base fire rate
        animation_player->set_speed_scale(calculated_speed);
        
        // Try common animation names
        if (animation_player->has_animation("pistol_shoot")) {
            animation_player->play("pistol_shoot");
            UtilityFunctions::print("WeaponManager: Playing 'pistol_shoot' at speed ", calculated_speed);
        } else if (animation_player->has_animation("shoot")) {
            animation_player->play("shoot");
            UtilityFunctions::print("WeaponManager: Playing 'shoot' at speed ", calculated_speed);
        } else if (animation_player->has_animation("Fire")) {
            animation_player->play("Fire");
            UtilityFunctions::print("WeaponManager: Playing 'Fire' at speed ", calculated_speed);
        } else {
            // Play the first animation if any exist
            PackedStringArray animations = animation_player->get_animation_list();
            if (animations.size() > 0) {
                String first_anim = animations[0];
                animation_player->play(first_anim);
                UtilityFunctions::print("WeaponManager: Playing first animation '", first_anim, "' at speed ", calculated_speed);
            }
        }
    } else {
        UtilityFunctions::print("WeaponManager: No AnimationPlayer found");
    }
}




void WeaponManager::handle_shoot_input(bool pressed) {
    if (!pressed) return;
    fire();
}

void WeaponManager::store_position() {
    original_position = get_position();
    UtilityFunctions::print("WeaponManager: Stored original position: ", original_position);
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    if (!enable_sway) return;
    
    // Horizontal sway should move opposite to mouse movement (realistic lag effect)
    // Vertical sway should also move opposite to mouse movement (look up = gun down)
    Vector2 sway_delta = Vector2(-mouse_delta.x, -mouse_delta.y) * sway_intensity * 0.01;
    target_sway = sway_delta;
}

void WeaponManager::update_sway(double delta) {
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    // Apply both horizontal and vertical sway
    Vector3 sway_offset = Vector3(current_sway.x, current_sway.y, 0);
    set_position(original_position + sway_offset);
    
    // Slower decay so sway is more noticeable
    target_sway = target_sway.lerp(Vector2(0, 0), delta * 0.5);
}

void WeaponManager::set_movement_state(bool moving, double speed) {
    is_moving = moving;
    player_speed = speed;
}

void WeaponManager::update_bob(double delta) {
    if (is_moving) {
        // Bob frequency increases with player speed
        double speed_multiplier = Math::max(1.0, player_speed / 5.0); // Normalize to walking speed of 5
        double effective_frequency = bob_frequency * speed_multiplier;
        
        bob_time += delta * effective_frequency;
        bob_offset = sin(bob_time) * bob_intensity;
    } else {
        bob_offset = Math::lerp(bob_offset, 0.0, delta * 5.0);
    }
    
    Vector3 current_pos = get_position();
    current_pos.y = original_position.y + bob_offset;
    set_position(current_pos);
}
