#include "weapon_manager.hpp"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

WeaponManager::WeaponManager() {
    animation_speed = 1.0;
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
    
    // Muzzle flash and projectile methods
    ClassDB::bind_method(D_METHOD("create_muzzle_flash"), &WeaponManager::create_muzzle_flash);
    ClassDB::bind_method(D_METHOD("fire_projectile"), &WeaponManager::fire_projectile);
    ClassDB::bind_method(D_METHOD("get_muzzle_world_position"), &WeaponManager::get_muzzle_world_position);
    
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
    
    // Expose properties to the editor
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_intensity", PROPERTY_HINT_RANGE, "0.0,5.0,0.01"), "set_sway_intensity", "get_sway_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "sway_smoothness", PROPERTY_HINT_RANGE, "0.1,20.0,0.1"), "set_sway_smoothness", "get_sway_smoothness");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_sway"), "set_enable_sway", "get_enable_sway");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_intensity", PROPERTY_HINT_RANGE, "0.0,0.05,0.0001"), "set_bob_intensity", "get_bob_intensity");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::FLOAT, "bob_frequency", PROPERTY_HINT_RANGE, "0.1,15.0,0.01"), "set_bob_frequency", "get_bob_frequency");
    ClassDB::add_property("WeaponManager", PropertyInfo(Variant::BOOL, "enable_bob"), "set_enable_bob", "get_enable_bob");
}

void WeaponManager::_ready() {
    setup_weapon_parts();
    store_position();
}

void WeaponManager::_process(double delta) {
    if (enable_sway) update_sway(delta);
    if (enable_bob) update_bob(delta);
    
    // Apply combined position updates
    Vector3 sway_offset = Vector3(current_sway.x, current_sway.y, 0);
    Vector3 bob_position_offset = Vector3(0, bob_offset, 0);
    Vector3 new_position = original_position + sway_offset + bob_position_offset;
    set_position(new_position);
    
    update_muzzle_flash(delta);
}

void WeaponManager::_input(const Ref<InputEvent>& event) {
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid() && mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
        handle_shoot_input(mouse_button->is_pressed());
    }
    
    Ref<InputEventMouseMotion> mouse_motion = event;
    if (mouse_motion.is_valid() && enable_sway) {
        apply_mouse_input(mouse_motion->get_relative());
    }
}

void WeaponManager::setup_weapon_parts() {
    // Find AnimationPlayer in Pistol -> P2262 -> AnimationPlayer hierarchy
    Node* pistol_script = get_node_or_null("Pistol");
    if (pistol_script) {
        Node* pistol_model = pistol_script->get_node_or_null("P2262");
        if (pistol_model) {
            animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
        }
    } else {
        // Fallback: Try the old hierarchy WeaponManager -> P2262 -> AnimationPlayer
        Node* pistol_model = get_node_or_null("P2262");
        if (pistol_model) {
            animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
        }
    }
}

void WeaponManager::fire() {
    play_fire_animation();
    create_muzzle_flash();
    fire_projectile();
}

void WeaponManager::play_fire_animation() {
    if (animation_player) {
        double calculated_speed = animation_speed;
        Node* pistol_script = get_node_or_null("Pistol");
        if (pistol_script) {
            Variant fire_rate_var = pistol_script->call("get_fire_rate");
            if (fire_rate_var.get_type() == Variant::FLOAT) {
                double fire_rate = fire_rate_var;
                calculated_speed = animation_speed * (fire_rate / 5.0);
            }
        }
        
        animation_player->set_speed_scale(calculated_speed);
        
        if (animation_player->has_animation("pistol_shoot")) {
            animation_player->play("pistol_shoot");
        } else if (animation_player->has_animation("shoot")) {
            animation_player->play("shoot");
        } else if (animation_player->has_animation("Fire")) {
            animation_player->play("Fire");
        } else {
            PackedStringArray animations = animation_player->get_animation_list();
            if (animations.size() > 0) {
                animation_player->play(animations[0]);
            }
        }
    }
}




void WeaponManager::handle_shoot_input(bool pressed) {
    if (!pressed) return;
    fire();
}

void WeaponManager::store_position() {
    original_position = get_position();
}

void WeaponManager::apply_mouse_input(Vector2 mouse_delta) {
    if (!enable_sway || sway_intensity <= 0.0) return;
    
    Vector2 sway_delta = Vector2(-mouse_delta.x, mouse_delta.y) * sway_intensity * 0.01;
    target_sway = sway_delta;
}

void WeaponManager::update_sway(double delta) {
    if (!enable_sway || sway_intensity <= 0.0) {
        target_sway = Vector2(0, 0);
        current_sway = current_sway.lerp(Vector2(0, 0), delta * 10.0);
    } else {
        current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
        target_sway = target_sway.lerp(Vector2(0, 0), delta * 0.5);
    }
}

void WeaponManager::set_movement_state(bool moving, double speed) {
    is_moving = moving;
    player_speed = speed;
}

void WeaponManager::update_bob(double delta) {
    if (!enable_bob || bob_intensity <= 0.0) {
        bob_offset = Math::lerp(bob_offset, 0.0, delta * 5.0);
    } else if (is_moving) {
        double speed_multiplier = Math::max(1.0, player_speed / 5.0);
        double effective_frequency = bob_frequency * speed_multiplier;
        bob_time += delta * effective_frequency;
        bob_offset = sin(bob_time) * bob_intensity;
    } else {
        bob_offset = Math::lerp(bob_offset, 0.0, delta * 5.0);
    }
}

Vector3 WeaponManager::get_muzzle_world_position() {
    Transform3D weapon_transform = get_global_transform();
    Vector3 world_muzzle_pos = weapon_transform.xform(muzzle_offset);
    return world_muzzle_pos;
}

void WeaponManager::create_muzzle_flash() {
    if (!muzzle_flash) {
        muzzle_flash = memnew(MeshInstance3D);
        add_child(muzzle_flash);
        
        Ref<SphereMesh> sphere = memnew(SphereMesh);
        sphere->set_radius(0.05);
        sphere->set_radial_segments(8);
        sphere->set_rings(6);
        muzzle_flash->set_mesh(sphere);
        
        Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
        material->set_emission(Color(1.0, 0.8, 0.2, 1.0));
        material->set_emission_intensity(30.0);
        material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
        muzzle_flash->set_material_override(material);
    }
    
    muzzle_flash->set_position(muzzle_offset);
    muzzle_flash->set_visible(true);
    muzzle_flash_timer = muzzle_flash_duration;
}

void WeaponManager::update_muzzle_flash(double delta) {
    if (muzzle_flash && muzzle_flash_timer > 0.0) {
        muzzle_flash_timer -= delta;
        
        double alpha = muzzle_flash_timer / muzzle_flash_duration;
        Ref<StandardMaterial3D> material = muzzle_flash->get_material_override();
        if (material.is_valid()) {
            Color emission = material->get_emission();
            emission.a = alpha;
            material->set_emission(emission);
        }
        
        if (muzzle_flash_timer <= 0.0) {
            muzzle_flash->set_visible(false);
        }
    }
}

void WeaponManager::fire_projectile() {
    Camera3D* camera = nullptr;
    Node* parent = get_parent();
    while (parent && !camera) {
        camera = Object::cast_to<Camera3D>(parent);
        if (!camera) {
            for (int i = 0; i < parent->get_child_count(); i++) {
                camera = Object::cast_to<Camera3D>(parent->get_child(i));
                if (camera) break;
            }
        }
        parent = parent->get_parent();
    }
    
    Vector3 shoot_direction = Vector3(0, 0, -1);
    if (camera) {
        Transform3D cam_transform = camera->get_global_transform();
        shoot_direction = -cam_transform.basis.get_column(2);
    }
    
    MeshInstance3D* projectile_visual = memnew(MeshInstance3D);
    get_tree()->get_current_scene()->add_child(projectile_visual);
    
    Ref<SphereMesh> sphere = memnew(SphereMesh);
    sphere->set_radius(0.01);
    sphere->set_radial_segments(6);
    sphere->set_rings(4);
    projectile_visual->set_mesh(sphere);
    
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_emission(Color(1.0, 1.0, 0.0, 1.0));
    material->set_emission_intensity(2.0);
    projectile_visual->set_material_override(material);
    
    Vector3 start_pos = get_muzzle_world_position();
    projectile_visual->set_global_position(start_pos);
    
    projectile_visual->call_deferred("queue_free");
}
