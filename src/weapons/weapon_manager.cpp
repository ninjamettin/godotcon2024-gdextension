#include "weapon_manager.hpp"
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/sphere_mesh.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>


using namespace godot;

WeaponManager::WeaponManager() {
    // Initialize all properties with default values
    animation_speed = 3.0;
    
    sway_intensity = 1.0;
    sway_smoothness = 5.0;
    enable_sway = true;
    bob_intensity = 0.01;
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
    
    // Expose properties to the editor (removed animation_speed slider)
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
    
    // Update muzzle flash and flying rays
    update_muzzle_flash(delta);
    update_flying_rays(delta);
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
    
    // Debug: List all child nodes
    UtilityFunctions::print("WeaponManager: Child count: ", get_child_count());
    for (int i = 0; i < get_child_count(); i++) {
        Node* child = get_child(i);
        UtilityFunctions::print("WeaponManager: Child ", i, ": ", child->get_name(), " (", child->get_class(), ")");
    }

    // Find AnimationPlayer in Pistol -> P2262 -> AnimationPlayer hierarchy
    Node* pistol_script = get_node_or_null("Pistol");
    if (pistol_script) {
        UtilityFunctions::print("WeaponManager: Found Pistol script node");
        
        // Debug: List Pistol's children
        UtilityFunctions::print("WeaponManager: Pistol child count: ", pistol_script->get_child_count());
        for (int i = 0; i < pistol_script->get_child_count(); i++) {
            Node* child = pistol_script->get_child(i);
            UtilityFunctions::print("WeaponManager: Pistol child ", i, ": ", child->get_name(), " (", child->get_class(), ")");
        }
        
        Node* pistol_model = pistol_script->get_node_or_null("P2262");
        if (pistol_model) {
            UtilityFunctions::print("WeaponManager: Found P2262 node under Pistol");
            
            // Debug: List P2262's children
            UtilityFunctions::print("WeaponManager: P2262 child count: ", pistol_model->get_child_count());
            for (int i = 0; i < pistol_model->get_child_count(); i++) {
                Node* child = pistol_model->get_child(i);
                UtilityFunctions::print("WeaponManager: P2262 child ", i, ": ", child->get_name(), " (", child->get_class(), ")");
            }
            
            animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
        } else {
            UtilityFunctions::print("WeaponManager: No P2262 node found under Pistol");
        }
    } else {
        UtilityFunctions::print("WeaponManager: No Pistol script node found - trying fallback hierarchy");
        
        // Fallback: Try the old hierarchy WeaponManager -> P2262 -> AnimationPlayer
        Node* pistol_model = get_node_or_null("P2262");
        if (pistol_model) {
            UtilityFunctions::print("WeaponManager: Found P2262 node directly under WeaponManager (fallback)");
            animation_player = Object::cast_to<AnimationPlayer>(pistol_model->get_node_or_null("AnimationPlayer"));
        } else {
            UtilityFunctions::print("WeaponManager: No P2262 node found in fallback either");
        }
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
    create_muzzle_flash();
    fire_projectile();
}

void WeaponManager::play_fire_animation() {
    if (animation_player) {
        // Get fire_rate from Pistol script to calculate animation speed
        double calculated_speed = animation_speed;
        Node* pistol_script = get_node_or_null("Pistol");
        if (pistol_script) {
            // Try to get fire_rate from Pistol
            Variant fire_rate_var = pistol_script->call("get_fire_rate");
            if (fire_rate_var.get_type() == Variant::FLOAT) {
                double fire_rate = fire_rate_var;
                calculated_speed = animation_speed * (fire_rate / 5.0); // 5.0 is base fire rate
                UtilityFunctions::print("WeaponManager: Using fire rate ", fire_rate, " for animation speed ", calculated_speed);
            } else {
                UtilityFunctions::print("WeaponManager: Could not get fire_rate from Pistol");
            }
        }
        
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
    
    // Debug mouse input
    //UtilityFunctions::print("WeaponManager: Mouse delta: ", mouse_delta);
    
    // Horizontal sway should move opposite to mouse movement (realistic lag effect)
    // Vertical sway should also move opposite to mouse movement (look up = gun down)
    Vector2 sway_delta = Vector2(-mouse_delta.x, mouse_delta.y) * sway_intensity * 0.01;
    target_sway = sway_delta;
    
    //UtilityFunctions::print("WeaponManager: Sway delta: ", sway_delta, ", Target sway: ", target_sway);
}

void WeaponManager::update_sway(double delta) {
    current_sway = current_sway.lerp(target_sway, sway_smoothness * delta);
    
    // Apply both horizontal and vertical sway, plus bob
    Vector3 sway_offset = Vector3(current_sway.x, current_sway.y, 0);
    Vector3 bob_position_offset = Vector3(0, bob_offset, 0);
    Vector3 new_position = original_position + sway_offset + bob_position_offset;
    set_position(new_position);
    
    // Debug sway application
 
    
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
    
    // Don't set position here - let update_sway handle the final position
    // Bob offset will be applied in update_sway
}

Vector3 WeaponManager::get_muzzle_world_position() {
    // Get the world position of the muzzle based on gun position and orientation
    Transform3D weapon_transform = get_global_transform();
    Vector3 world_muzzle_pos = weapon_transform.xform(muzzle_offset);
    return world_muzzle_pos;
}

void WeaponManager::create_muzzle_flash() {
    // Create or reuse muzzle flash
    if (!muzzle_flash) {
        muzzle_flash = memnew(MeshInstance3D);
        add_child(muzzle_flash);
        
        // Create a bright sphere for muzzle flash
        Ref<SphereMesh> sphere = memnew(SphereMesh);
        sphere->set_radius(0.05);
        sphere->set_radial_segments(8);
        sphere->set_rings(6);
        muzzle_flash->set_mesh(sphere);
        
        // Create bright orange/yellow material
        Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
        material->set_emission(Color(1.0, 0.8, 0.2, 1.0)); // Bright orange
        material->set_emission_intensity(3.0); // Set emission energy/intensity
        material->set_transparency(BaseMaterial3D::TRANSPARENCY_ALPHA);
        muzzle_flash->set_material_override(material);
        
        //UtilityFunctions::print("WeaponManager: Created muzzle flash");
    }
    
    // Position muzzle flash at muzzle location
    muzzle_flash->set_position(muzzle_offset);
    muzzle_flash->set_visible(true);
    muzzle_flash_timer = muzzle_flash_duration;
    
    UtilityFunctions::print("WeaponManager: Muzzle flash triggered at position: ", muzzle_offset);
}

void WeaponManager::update_muzzle_flash(double delta) {
    if (muzzle_flash && muzzle_flash_timer > 0.0) {
        muzzle_flash_timer -= delta;
        
        // Fade out the muzzle flash
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
    // Get camera direction for projectile trajectory
    Camera3D* camera = nullptr;
    Node* parent = get_parent();
    while (parent && !camera) {
        camera = Object::cast_to<Camera3D>(parent);
        if (!camera) {
            // Look for camera in children
            for (int i = 0; i < parent->get_child_count(); i++) {
                camera = Object::cast_to<Camera3D>(parent->get_child(i));
                if (camera) break;
            }
        }
        parent = parent->get_parent();
    }
    
    Vector3 shoot_direction = Vector3(0, 0, -1); // Default forward
    if (camera) {
        Transform3D cam_transform = camera->get_global_transform();
        shoot_direction = -cam_transform.basis.get_column(2); // Camera forward direction
        UtilityFunctions::print("WeaponManager: Using camera direction: ", shoot_direction);
    } else {
        UtilityFunctions::print("WeaponManager: No camera found, using default direction");
    }
    
    // Create visual projectile
    MeshInstance3D* projectile_visual = memnew(MeshInstance3D);
    get_tree()->get_current_scene()->add_child(projectile_visual);
    
    // Create small sphere for projectile
    Ref<SphereMesh> sphere = memnew(SphereMesh);
    sphere->set_radius(0.01);
    sphere->set_radial_segments(6);
    sphere->set_rings(4);
    projectile_visual->set_mesh(sphere);
    
    // Create bright material for visibility
    Ref<StandardMaterial3D> material = memnew(StandardMaterial3D);
    material->set_emission(Color(1.0, 1.0, 0.0, 1.0)); // Bright yellow
    material->set_emission_intensity(2.0);

    projectile_visual->set_material_override(material);
    
    Vector3 start_pos = get_muzzle_world_position();
    projectile_visual->set_global_position(start_pos);
    
    // Store projectile data (simplified for now - we'll improve this)
    UtilityFunctions::print("WeaponManager: Fired projectile from ", start_pos, " in direction ", shoot_direction);
    
    // For now, just schedule the projectile to be removed after 3 seconds
    // We'll implement proper trajectory in the next step
    projectile_visual->call_deferred("queue_free");
}

void WeaponManager::update_flying_rays(double delta) {
    // Placeholder for ray updates - will implement trajectory in next iteration
    // For now, just log that we're updating rays
    /*if (flying_rays.size() > 0) {
        UtilityFunctions::print("WeaponManager: Updating ", flying_rays.size(), " flying rays");
    }*/
}

void WeaponManager::cleanup_ray(int index) {
    // Placeholder for ray cleanup
    UtilityFunctions::print("WeaponManager: Cleaning up ray at index ", index);
}
