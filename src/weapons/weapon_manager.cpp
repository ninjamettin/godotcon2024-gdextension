#include "weapon_manager.hpp"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

using namespace godot;

// ================ WEAPON CLASS ================

Weapon::Weapon() {
    recoil_amplifier = 1.0;
    is_in_recoil = false;
    current_recoil_time = 0.0;
}

Weapon::~Weapon() {}

void Weapon::_bind_methods() {
    ClassDB::bind_method(D_METHOD("fire"), &Weapon::fire);
    ClassDB::bind_method(D_METHOD("setup_pistol_parts"), &Weapon::setup_pistol_parts);
    ClassDB::bind_method(D_METHOD("play_fire_animation"), &Weapon::play_fire_animation);
    ClassDB::bind_method(D_METHOD("get_recoil_amplifier"), &Weapon::get_recoil_amplifier);
    ClassDB::bind_method(D_METHOD("set_recoil_amplifier", "amplifier"), &Weapon::set_recoil_amplifier);
}

void Weapon::_ready() {
    setup_pistol_parts();
}

void Weapon::_process(double delta) {
    if (is_in_recoil) {
        update_recoil(delta);
    }
}

void Weapon::setup_pistol_parts() {
    // Find pistol parts in the scene
    pistol_root = this;
    pistol_slide   = Object::cast_to<MeshInstance3D>(get_node_or_null("Slide"));
    pistol_hammer  = Object::cast_to<MeshInstance3D>(get_node_or_null("Hammer"));
    pistol_trigger = Object::cast_to<MeshInstance3D>(get_node_or_null("Trigger"));

    // Find AnimationPlayer - check in children recursively
    animation_player = Object::cast_to<AnimationPlayer>(get_node_or_null("AnimationPlayer"));
    if (!animation_player) {
        // Try to find in child nodes (P2262/AnimationPlayer path)
        Node* pistol_model = get_node_or_null("P2262");
        if (pistol_model) {
            animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
        }
    }
    
    if (animation_player) {
        UtilityFunctions::print("Weapon: Found AnimationPlayer");
    } else {
        UtilityFunctions::print("Weapon: AnimationPlayer not found");
    }
    
    UtilityFunctions::print("Weapon: Setup pistol parts complete");
}

void Weapon::fire() {
    play_recoil_animation();
    play_fire_animation();
}

void Weapon::play_recoil_animation() {
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
    
    UtilityFunctions::print("Weapon: Recoil animation started with amplifier ", recoil_amplifier);
}

void Weapon::update_recoil(double delta) {
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

void Weapon::reset_parts() {
    if (pistol_slide) pistol_slide->set_position(Vector3(0, 0, 0));
    if (pistol_hammer) pistol_hammer->set_rotation_degrees(Vector3(0, 0, 0));
    if (pistol_trigger) pistol_trigger->set_position(Vector3(0, 0, 0));
    if (pistol_root) pistol_root->set_rotation_degrees(Vector3(0, 0, 0));
    
    is_in_recoil = false;
    current_recoil_time = 0.0;
}

void Weapon::play_fire_animation() {
    if (animation_player) {
        // Try common animation names - replace "fire" with your animation name
        if (animation_player->has_animation("fire")) {
            animation_player->play("fire");
            UtilityFunctions::print("Weapon: Playing 'fire' animation");
        } else if (animation_player->has_animation("shoot")) {
            animation_player->play("shoot");
            UtilityFunctions::print("Weapon: Playing 'shoot' animation");
        } else if (animation_player->has_animation("Fire")) {
            animation_player->play("Fire");
            UtilityFunctions::print("Weapon: Playing 'Fire' animation");
        } else {
            // Print available animations to help debug
            PackedStringArray animations = animation_player->get_animation_list();
            UtilityFunctions::print("Weapon: Available animations: ", animations);
            
            // Play the first animation if any exist
            if (animations.size() > 0) {
                String first_anim = animations[0];
                animation_player->play(first_anim);
                UtilityFunctions::print("Weapon: Playing first available animation: ", first_anim);
            }
        }
    } else {
        UtilityFunctions::print("Weapon: No AnimationPlayer found for fire animation");
    }
}

// ================ WEAPON MANAGER CLASS ================

WeaponManager::WeaponManager() {
    sway_intensity = 1.0;
    sway_smoothness = 5.0;
    enable_sway = true;
    bob_intensity = 0.005;
    bob_frequency = 0.8;
    enable_bob = true;
}

WeaponManager::~WeaponManager() {}

void WeaponManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("apply_mouse_input", "mouse_delta"), &WeaponManager::apply_mouse_input);
    ClassDB::bind_method(D_METHOD("set_movement_state", "moving"), &WeaponManager::set_movement_state);
    ClassDB::bind_method(D_METHOD("handle_shoot_input", "pressed"), &WeaponManager::handle_shoot_input);
    ClassDB::bind_method(D_METHOD("set_weapon_recoil_amplifier", "weapon_index", "amplifier"), &WeaponManager::set_weapon_recoil_amplifier);
    
    ClassDB::bind_method(D_METHOD("get_sway_intensity"), &WeaponManager::get_sway_intensity);
    ClassDB::bind_method(D_METHOD("set_sway_intensity", "intensity"), &WeaponManager::set_sway_intensity);
    ClassDB::bind_method(D_METHOD("get_bob_intensity"), &WeaponManager::get_bob_intensity);
    ClassDB::bind_method(D_METHOD("set_bob_intensity", "intensity"), &WeaponManager::set_bob_intensity);
    ClassDB::bind_method(D_METHOD("get_enable_sway"), &WeaponManager::get_enable_sway);
    ClassDB::bind_method(D_METHOD("set_enable_sway", "enable"), &WeaponManager::set_enable_sway);
    ClassDB::bind_method(D_METHOD("get_enable_bob"), &WeaponManager::get_enable_bob);
    ClassDB::bind_method(D_METHOD("set_enable_bob", "enable"), &WeaponManager::set_enable_bob);
    ClassDB::bind_method(D_METHOD("get_sway_smoothness"), &WeaponManager::get_sway_smoothness);
    ClassDB::bind_method(D_METHOD("set_sway_smoothness", "smoothness"), &WeaponManager::set_sway_smoothness);
    ClassDB::bind_method(D_METHOD("get_bob_frequency"), &WeaponManager::get_bob_frequency);
    ClassDB::bind_method(D_METHOD("set_bob_frequency", "frequency"), &WeaponManager::set_bob_frequency);
    
    // Expose properties to the editor
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_intensity", PROPERTY_HINT_RANGE, "0.0,5.0,0.1"), "set_sway_intensity", "get_sway_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_smoothness", PROPERTY_HINT_RANGE, "0.1,20.0,0.1"), "set_sway_smoothness", "get_sway_smoothness");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_sway"), "set_enable_sway", "get_enable_sway");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_intensity", PROPERTY_HINT_RANGE, "0.0,0.1,0.001"), "set_bob_intensity", "get_bob_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_frequency", PROPERTY_HINT_RANGE, "0.1,5.0,0.1"), "set_bob_frequency", "get_bob_frequency");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_bob"), "set_enable_bob", "get_enable_bob");
}

void WeaponManager::_ready() {
    store_positions();
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
}

void WeaponManager::store_positions() {
    weapon_children.clear();
    original_positions.clear();
    
    for (int i = 0; i < get_child_count(); i++) {
        Node3D* child = Object::cast_to<Node3D>(get_child(i));
        if (child) {
            weapon_children.append(child);
            original_positions.append(child->get_position());
        }
    }
    
    UtilityFunctions::print("WeaponManager: Stored ", weapon_children.size(), " weapon positions");
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    if (!enable_sway) return;
    
    target_sway = mouse_delta * sway_intensity * 0.001;
}

void WeaponManager::update_sway(double delta) {
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    for (int i = 0; i < weapon_children.size(); i++) {
        Variant weapon_variant = weapon_children[i];
        Node3D* weapon = Object::cast_to<Node3D>(weapon_variant);
        Variant pos_variant = original_positions[i];
        Vector3 original_pos = pos_variant;
        
        if (weapon) {
            Vector3 sway_offset = Vector3(current_sway.x, current_sway.y, 0);
            weapon->set_position(original_pos + sway_offset);
        }
    }
    
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
    
    for (int i = 0; i < weapon_children.size(); i++) {
        Variant weapon_variant = weapon_children[i];
        Node3D* weapon = Object::cast_to<Node3D>(weapon_variant);
        Variant pos_variant = original_positions[i];
        Vector3 original_pos = pos_variant;
        
        if (weapon) {
            Vector3 current_pos = weapon->get_position();
            current_pos.y = original_pos.y + bob_offset;
            weapon->set_position(current_pos);
        }
    }
}

void WeaponManager::handle_shoot_input(bool pressed) {
    if (!pressed) return;
    
    for (int i = 0; i < weapon_children.size(); i++) {
        Variant weapon_variant = weapon_children[i];
        Weapon* weapon = Object::cast_to<Weapon>(weapon_variant);
        if (weapon) {
            weapon->fire();
        }
    }
}

void WeaponManager::set_weapon_recoil_amplifier(int weapon_index, double amplifier) {
    if (weapon_index < 0 || weapon_index >= weapon_children.size()) return;
    
    Variant weapon_variant = weapon_children[weapon_index];
    Weapon* weapon = Object::cast_to<Weapon>(weapon_variant);
    if (weapon) {
        weapon->set_recoil_amplifier(amplifier);
        UtilityFunctions::print("WeaponManager: Set recoil amplifier for weapon ", weapon_index, " to ", amplifier);
    }
}
