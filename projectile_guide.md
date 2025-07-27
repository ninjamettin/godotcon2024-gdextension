# Projectile System Implementation Guide

This guide will walk you through implementing a complete projectile system with muzzle flash, recoil animation, and optimized ray-based projectiles for your GDExtension project.

## Overview

The system consists of:
1. **Base Weapon Class** - Common functionality for all weapons (in weapon_manager.hpp/cpp)
2. **WeaponManager Class** - Handles global weapon behaviors like sway and bob
3. **Individual Weapon Scripts** - Specific weapon implementations that inherit from Weapon
4. **Projectile System** - Optimized ray-based projectiles for performance
5. **Visual Effects** - Muzzle flash and projectile trails

## 1. Architecture Overview

### Combined weapon_manager.hpp/cpp Structure:
- **Weapon Class**: Base class with common functionality (muzzle flash, recoil, firing mechanics)
- **WeaponManager Class**: Handles global behaviors (sway, bob, input handling)
- **Individual Weapon Classes**: Inherit from Weapon and customize stats

### Key Design Principles:
- **Global settings** (sway, bob) are configured in WeaponManager
- **Weapon-specific stats** (damage, fire_rate, recoil_duration) are set in individual weapon scripts
- **Reusable functionality** is provided by the base Weapon class

## 2. Creating Individual Weapon Scripts

Each specific weapon (pistol, rifle, etc.) should inherit from the base Weapon class and customize its stats:

### Example: Pistol Implementation
Create `src/weapons/pistol.hpp`:
```cpp
#ifndef PISTOL_H
#define PISTOL_H

#include "weapon_manager.hpp"

namespace godot {

class Pistol : public Weapon {
    GDCLASS(Pistol, Weapon)

public:
    Pistol();
    ~Pistol();
    
    static void _bind_methods();
    void _ready() override;
    
    // Override weapon behavior if needed
    void setup_first_person_position() override;
};

}

#endif
```

Create `src/weapons/pistol.cpp`:
```cpp
#include "pistol.hpp"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

Pistol::Pistol() {
}

Pistol::~Pistol() {
}

void Pistol::_bind_methods() {
    // Call parent bind methods
    // Individual weapon properties will be exposed through the base class
}

void Pistol::_ready() {
    // Call parent ready first
    Weapon::_ready();
    
    // Customize pistol-specific stats
    set_damage(15.0);                    // Moderate damage
    set_fire_rate(8.0);                  // 8 shots per second
    set_max_ammo(12);                    // 12 round magazine
    set_recoil_duration(0.1);            // Quick recoil
    set_recoil_offset(Vector3(0, 0, 0.03)); // Light recoil movement
    
    // Set reload time and other stats
    reload_time = 1.5;                   // Fast reload
    projectile_speed = 80.0;             // High velocity
    range = 50.0;                        // Medium range
    
    // TODO: Load pistol-specific assets
    // - Weapon mesh
    // - Animation player
    // - Audio files
    // - Muzzle flash effects
}

void Pistol::setup_first_person_position() {
    // TODO: Position pistol for first-person view
    // This might include specific offset and rotation
    set_position(Vector3(0.3, -0.2, -0.5)); // Right-handed position
    set_rotation_degrees(Vector3(0, -5, 0)); // Slight angle
}
```

### Example: Assault Rifle Implementation
Create `src/weapons/assault_rifle.hpp` and `assault_rifle.cpp`:
```cpp
// assault_rifle.cpp implementation
void AssaultRifle::_ready() {
    Weapon::_ready();
    
    // Customize assault rifle stats
    set_damage(25.0);                     // Higher damage
    set_fire_rate(12.0);                  // 12 shots per second (full auto)
    set_max_ammo(30);                     // 30 round magazine
    set_recoil_duration(0.15);            // Longer recoil
    set_recoil_offset(Vector3(0, 0, 0.08)); // More recoil movement
    
    reload_time = 2.5;                    // Slower reload
    projectile_speed = 120.0;             // Higher velocity
    range = 100.0;                        // Longer range
    
    // TODO: Load assault rifle assets
}
```

## 3. Using the Weapon System

### In Your Scene Setup:
1. Add a WeaponManager node as a child of your Camera3D
2. Add individual weapon nodes (Pistol, AssaultRifle, etc.) as children of WeaponManager
3. Configure global settings on WeaponManager (sway, bob)
4. Individual weapon stats are automatically configured in their _ready() methods

### Scene Structure:
```
Player (CharacterBody3D)
└── Camera3D
    └── WeaponManager
        ├── Pistol (extends Weapon)
        ├── AssaultRifle (extends Weapon)
        └── Shotgun (extends Weapon)
```

### WeaponManager Configuration:
```cpp
// In the editor or via code, configure global weapon behavior:
weapon_manager->set_sway_intensity(1);        // Global sway amount
weapon_manager->set_bob_intensity(0.03);       // Global bob amount
weapon_manager->set_bob_frequency(1.8);         // Global bob speed
```

## 4. Base Weapon Class Features

The base Weapon class provides these features for all weapons:

### Automatic Muzzle Flash System:
- Creates MuzzlePoint node at weapon tip
- Handles flash timing and visibility
- Customizable flash duration

### Recoil Animation System:
- Smooth weapon movement during firing
- Configurable recoil offset and duration
- Automatic recovery animation
- Rapid-fire interruption support

### Firing Mechanics:
- Ammo management
- Fire rate limiting
- Reload system
- Can-fire validation

### Customizable Properties (Per Weapon):
- `damage`: Damage per shot
- `fire_rate`: Shots per second
- `max_ammo`: Magazine capacity
- `recoil_duration`: How long recoil animation takes
- `recoil_offset`: How far weapon moves during recoil
- `reload_time`: How long reload takes
- `projectile_speed`: Bullet velocity
- `range`: Maximum projectile range

## 5. WeaponManager Global Features

### Weapon Sway:
- Mouse-based weapon movement
- Configurable intensity and smoothness
- Maximum sway distance limiting

### Weapon Bob:
- Walking-based weapon bobbing
- Sine wave animation
- Movement state detection

### Input Handling:
- Automatic mouse click detection
- Continuous firing support
- Current weapon selection

### Global Properties:
- `sway_intensity`: How much weapons sway
- `sway_smoothness`: Sway interpolation speed
- `max_sway_distance`: Maximum sway range
- `bob_intensity`: Bobbing amplitude
- `bob_frequency`: Bobbing speed
- `enable_bob`: Toggle bobbing on/off

## 6. Registering New Weapons

When you create a new weapon class, register it in `register_types.cpp`:

```cpp
void initialize_gdextension_types(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    godot::ClassDB::register_class<godot::Player>();
    godot::ClassDB::register_class<godot::Weapon>();
    godot::ClassDB::register_class<godot::WeaponManager>();
    
    // Register your individual weapons
    godot::ClassDB::register_class<godot::Pistol>();
    godot::ClassDB::register_class<godot::AssaultRifle>();
    godot::ClassDB::register_class<godot::Shotgun>();
}
```

## 2. Optimized Projectile System

For performance with hundreds of bullets, we'll use a ray-based system instead of physics bodies:

### ProjectileManager Singleton
Create a new file: `src/weapons/projectile_manager.hpp`
```cpp
#ifndef PROJECTILE_MANAGER_H
#define PROJECTILE_MANAGER_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
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
};

}

#endif
```

### ProjectileManager Implementation Template
Create `src/weapons/projectile_manager.cpp`:
```cpp
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
    // TODO: Get physics space from main scene
    // physics_space = get_viewport()->get_world_3d()->get_direct_space_state();
    
    setup_projectile_visuals();
    
    // Initialize projectile pool
    active_projectiles.resize(max_projectiles);
    for (int i = 0; i < max_projectiles; i++) {
        Dictionary projectile_dict;
        projectile_dict["active"] = false;
        active_projectiles[i] = projectile_dict;
    }
}

void ProjectileManager::create_projectile(Vector3 start_pos, Vector3 direction, double speed, double damage, Node* shooter, double max_range) {
    // Find next available projectile slot
    int start_index = next_projectile_index;
    do {
        Dictionary& projectile_dict = active_projectiles[next_projectile_index];
        if (!projectile_dict["active"]) {
            // Found available slot
            projectile_dict["position"] = start_pos;
            projectile_dict["direction"] = direction.normalized();
            projectile_dict["speed"] = speed;
            projectile_dict["damage"] = damage;
            projectile_dict["max_range"] = max_range;
            projectile_dict["traveled_distance"] = 0.0;
            projectile_dict["shooter"] = Variant(shooter);
            projectile_dict["active"] = true;
            
            // TODO: Create visual representation
            update_projectile_visual(next_projectile_index);
            
            next_projectile_index = (next_projectile_index + 1) % max_projectiles;
            return;
        }
        
        next_projectile_index = (next_projectile_index + 1) % max_projectiles;
    } while (next_projectile_index != start_index);
    
    // No available slots - oldest projectile gets replaced
    // TODO: Log warning about projectile limit reached
}

void ProjectileManager::_process(double delta) {
    update_projectiles(delta);
}

void ProjectileManager::update_projectiles(double delta) {
    if (!physics_space) return;
    
    for (int i = 0; i < max_projectiles; i++) {
        Dictionary& projectile_dict = active_projectiles[i];
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
        
        // TODO: Perform raycast for collision detection
        // PhysicsRayQueryParameters3D query;
        // query->set_from(current_pos);
        // query->set_to(new_pos);
        // Ref<PhysicsRayQueryResult3D> result = physics_space->intersect_ray(query);
        
        // if (result.is_valid()) {
        //     // Hit something - TODO: Apply damage and cleanup
        //     Node3D* hit_body = Object::cast_to<Node3D>(result->get_collider());
        //     if (hit_body && hit_body != projectile_dict["shooter"]) {
        //         // TODO: Apply damage to hit target
        //         cleanup_projectile(i);
        //         continue;
        //     }
        // }
        
        // Update projectile position
        projectile_dict["position"] = new_pos;
        projectile_dict["traveled_distance"] = new_traveled;
        
        // Update visual
        update_projectile_visual(i);
    }
}

void ProjectileManager::setup_projectile_visuals() {
    // TODO: Create material for projectile trails
    // This could be a simple bright material or a trail effect
    
    // TODO: Create mesh for projectile representation
    // This could be a thin cylinder or quad for the trail
}

void ProjectileManager::update_projectile_visual(int index) {
    Dictionary& projectile_dict = active_projectiles[index];
    if (!projectile_dict["active"]) return;
    
    Vector3 position = projectile_dict["position"];
    Vector3 direction = projectile_dict["direction"];
    
    // TODO: Update visual instance position and rotation
    // Make the visual trail point in the direction of travel
    // RenderingServer::get_singleton()->instance_set_transform(visual_instance, transform);
}

void ProjectileManager::cleanup_projectile(int index) {
    Dictionary& projectile_dict = active_projectiles[index];
    projectile_dict["active"] = false;
    
    // TODO: Hide visual representation
    hide_projectile_visual(index);
}

void ProjectileManager::hide_projectile_visual(int index) {
    // TODO: Hide or disable the visual representation
}
```

## 3. Integration with Player and Weapon Manager

### Enhanced WeaponManager for Shooting Input
Add to your `weapon_manager.hpp`:
```cpp
// Add to private section:
private:
    bool is_shooting = false;
    double last_shot_time = 0.0;

// Add to public section:
public:
    void handle_shoot_input(bool pressed);
    void _input(const Ref<InputEvent>& event) override;
```

### WeaponManager Shooting Implementation
Add to `weapon_manager.cpp`:
```cpp
void WeaponManager::_input(const Ref<InputEvent>& event) {
    // TODO: Handle mouse button input for shooting
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
        // TODO: Get current weapon and fire
        // for (int i = 0; i < get_child_count(); i++) {
        //     Weapon* weapon = Object::cast_to<Weapon>(get_child(i));
        //     if (weapon && weapon->is_visible()) {
        //         weapon->fire();
        //         break;
        //     }
        // }
    }
}

void WeaponManager::_process(double delta) {
    // Existing sway and bob code...
    
    // Handle continuous firing
    if (is_shooting) {
        // TODO: Check if enough time has passed for next shot based on fire rate
        // TODO: Fire weapon if ready
    }
}
```

## 4. Performance Optimization Tips

### Object Pooling
- Pre-allocate projectile objects in an array/pool
- Reuse projectiles instead of creating/destroying them
- Limit maximum active projectiles (suggested: 500-1000)

### Visual Optimization
```cpp
// Instead of individual MeshInstance3D for each projectile:
// Use MultiMeshInstance3D for batch rendering hundreds of projectiles
// Or use RenderingServer directly for maximum performance

// Example setup in ProjectileManager:
void ProjectileManager::setup_optimized_visuals() {
    // TODO: Create MultiMeshInstance3D for batch rendering
    // multi_mesh_instance = memnew(MultiMeshInstance3D);
    // add_child(multi_mesh_instance);
    
    // TODO: Set up MultiMesh with max projectile count
    // Ref<MultiMesh> multi_mesh = memnew(MultiMesh);
    // multi_mesh->set_transform_format(MultiMesh::TRANSFORM_3D);
    // multi_mesh->set_instance_count(max_projectiles);
    // multi_mesh->set_mesh(projectile_mesh);
    // multi_mesh_instance->set_multimesh(multi_mesh);
}
```

### Physics Optimization
- Use area/space queries instead of individual raycasts when possible
- Batch collision checks
- Use simpler collision shapes
- Consider distance-based culling for very far projectiles

## 5. Implementation Checklist

### Phase 1: Basic Setup
- [ ] Enhance Weapon class with muzzle flash and recoil systems
- [ ] Create muzzle flash point positioning system
- [ ] Implement basic recoil animation
- [ ] Test firing input and muzzle flash visibility

### Phase 2: Projectile System
- [ ] Create ProjectileManager singleton
- [ ] Implement projectile pooling system
- [ ] Add basic raycast collision detection
- [ ] Create simple projectile trail visuals

### Phase 3: Integration
- [ ] Connect weapon firing to projectile creation
- [ ] Implement damage system integration
- [ ] Add projectile-to-enemy hit detection
- [ ] Test rapid fire scenarios

### Phase 4: Optimization
- [ ] Implement MultiMesh rendering for projectiles
- [ ] Add performance profiling
- [ ] Optimize collision detection
- [ ] Add projectile limit controls

### Phase 5: Polish
- [ ] Add particle effects for muzzle flash
- [ ] Implement different projectile types
- [ ] Add sound effects integration
- [ ] Create projectile impact effects

## 6. Testing Scenarios

1. **Single Shot**: Fire one bullet, verify muzzle flash and recoil
2. **Rapid Fire**: Hold fire button, ensure smooth animation interruption
3. **Performance Test**: Fire 100+ bullets in quick succession
4. **Hit Detection**: Verify projectiles hit enemies correctly
5. **Range Limit**: Confirm projectiles disappear at max range

## Notes

- Position your `MuzzlePoint` node exactly at the tip of your weapon model
- Adjust `recoil_offset` values based on your weapon scale
- Tune `fire_rate` to match your desired shooting speed
- Consider adding weapon-specific projectile types
- Test projectile performance in complex scenes

This system provides a solid foundation for high-performance projectile mechanics while maintaining visual quality and responsive controls.
