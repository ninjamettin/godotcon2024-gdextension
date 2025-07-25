# Weapon System Implementation Guide

## Table of Contents
1. [Architecture Overview](#architecture-overview)
2. [Weapon Base Class](#weapon-base-class)
3. [First-Person Weapon Display](#first-person-weapon-display)
4. [Projectile System](#projectile-system)
5. [Hit Detection & Damage](#hit-detection--damage)
6. [Weapon Types Implementation](#weapon-types-implementation)
7. [Integration with Player](#integration-with-player)
8. [Testing & Debugging](#testing--debugging)

---

## Architecture Overview

### Core Components Needed:
- **Weapon Base Class** - Abstract weapon functionality
- **Projectile Class** - Handles bullets/projectiles
- **WeaponManager** - Manages equipped weapons and switching
- **DamageSystem** - Handles health and damage dealing
- **Enemy Health Component** - For targets to take damage

### File Structure:
```
src/
├── weapons/
│   ├── weapon.hpp/cpp          # Base weapon class
│   ├── projectile.hpp/cpp      # Bullet/projectile handling
│   ├── weapon_manager.hpp/cpp  # Weapon switching/management
│   └── guns/
│       ├── pistol.hpp/cpp      # Specific weapon implementations
│       └── rifle.hpp/cpp
├── combat/
│   ├── health.hpp/cpp          # Health component
│   └── damage_system.hpp/cpp   # Damage dealing logic
└── enemies/
    └── enemy.hpp/cpp           # Enemy with health
```

---

## Weapon Base Class

### 1. Create `src/weapons/weapon.hpp`

```cpp
#ifndef WEAPON_H
#define WEAPON_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/animation_player.hpp>
#include <godot_cpp/classes/audio_stream_player3d.hpp>

namespace godot {

class Weapon : public Node3D {
    GDCLASS(Weapon, Node3D)

protected:
    // Weapon stats
    double damage = 10.0;
    double fire_rate = 1.0;        // Shots per second
    int max_ammo = 30;
    int current_ammo = 30;
    double reload_time = 2.0;
    double projectile_speed = 50.0;
    double range = 100.0;
    
    // State
    bool is_firing = false;
    bool is_reloading = false;
    double fire_timer = 0.0;
    double reload_timer = 0.0;
    
    // Components
    MeshInstance3D* weapon_mesh = nullptr;
    Node3D* muzzle_point = nullptr;        // Where bullets spawn
    AnimationPlayer* animation_player = nullptr;
    AudioStreamPlayer3D* audio_player = nullptr;

public:
    Weapon();
    ~Weapon();
    
    static void _bind_methods();
    
    void _ready() override;
    void _process(double delta) override;
    
    // Core weapon functions
    virtual bool can_fire() const;
    virtual void fire();
    virtual void reload();
    virtual void stop_firing();
    
    // Weapon positioning for first-person
    virtual void setup_first_person_position();
    virtual void play_fire_animation();
    virtual void play_reload_animation();
    
    // Getters/Setters
    double get_damage() const { return damage; }
    int get_current_ammo() const { return current_ammo; }
    int get_max_ammo() const { return max_ammo; }
    bool get_is_reloading() const { return is_reloading; }
};

}

#endif
```

### 2. Key Implementation Points:

- **Fire Rate Control**: Use timer to prevent shooting too fast
- **Ammo Management**: Track current/max ammo, handle reloading
- **Animation Integration**: Play firing/reloading animations
- **Audio**: Gunshot and reload sounds
- **Muzzle Point**: 3D position where projectiles spawn

---

## First-Person Weapon Display

### 1. Weapon Positioning Strategy

**Approach A: Camera Child (Recommended)**
- Attach weapon as child of Camera3D
- Position weapon in camera's local space
- Weapon follows camera rotation automatically
- Easy to implement and maintain

**Approach B: Manual Synchronization**
- Keep weapon as separate node
- Manually sync position/rotation with camera each frame
- More complex but gives more control

### 2. Implementation Steps

#### In `Player::setup_camera()`:
```cpp
void Player::setup_camera() {
    // ... existing camera setup ...
    
    // Setup weapon attachment point
    Node3D* weapon_holder = memnew(Node3D);
    camera->add_child(weapon_holder);
    weapon_holder->set_name("WeaponHolder");
    
    // Position weapon in front of camera
    weapon_holder->set_position(Vector3(0.3, -0.2, -0.5)); // Right, down, forward
    weapon_holder->set_rotation(Vector3(0, 0, 0));
}
```

#### In Weapon Class:
```cpp
void Weapon::setup_first_person_position() {
    // Fine-tune weapon position for first-person view
    set_position(Vector3(0, 0, 0));  // Relative to weapon holder
    set_rotation(Vector3(0, 0, 0));
    
    // Adjust weapon mesh if needed
    if (weapon_mesh) {
        weapon_mesh->set_position(Vector3(0, 0, 0));
        weapon_mesh->set_scale(Vector3(1, 1, 1));
    }
}
```

### 3. Weapon Sway and Bob (Optional Enhancement)

Add subtle movement to make weapon feel more realistic:
- **Weapon Sway**: Weapon follows mouse movement with slight delay
- **Walking Bob**: Weapon bobs up/down when walking
- **Recoil**: Weapon kicks back when firing

---

## Projectile System

### 1. Create `src/weapons/projectile.hpp`

```cpp
#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {

class Projectile : public RigidBody3D {
    GDCLASS(Projectile, RigidBody3D)

private:
    double damage = 10.0;
    double speed = 50.0;
    double lifetime = 5.0;        // Auto-destroy after 5 seconds
    double life_timer = 0.0;
    
    Vector3 direction = Vector3(0, 0, -1);
    Node* shooter = nullptr;      // Who fired this projectile
    
    // Components
    CollisionShape3D* collision_shape = nullptr;
    MeshInstance3D* mesh_instance = nullptr;

public:
    Projectile();
    ~Projectile();
    
    static void _bind_methods();
    
    void _ready() override;
    void _physics_process(double delta) override;
    
    // Setup functions
    void initialize(Vector3 start_pos, Vector3 fire_direction, double proj_speed, double proj_damage, Node* proj_shooter);
    
    // Collision handling
    void _on_body_entered(Node3D* body);
    
    // Getters/Setters
    double get_damage() const { return damage; }
    Node* get_shooter() const { return shooter; }
};

}

#endif
```

### 2. Projectile Implementation Details

#### Initialization:
```cpp
void Projectile::initialize(Vector3 start_pos, Vector3 fire_direction, double proj_speed, double proj_damage, Node* proj_shooter) {
    set_global_position(start_pos);
    direction = fire_direction.normalized();
    speed = proj_speed;
    damage = proj_damage;
    shooter = proj_shooter;
    
    // Set initial velocity
    set_linear_velocity(direction * speed);
}
```

#### Movement Options:

**Option A: Physics-Based (Recommended)**
- Use RigidBody3D with initial velocity
- Gravity affects projectiles naturally
- Good for grenades, arrows

**Option B: Kinematic Movement**
- Use CharacterBody3D or Area3D
- Manual position updates each frame
- Better for laser beams, instant bullets

#### Collision Detection:
```cpp
void Projectile::_on_body_entered(Node3D* body) {
    // Don't hit the shooter
    if (body == shooter) return;
    
    // Check if it's an enemy or destructible object
    if (body->has_method("take_damage")) {
        body->call("take_damage", damage, shooter);
    }
    
    // Create hit effect (particles, sound)
    create_hit_effect();
    
    // Destroy projectile
    queue_free();
}
```

---

## Hit Detection & Damage

### 1. Create Health Component (`src/combat/health.hpp`)

```cpp
#ifndef HEALTH_H
#define HEALTH_H

#include <godot_cpp/classes/node.hpp>

namespace godot {

class Health : public Node {
    GDCLASS(Health, Node)

private:
    double max_health = 100.0;
    double current_health = 100.0;
    bool is_dead = false;

public:
    Health();
    ~Health();
    
    static void _bind_methods();
    
    void _ready() override;
    
    // Health management
    void take_damage(double amount, Node* attacker = nullptr);
    void heal(double amount);
    void die();
    
    // Getters/Setters
    double get_current_health() const { return current_health; }
    double get_max_health() const { return max_health; }
    bool get_is_dead() const { return is_dead; }
    double get_health_percentage() const { return current_health / max_health; }
    
    // Signals (bind in _bind_methods)
    // health_changed(new_health, max_health)
    // health_depleted(attacker)
    // damage_taken(amount, attacker)
};

}

#endif
```

### 2. Damage System Integration

#### In Enemy Class:
```cpp
// Add health component as child
Health* health_component = memnew(Health);
add_child(health_component);
health_component->set_name("Health");

// Connect to health signals
health_component->connect("health_depleted", Callable(this, "_on_health_depleted"));
```

#### Universal Damage Interface:
```cpp
// Any object can implement this method to receive damage
void take_damage(double amount, Node* attacker) {
    Health* health = get_node<Health>("Health");
    if (health) {
        health->take_damage(amount, attacker);
    }
}
```

---

## Weapon Types Implementation

### 1. Pistol Example (`src/weapons/guns/pistol.hpp`)

```cpp
#ifndef PISTOL_H
#define PISTOL_H

#include "../weapon.hpp"

namespace godot {

class Pistol : public Weapon {
    GDCLASS(Pistol, Weapon)

public:
    Pistol();
    ~Pistol();
    
    static void _bind_methods();
    
    void _ready() override;
    
    // Override weapon behavior
    void fire() override;
    void setup_first_person_position() override;
};

}

#endif
```

#### Pistol Implementation:
```cpp
Pistol::Pistol() {
    // Set pistol-specific stats
    damage = 25.0;
    fire_rate = 3.0;        // 3 shots per second
    max_ammo = 12;
    current_ammo = 12;
    reload_time = 1.5;
    projectile_speed = 80.0;
    range = 50.0;
}

void Pistol::fire() {
    if (!can_fire()) return;
    
    // Call parent fire logic
    Weapon::fire();
    
    // Spawn projectile at muzzle point
    spawn_projectile();
    
    // Play pistol-specific effects
    play_muzzle_flash();
    play_gunshot_sound();
    play_recoil_animation();
    
    // Consume ammo
    current_ammo--;
    fire_timer = 1.0 / fire_rate;  // Reset fire rate timer
}
```

### 2. Different Weapon Types

#### Assault Rifle:
- Higher fire rate
- More recoil
- Larger magazine
- Automatic fire mode

#### Shotgun:
- Multiple projectiles per shot
- Spread pattern
- High damage, short range
- Slow reload

#### Sniper Rifle:
- Very high damage
- Slow fire rate
- Zoom functionality
- Long range

---

## Integration with Player

### 1. Add Weapon Manager to Player

#### In `player.hpp`:
```cpp
#include "weapons/weapon_manager.hpp"

class Player : public CharacterBody3D {
    // ... existing code ...
    
private:
    WeaponManager* weapon_manager = nullptr;
    
public:
    // Add weapon methods
    void fire_weapon();
    void reload_weapon();
    void switch_weapon(int weapon_index);
};
```

#### In `player.cpp`:
```cpp
void Player::_ready() {
    // ... existing camera setup ...
    
    // Setup weapon manager
    weapon_manager = memnew(WeaponManager);
    add_child(weapon_manager);
    weapon_manager->set_name("WeaponManager");
    
    // Give player a starting weapon
    weapon_manager->add_weapon("pistol");
    weapon_manager->equip_weapon(0);
}

void Player::_input(const Ref<InputEvent>& event) {
    // ... existing mouse movement code ...
    
    // Handle weapon input
    Ref<InputEventMouseButton> mouse_button = event;
    if (mouse_button.is_valid()) {
        if (mouse_button->get_button_index() == MOUSE_BUTTON_LEFT) {
            if (mouse_button->is_pressed()) {
                fire_weapon();
            } else {
                weapon_manager->stop_firing();
            }
        }
    }
    
    // Handle reload
    Ref<InputEventKey> key_event = event;
    if (key_event.is_valid() && key_event->is_pressed()) {
        if (key_event->get_keycode() == KEY_R) {
            reload_weapon();
        }
    }
}

void Player::fire_weapon() {
    if (weapon_manager && weapon_manager->get_current_weapon()) {
        weapon_manager->get_current_weapon()->fire();
    }
}
```

### 2. Weapon Manager Implementation

```cpp
class WeaponManager : public Node {
private:
    Array available_weapons;     // List of all weapons
    int current_weapon_index = -1;
    Weapon* current_weapon = nullptr;
    Node3D* weapon_holder = nullptr;   // Reference to camera's weapon holder

public:
    void add_weapon(String weapon_type);
    void equip_weapon(int index);
    void switch_to_next_weapon();
    void switch_to_previous_weapon();
    
    Weapon* get_current_weapon() const { return current_weapon; }
    int get_weapon_count() const { return available_weapons.size(); }
};
```

---

## Testing & Debugging

### 1. Debug Visualization

#### Weapon Debug Info:
- Display current ammo on screen
- Show weapon name
- Visualize muzzle point position
- Draw projectile trajectory

#### Debug Console Commands:
```cpp
// Add these methods to weapon for testing
void give_infinite_ammo() { current_ammo = max_ammo; }
void set_fire_rate(double rate) { fire_rate = rate; }
void set_damage(double dmg) { damage = dmg; }
```

### 2. Testing Checklist

#### Basic Functionality:
- [ ] Weapon appears in first-person view
- [ ] Weapon fires when clicking
- [ ] Projectiles spawn at correct position
- [ ] Projectiles move in correct direction
- [ ] Fire rate limiting works
- [ ] Ammo decreases when firing
- [ ] Reload functionality works

#### Visual/Audio:
- [ ] Muzzle flash appears
- [ ] Gunshot sound plays
- [ ] Reload animation plays
- [ ] Weapon positioning looks good
- [ ] No clipping with camera/geometry

#### Combat Integration:
- [ ] Projectiles hit enemies
- [ ] Damage is applied correctly
- [ ] Enemies die when health reaches 0
- [ ] Player can't damage themselves

### 3. Common Issues & Solutions

#### Weapon Not Visible:
- Check weapon holder position
- Verify camera FOV settings
- Ensure weapon mesh is loaded
- Check weapon scale

#### Projectiles Not Hitting:
- Verify collision layers/masks
- Check projectile collision shape
- Ensure muzzle point position is correct
- Debug projectile spawning position

#### Performance Issues:
- Pool projectiles instead of creating/destroying
- Limit maximum projectiles in scene
- Use Area3D for instant-hit weapons
- Optimize collision detection

---

## Implementation Order

### Phase 1: Basic Setup
1. Create weapon base class
2. Create simple projectile system
3. Add weapon to player camera
4. Implement basic firing

### Phase 2: Combat Integration
1. Create health component
2. Add damage dealing to projectiles
3. Create basic enemy with health
4. Test weapon vs enemy combat

### Phase 3: Polish & Features
1. Add weapon animations
2. Implement audio system
3. Add multiple weapon types
4. Create weapon switching system

### Phase 4: Advanced Features
1. Add weapon modifications/upgrades
2. Implement different ammo types
3. Add weapon recoil and spread
4. Create weapon pickup system

---

## Tips & Best Practices

### Performance:
- Use object pooling for frequently spawned projectiles
- Limit the number of active projectiles
- Use Area3D for hitscan weapons (instant bullets)
- Cache frequently accessed nodes

### Code Organization:
- Keep weapon stats in easily editable data files
- Use inheritance for shared weapon behavior
- Implement interfaces for modular systems
- Separate rendering from logic

### Game Feel:
- Add screen shake when firing
- Include weapon bob and sway
- Use particle effects for muzzle flashes
- Add shell casing ejection
- Implement weapon recoil patterns

### Debugging:
- Add visual debug helpers (lines, spheres)
- Implement console commands for testing
- Create debug UI for weapon stats
- Add logging for projectile lifecycle

This guide provides a complete roadmap for implementing a robust weapon system. Start with Phase 1 and gradually build up the complexity!
