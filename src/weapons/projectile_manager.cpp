#include "projectile_manager.hpp"
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/viewport.hpp>

using namespace godot;

ProjectileManager* ProjectileManager::singleton = nullptr;

ProjectileManager::ProjectileManager() {
    singleton = this;
}

ProjectileManager::~ProjectileManager() {
    singleton = nullptr;
}

void ProjectileManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("create_projectile"), &ProjectileManager::create_projectile);
}

void ProjectileManager::_ready() {
    physics_space = get_viewport()->get_world_3d()->get_direct_space_state();
    setup_projectile_visuals();
    
    active_projectiles.resize(max_projectiles);
    for (int i = 0; i < max_projectiles; i++) {
        Dictionary projectile_dict;
        projectile_dict["active"] = false;
        active_projectiles[i] = projectile_dict;
    }
}

void ProjectileManager::create_projectile(Vector3 start_pos, Vector3 direction, double speed, double damage, Node* shooter, double max_range) {
    int start_index = next_projectile_index;
    do {
        Dictionary projectile_dict = active_projectiles[next_projectile_index];
        if (!projectile_dict["active"]) {
            projectile_dict["position"] = start_pos;
            projectile_dict["direction"] = direction.normalized();
            projectile_dict["speed"] = speed;
            projectile_dict["damage"] = damage;
            projectile_dict["max_range"] = max_range;
            projectile_dict["traveled_distance"] = 0.0;
            projectile_dict["shooter"] = Variant(shooter);
            projectile_dict["active"] = true;
            
            update_projectile_visual(next_projectile_index);
            
            next_projectile_index = (next_projectile_index + 1) % max_projectiles;
            return;
        }
        
        next_projectile_index = (next_projectile_index + 1) % max_projectiles;
    } while (next_projectile_index != start_index);
}

void ProjectileManager::_process(double delta) {
    update_projectiles(delta);
}

void ProjectileManager::update_projectiles(double delta) {
    if (!physics_space) return;
    
    for (int i = 0; i < max_projectiles; i++) {
        Dictionary projectile_dict = active_projectiles[i];
        if (!projectile_dict["active"]) continue;
        
        Vector3 current_pos = projectile_dict["position"];
        Vector3 direction = projectile_dict["direction"];
        double speed = projectile_dict["speed"];
        double max_range = projectile_dict["max_range"];
        double traveled = projectile_dict["traveled_distance"];
        
        // Calculate movement this frame
        double distance_this_frame = speed * delta;
        Vector3 new_pos = current_pos + direction * distance_this_frame;
        
        // Check range limit
        double new_traveled = traveled + distance_this_frame;
        if (new_traveled >= max_range) {
            cleanup_projectile(i);
            continue;
        }
        
        projectile_dict["position"] = new_pos;
        projectile_dict["traveled_distance"] = new_traveled;
        
        update_projectile_visual(i);
    }
}

void ProjectileManager::setup_projectile_visuals() {
}

void ProjectileManager::update_projectile_visual(int index) {
    Dictionary projectile_dict = active_projectiles[index];
    if (!projectile_dict["active"]) return;
}

void ProjectileManager::cleanup_projectile(int index) {
    Dictionary projectile_dict = active_projectiles[index];
    projectile_dict["active"] = false;
    hide_projectile_visual(index);
}

void ProjectileManager::hide_projectile_visual(int index) {
}

void ProjectileManager::setup_optimized_visuals() {
}