#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/mesh.hpp>

#include <godot_cpp/core/class_db.hpp>

namespace godot {

struct ProjectileData {
    Vector3 position;
    Vector3 direction;
    double speed;
    double damage;
    double max_range;
    double traveled_distance;
    Node* shooter;
    bool active;
    
    // Visual representation
    RID visual_instance;  // For rendering the projectile trail
};

class ProjectileManager : public Node {
    GDCLASS(ProjectileManager, Node)

private:
    static ProjectileManager* singleton;
    
    Array active_projectiles;  // Pool of ProjectileData
    int max_projectiles = 1000; // Performance limit
    int next_projectile_index = 0;
    
    // Visual settings
    Ref<Material> projectile_material;
    Ref<Mesh> projectile_mesh;
    double projectile_visual_length = 0.5; // Length of visible trail
    
    // Physics world for raycasting
    PhysicsDirectSpaceState3D* physics_space = nullptr;

public:
    ProjectileManager();
    ~ProjectileManager();
    
    static void _bind_methods();
    static ProjectileManager* get_singleton() { return singleton; }
    
    void _ready() override;
    void _process(double delta) override;
    
    // Projectile management
    void create_projectile(Vector3 start_pos, Vector3 direction, double speed, double damage, Node* shooter, double max_range = 100.0);
    void update_projectiles(double delta);
    void cleanup_projectile(int index);
    
    // Visual management
    void setup_projectile_visuals();
    void update_projectile_visual(int index);
    void hide_projectile_visual(int index);
    void setup_optimized_visuals();
};

}

#endif