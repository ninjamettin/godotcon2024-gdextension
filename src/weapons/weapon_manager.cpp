#include "weapon_manager.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>

#include <cmath>

using namespace godot;

// ================ WEAPON CLASS IMPLEMENTATION ================

Weapon::Weapon() {
}

Weapon::~Weapon() {
}

void Weapon::_bind_methods() {
    // Bind weapon methods
    ClassDB::bind_method(D_METHOD("get_damage"), &Weapon::get_damage);
    ClassDB::bind_method(D_METHOD("set_damage", "damage"), &Weapon::set_damage);
    ClassDB::bind_method(D_METHOD("get_fire_rate"), &Weapon::get_fire_rate);
    ClassDB::bind_method(D_METHOD("set_fire_rate", "rate"), &Weapon::set_fire_rate);
    ClassDB::bind_method(D_METHOD("get_current_ammo"), &Weapon::get_current_ammo);
    ClassDB::bind_method(D_METHOD("get_max_ammo"), &Weapon::get_max_ammo);
    ClassDB::bind_method(D_METHOD("set_max_ammo", "ammo"), &Weapon::set_max_ammo);
    ClassDB::bind_method(D_METHOD("get_is_reloading"), &Weapon::get_is_reloading);
    ClassDB::bind_method(D_METHOD("get_recoil_duration"), &Weapon::get_recoil_duration);
    ClassDB::bind_method(D_METHOD("set_recoil_duration", "duration"), &Weapon::set_recoil_duration);
    ClassDB::bind_method(D_METHOD("get_recoil_offset"), &Weapon::get_recoil_offset);
    ClassDB::bind_method(D_METHOD("set_recoil_offset", "offset"), &Weapon::set_recoil_offset);
    ClassDB::bind_method(D_METHOD("fire"), &Weapon::fire);
    ClassDB::bind_method(D_METHOD("reload"), &Weapon::reload);
    
    // Add properties for individual weapon customization
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "damage"), "set_damage", "get_damage");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "fire_rate"), "set_fire_rate", "get_fire_rate");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "max_ammo"), "set_max_ammo", "get_max_ammo");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "recoil_duration"), "set_recoil_duration", "get_recoil_duration");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "recoil_offset"), "set_recoil_offset", "get_recoil_offset");
}

bool Weapon::can_fire() const {
    return !is_reloading && current_ammo > 0 && fire_timer <= 0.0;
}

void Weapon::reload() {
    if (is_reloading || current_ammo == max_ammo) return;
    
    is_reloading = true;
    reload_timer = reload_time;
    
    // TODO: Play reload animation
    play_reload_animation();
}

void Weapon::stop_firing() {
    is_firing = false;
}

void Weapon::setup_first_person_position() {
    // TODO: Set up weapon positioning for first-person view
}

void Weapon::play_fire_animation() {
    if (animation_player) {
        animation_player->play(fire_animation_name);
    }
}

void Weapon::play_reload_animation() {
    if (animation_player) {
        animation_player->play(reload_animation_name);
    }
}

void Weapon::_ready() {
    // Call parent ready
    Node3D::_ready();
    
    // Store original position for recoil calculations
    original_position = get_position();
    
    // TODO: Find or create muzzle flash point
    // This should be positioned at the tip of your weapon
    muzzle_flash_point = get_node<Node3D>("MuzzlePoint");
    if (!muzzle_flash_point) {
        muzzle_flash_point = memnew(Node3D);
        add_child(muzzle_flash_point);
        muzzle_flash_point->set_name("MuzzlePoint");
        // TODO: Position this at your weapon's barrel tip
        muzzle_flash_point->set_position(Vector3(0, 0, -1.0)); // Adjust as needed
    }
    
    // TODO: Create muzzle flash effect node
    create_muzzle_flash();
}

void Weapon::create_muzzle_flash() {
    if (!muzzle_flash_point) return;
    
    // TODO: Create your muzzle flash visual effect
    // This could be a MeshInstance3D with a plane, GPUParticles3D, or CPUParticles3D
    muzzle_flash_effect = memnew(MeshInstance3D);
    muzzle_flash_point->add_child(muzzle_flash_effect);
    muzzle_flash_effect->set_name("MuzzleFlash");
    
    // TODO: Set up muzzle flash material and mesh
    // Example: Set up a bright material, make it face camera
    muzzle_flash_effect->set_visible(false);
}

void Weapon::fire() {
    if (!can_fire()) return;
    
    // Handle recoil interruption
    if (is_in_recoil && interrupt_recoil_on_fire) {
        interrupt_recoil();
    }
    
    // Start recoil animation
    start_recoil_animation();
    
    // Trigger muzzle flash
    muzzle_flash_active = true;
    muzzle_flash_timer = muzzle_flash_duration;
    if (muzzle_flash_effect) {
        muzzle_flash_effect->set_visible(true);
    }
    
    // TODO: Create and fire projectile
    fire_projectile();
    
    // TODO: Play firing sound
    // TODO: Play firing animation
    
    // Update weapon state
    current_ammo--;
    fire_timer = 1.0 / fire_rate;
    is_firing = true;
}

void Weapon::fire_projectile() {
    if (!muzzle_flash_point) return;
    
    // TODO: Get firing direction (usually camera forward)
    Vector3 fire_direction = -get_global_transform().basis.get_column(2); // Forward (Z axis)
    Vector3 start_position = muzzle_flash_point->get_global_position();
    
    // TODO: Create projectile using your projectile manager
    // ProjectileManager::get_singleton()->create_projectile(
    //     start_position, fire_direction, projectile_speed, damage, this
    // );
}

void Weapon::start_recoil_animation() {
    is_in_recoil = true;
    current_recoil_time = 0.0;
}

void Weapon::interrupt_recoil() {
    // Quickly return to firing position
    is_in_recoil = false;
    set_position(original_position);
}

void Weapon::update_recoil_animation(double delta) {
    if (!is_in_recoil) return;
    
    current_recoil_time += delta;
    
    if (current_recoil_time >= recoil_duration) {
        // Recoil phase complete, start recovery
        double recovery_progress = (current_recoil_time - recoil_duration) * recoil_recovery_speed;
        
        if (recovery_progress >= 1.0) {
            // Recovery complete
            set_position(original_position);
            is_in_recoil = false;
            current_recoil_time = 0.0;
        } else {
            // Lerp back to original position
            Vector3 recoil_pos = original_position + recoil_offset;
            Vector3 current_pos = recoil_pos.lerp(original_position, recovery_progress);
            set_position(current_pos);
        }
    } else {
        // Still in recoil phase - move to max recoil position
        double recoil_progress = current_recoil_time / recoil_duration;
        Vector3 current_pos = original_position.lerp(original_position + recoil_offset, recoil_progress);
        set_position(current_pos);
    }
}

void Weapon::update_muzzle_flash(double delta) {
    if (!muzzle_flash_active) return;
    
    muzzle_flash_timer -= delta;
    
    if (muzzle_flash_timer <= 0.0) {
        muzzle_flash_active = false;
        if (muzzle_flash_effect) {
            muzzle_flash_effect->set_visible(false);
        }
    }
}

void Weapon::_process(double delta) {
    // Update existing timers
    if (fire_timer > 0.0) {
        fire_timer -= delta;
    }
    
    if (is_reloading) {
        reload_timer -= delta;
        if (reload_timer <= 0.0) {
            is_reloading = false;
            current_ammo = max_ammo;
        }
    }
    
    // Update recoil animation
    update_recoil_animation(delta);
    
    // Update muzzle flash
    update_muzzle_flash(delta);
}

// ================ WEAPON MANAGER CLASS IMPLEMENTATION ================

WeaponManager::WeaponManager() {
    // Constructor - initialize default values if needed
}

WeaponManager::~WeaponManager() {
    // Destructor
}

void WeaponManager::_bind_methods() {
    // Bind the public methods
    ClassDB::bind_method(D_METHOD("apply_mouse_input", "mouse_delta"), &WeaponManager::apply_mouse_input);
    ClassDB::bind_method(D_METHOD("set_movement_state", "moving"), &WeaponManager::set_movement_state);
    ClassDB::bind_method(D_METHOD("get_weapon_count"), &WeaponManager::get_weapon_count);
    ClassDB::bind_method(D_METHOD("store_weapon_positions"), &WeaponManager::store_weapon_positions);
    ClassDB::bind_method(D_METHOD("handle_shoot_input", "pressed"), &WeaponManager::handle_shoot_input);
    
    // Bind sway property getters and setters
    ClassDB::bind_method(D_METHOD("get_sway_intensity"), &WeaponManager::get_sway_intensity);
    ClassDB::bind_method(D_METHOD("set_sway_intensity", "intensity"), &WeaponManager::set_sway_intensity);
    ClassDB::bind_method(D_METHOD("get_sway_smoothness"), &WeaponManager::get_sway_smoothness);
    ClassDB::bind_method(D_METHOD("set_sway_smoothness", "smoothness"), &WeaponManager::set_sway_smoothness);
    ClassDB::bind_method(D_METHOD("get_max_sway_distance"), &WeaponManager::get_max_sway_distance);
    ClassDB::bind_method(D_METHOD("set_max_sway_distance", "distance"), &WeaponManager::set_max_sway_distance);
    
    // Bind bob property getters and setters
    ClassDB::bind_method(D_METHOD("get_bob_intensity"), &WeaponManager::get_bob_intensity);
    ClassDB::bind_method(D_METHOD("set_bob_intensity", "intensity"), &WeaponManager::set_bob_intensity);
    ClassDB::bind_method(D_METHOD("get_bob_frequency"), &WeaponManager::get_bob_frequency);
    ClassDB::bind_method(D_METHOD("set_bob_frequency", "frequency"), &WeaponManager::set_bob_frequency);
    ClassDB::bind_method(D_METHOD("get_enable_bob"), &WeaponManager::get_enable_bob);
    ClassDB::bind_method(D_METHOD("set_enable_bob", "enable"), &WeaponManager::set_enable_bob);
    
    // Add properties to the inspector - only global settings
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sway_intensity"), "set_sway_intensity", "get_sway_intensity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "sway_smoothness"), "set_sway_smoothness", "get_sway_smoothness");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_sway_distance"), "set_max_sway_distance", "get_max_sway_distance");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bob_intensity"), "set_bob_intensity", "get_bob_intensity");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "bob_frequency"), "set_bob_frequency", "get_bob_frequency");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "enable_bob"), "set_enable_bob", "get_enable_bob");
}

void WeaponManager::_ready() {
    // Find all Node3D children and store their positions
    UtilityFunctions::print("WeaponManager: Found ", get_child_count(), " children");
    
    store_weapon_positions();
}

void WeaponManager::store_weapon_positions() {
    weapon_children.clear();
    original_positions.clear();
    
    for (int i = 0; i < get_child_count(); i++) {
        Node3D* child = Object::cast_to<Node3D>(get_child(i));
        if (child) {
            UtilityFunctions::print("WeaponManager: Found weapon child: ", child->get_name());
            weapon_children.append(child);
            original_positions.append(child->get_position());
        }
    }
    
    UtilityFunctions::print("WeaponManager: Tracking ", weapon_children.size(), " weapons for sway");
}

void WeaponManager::_process(double delta) {
    // Update weapon sway and bob every frame
    update_weapon_sway(delta);
    update_weapon_bob(delta);
    
    // Handle continuous firing
    if (is_shooting) {
        Weapon* current_weapon = get_current_weapon();
        if (current_weapon && current_weapon->can_fire()) {
            current_weapon->fire();
        }
    }
}

void WeaponManager::update_weapon_sway(double delta) {
    if (weapon_children.size() == 0) return;
    
    // Smoothly interpolate current sway towards target sway
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    // Combine sway and bob offsets
    // TO-LOOK: I do not know why but the gun moves up when the camera moves up without the minus sign.
    Vector3 sway_offset = Vector3(current_sway.x, -current_sway.y, 0.0); 
    Vector3 bob_offset_vec = Vector3(0.0, bob_offset, 0.0);
    Vector3 total_offset = sway_offset + bob_offset_vec;
    
    // Apply combined offset to ALL weapon children
    for (int i = 0; i < weapon_children.size(); i++) {
        Node3D* weapon = Object::cast_to<Node3D>(weapon_children[i]);
        Vector3 original_pos = original_positions[i];
        
        if (weapon) {
            Vector3 new_position = original_pos + total_offset;
            weapon->set_position(new_position);
        }
    }
    
    // Debug output (remove this later)
    static double debug_timer = 0.0;
    debug_timer += delta;
    if (debug_timer > 1.0) { // Print every second
        //UtilityFunctions::print("Sway - Target: ", target_sway, " Current: ", current_sway, " Weapons: ", weapon_children.size());
        debug_timer = 0.0;
    }
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    // Debug: Check if mouse input is reaching this function
    //UtilityFunctions::print("WeaponManager: Received mouse input: ", mouse_delta);
    
    // Calculate target sway based on mouse input
    // Weapon sways in OPPOSITE direction of mouse movement
    Vector2 sway_input = -mouse_delta * sway_intensity * 0.001; // Scale down mouse input
    
    // Clamp sway to maximum distance
    if (sway_input.length() > max_sway_distance) {
        sway_input = sway_input.normalized() * max_sway_distance;
    }
    
    target_sway = sway_input;
}

void WeaponManager::update_weapon_bob(double delta) {
    if (!enable_bob) {
        bob_offset = 0.0;
        return;
    }
    
    if (is_moving) {
        // Update bob time when moving
        bob_time += delta;
        
        // Calculate bob offset using sine wave
        // Use bob_frequency to control speed, bob_intensity for amplitude
        bob_offset = sin(bob_time * bob_frequency * 6.28318530718) * bob_intensity;
    } else {
        // Gradually return to center when not moving
        bob_offset = Math::lerp(bob_offset, 0.0, 5.0 * delta);
        
        // Reset bob time when stopped
        if (fabs(bob_offset) < 0.001) {
            bob_time = 0.0;
        }
    }
}

void WeaponManager::set_movement_state(bool moving) {
    is_moving = moving;
}

Weapon* WeaponManager::get_current_weapon() const {
    // Return the first visible weapon found, or nullptr if none
    for (int i = 0; i < get_child_count(); i++) {
        Weapon* weapon = Object::cast_to<Weapon>(get_child(i));
        if (weapon && weapon->is_visible()) {
            return weapon;
        }
    }
    return nullptr;
}

void WeaponManager::_input(const Ref<InputEvent>& event) {
    // Handle mouse button input for shooting
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid()) {
        if (mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
            handle_shoot_input(mouse_button->is_pressed());
        }
    }
}

void WeaponManager::handle_shoot_input(bool pressed) {
    is_shooting = pressed;
    
    if (pressed) {
        // Fire current weapon immediately
        Weapon* current_weapon = get_current_weapon();
        if (current_weapon) {
            current_weapon->fire();
        }
    }
}