#include "weapon_manager.hpp"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

using namespace godot;

WeaponManager::WeaponManager() {
    // Initialize all properties with default values
    recoil_amplifier = 1.0;
    is_in_recoil = false;
    current_recoil_time = 0.0;
    animation_speed = 1.0;
    
    sway_intensity = 1.0;
    sway_smoothness = 5.0;
    enable_sway = true;
    bob_intensity = 0.005;
    bob_frequency = 0.8;
    enable_bob = true;
}

WeaponManager::~WeaponManager() {}

void WeaponManager::_bind_methods() {
    // Core weapon methods
    ClassDB::bind_method(D_METHOD("fire"), &WeaponManager::fire);
    ClassDB::bind_method(D_METHOD("setup_weapon_parts"), &WeaponManager::setup_weapon_parts);
    ClassDB::bind_method(D_METHOD("play_fire_animation"), &WeaponManager::play_fire_animation);
    
    // Movement and input methods
    ClassDB::bind_method(D_METHOD("apply_mouse_input", "mouse_delta"), &WeaponManager::apply_mouse_input);
    ClassDB::bind_method(D_METHOD("set_movement_state", "moving"), &WeaponManager::set_movement_state);
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
    ClassDB::bind_method(D_METHOD("get_recoil_amplifier"), &WeaponManager::get_recoil_amplifier);
    ClassDB::bind_method(D_METHOD("set_recoil_amplifier", "amplifier"), &WeaponManager::set_recoil_amplifier);
    ClassDB::bind_method(D_METHOD("get_animation_speed"), &WeaponManager::get_animation_speed);
    ClassDB::bind_method(D_METHOD("set_animation_speed", "speed"), &WeaponManager::set_animation_speed);
    
    // Expose properties to the editor
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "animation_speed", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_animation_speed", "get_animation_speed");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "recoil_amplifier", PROPERTY_HINT_RANGE, "0.1,3.0,0.1"), "set_recoil_amplifier", "get_recoil_amplifier");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_intensity", PROPERTY_HINT_RANGE, "0.0,5.0,0.1"), "set_sway_intensity", "get_sway_intensity");
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
    if (is_in_recoil) {
        update_recoil(delta);
    }
    if (enable_sway) update_sway(delta);
    if (enable_bob) update_bob(delta);
}

void WeaponManager::_input(const Ref<InputEvent>& event) {
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid() && mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
        handle_shoot_input(mouse_button->is_pressed());
    }
}

void WeaponManager::setup_weapon_parts() {
    UtilityFunctions::print("WeaponManager: Starting setup for node: ", get_name());
    
    // Find pistol parts in the scene
    pistol_root = this;
    pistol_slide   = Object::cast_to<MeshInstance3D>(get_node_or_null("Slide"));
    pistol_hammer  = Object::cast_to<MeshInstance3D>(get_node_or_null("Hammer"));
    pistol_trigger = Object::cast_to<MeshInstance3D>(get_node_or_null("Trigger"));

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
    play_recoil_animation();
    play_fire_animation();
}

void WeaponManager::play_fire_animation() {
    if (animation_player) {
        // Set the animation speed before playing
        animation_player->set_speed_scale(animation_speed);
        
        // Try common animation names
        if (animation_player->has_animation("pistol_shoot")) {
            animation_player->play("pistol_shoot");
            UtilityFunctions::print("WeaponManager: Playing 'pistol_shoot' at speed ", animation_speed);
        } else if (animation_player->has_animation("shoot")) {
            animation_player->play("shoot");
            UtilityFunctions::print("WeaponManager: Playing 'shoot' at speed ", animation_speed);
        } else if (animation_player->has_animation("Fire")) {
            animation_player->play("Fire");
            UtilityFunctions::print("WeaponManager: Playing 'Fire' at speed ", animation_speed);
        } else {
            // Play the first animation if any exist
            PackedStringArray animations = animation_player->get_animation_list();
            if (animations.size() > 0) {
                String first_anim = animations[0];
                animation_player->play(first_anim);
                UtilityFunctions::print("WeaponManager: Playing first animation '", first_anim, "' at speed ", animation_speed);
            }
        }
    } else {
        UtilityFunctions::print("WeaponManager: No AnimationPlayer found");
    }
}

void WeaponManager::play_recoil_animation() {
    if (is_in_recoil) {
        reset_parts(); // Reset if already recoiling
    }
    
    // Calculate scaled values
    Vector3 slide_kick = base_slide_distance * recoil_amplifier;
    Vector3 hammer_kick = base_hammer_rotation * recoil_amplifier;
    Vector3 trigger_kick = base_trigger_pull * recoil_amplifier;
    Vector3 weapon_kick = base_weapon_kick * recoil_amplifier;
    
    // Apply recoil to parts
    if (pistol_slide) {
        Vector3 pos = pistol_slide->get_position();
        pistol_slide->set_position(pos + slide_kick);
    }
    
    if (pistol_hammer) {
        Vector3 rot = pistol_hammer->get_rotation_degrees();
        pistol_hammer->set_rotation_degrees(rot + hammer_kick);
    }
    
    if (pistol_trigger) {
        Vector3 pos = pistol_trigger->get_position();
        pistol_trigger->set_position(pos + trigger_kick);
    }
    
    if (pistol_root) {
        Vector3 rot = pistol_root->get_rotation_degrees();
        pistol_root->set_rotation_degrees(rot + weapon_kick);
    }
    
    is_in_recoil = true;
    current_recoil_time = 0.0;
}

void WeaponManager::update_recoil(double delta) {
    current_recoil_time += delta;
    
    if (current_recoil_time >= recoil_duration) {
        reset_parts();
        return;
    }
    
    // Smooth return to rest position
    double progress = current_recoil_time / recoil_duration;
    double ease_out = 1.0 - (1.0 - progress) * (1.0 - progress);
    
    Vector3 slide_kick = base_slide_distance * recoil_amplifier;
    Vector3 hammer_kick = base_hammer_rotation * recoil_amplifier;
    Vector3 trigger_kick = base_trigger_pull * recoil_amplifier;
    Vector3 weapon_kick = base_weapon_kick * recoil_amplifier;
    
    if (pistol_slide) {
        Vector3 target = slide_kick * (1.0 - ease_out);
        pistol_slide->set_position(target);
    }
    
    if (pistol_hammer) {
        Vector3 target = hammer_kick * (1.0 - ease_out);
        pistol_hammer->set_rotation_degrees(target);
    }
    
    if (pistol_trigger) {
        Vector3 target = trigger_kick * (1.0 - ease_out);
        pistol_trigger->set_position(target);
    }
    
    if (pistol_root) {
        Vector3 target = weapon_kick * (1.0 - ease_out);
        pistol_root->set_rotation_degrees(target);
    }
}

void WeaponManager::reset_parts() {
    if (pistol_slide) pistol_slide->set_position(Vector3(0, 0, 0));
    if (pistol_hammer) pistol_hammer->set_rotation_degrees(Vector3(0, 0, 0));
    if (pistol_trigger) pistol_trigger->set_position(Vector3(0, 0, 0));
    if (pistol_root) pistol_root->set_rotation_degrees(Vector3(0, 0, 0));
    
    is_in_recoil = false;
    current_recoil_time = 0.0;
}

void WeaponManager::handle_shoot_input(bool pressed) {
    if (!pressed) return;
    fire();
}

void WeaponManager::store_position() {
    original_position = get_position();
    UtilityFunctions::print("WeaponManager: Stored original position");
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    if (!enable_sway) return;
    target_sway = mouse_delta * sway_intensity * 0.001;
}

void WeaponManager::update_sway(double delta) {
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    Vector3 sway_offset = Vector3(current_sway.x, current_sway.y, 0);
    set_position(original_position + sway_offset);
    
    // Decay sway toward zero
    target_sway = target_sway.lerp(Vector2(0, 0), delta * 2.0);
}

void WeaponManager::set_movement_state(bool moving) {
    is_moving = moving;
}

void WeaponManager::update_bob(double delta) {
    if (is_moving) {
        bob_time += delta * bob_frequency;
        bob_offset = sin(bob_time) * bob_intensity;
    } else {
        bob_offset = Math::lerp(bob_offset, 0.0, delta * 5.0);
    }
    
    Vector3 current_pos = get_position();
    current_pos.y = original_position.y + bob_offset;
    set_position(current_pos);
}
