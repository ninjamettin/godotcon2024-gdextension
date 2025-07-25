# Godot GDExtension Learning Guide

## Table of Contents
1. [Understanding .hpp Files](#understanding-hpp-files)
2. [Register Types System](#register-types-system)
3. [Game Object Architecture](#game-object-architecture)
4. [Build System (SCons)](#build-system-scons)
5. [Shared Library Files (.so)](#shared-library-files-so)
6. [C++ Code Breakdown](#c-code-breakdown)
7. [Adding New Features](#adding-new-features)

---

## Understanding .hpp Files

### What are .hpp files?
`.hpp` files are **header files** in C++. Think of them as "blueprints" or "declarations" that tell the compiler what your code looks like without showing the actual implementation.

### Purpose in our code:
- **`player.hpp`**: Contains the **declaration** of the Player class
- **Declares**: What variables exist, what functions exist, what they return
- **Doesn't contain**: The actual code that runs (that's in `.cpp` files)

### Why separate .hpp and .cpp?
```
┌─────────────────┐    ┌─────────────────┐
│   player.hpp    │    │   player.cpp    │
│   (Declaration) │    │ (Implementation)│
├─────────────────┤    ├─────────────────┤
│ class Player {  │    │ void Player::   │
│   void jump();  │    │ jump() {        │
│   float speed;  │    │   // actual code│
│ }               │    │ }               │
└─────────────────┘    └─────────────────┘
```

**Benefits:**
- **Compilation Speed**: Other files only need to read the small .hpp file
- **Organization**: Separate interface from implementation
- **Reusability**: Multiple .cpp files can include the same .hpp

---

## Register Types System

### What is register_types.cpp?
This file tells Godot about your custom C++ classes so they can be used in the editor and GDScript.

### Purpose:
```cpp
// This line makes "Player" available in Godot
godot::ClassDB::register_class<godot::Player>();
```

### What happens:
1. **Without registration**: Godot doesn't know your Player class exists
2. **With registration**: Godot can create Player nodes, show them in editor, use them in scenes

### register_types.h:
- **Header file** for the registration functions
- Contains function declarations that other files might need

### Think of it like:
```
Your C++ Class → Register Types → Godot Engine → Available in Editor
```

---

## Game Object Architecture

### For Mobs, Guns, and Power-ups:

**Yes, you'll typically create separate classes for each:**

#### Recommended Structure:
```
src/
├── player.cpp/hpp          # Player character
├── enemy.cpp/hpp           # Base enemy class
├── zombie.cpp/hpp          # Specific zombie enemy
├── weapon.cpp/hpp          # Base weapon class
├── pistol.cpp/hpp          # Specific pistol weapon
├── powerup.cpp/hpp         # Base power-up class
├── health_powerup.cpp/hpp  # Specific health power-up
└── register_types.cpp      # Register ALL your classes
```

#### Class Hierarchy Example:
```cpp
// Base classes
class Enemy : public CharacterBody3D { ... }
class Weapon : public Node3D { ... }
class PowerUp : public Area3D { ... }

// Specific implementations
class Zombie : public Enemy { ... }
class Pistol : public Weapon { ... }
class HealthPack : public PowerUp { ... }
```

#### Benefits of this approach:
- **Modularity**: Each class handles one responsibility
- **Inheritance**: Share common behavior between similar objects
- **Maintainability**: Easy to modify one type without affecting others
- **Performance**: C++ classes are faster than GDScript for complex logic

---

## Build System (SCons)

### What happens when you run `scons`?

#### Step-by-step process:
1. **Read SConstruct file**: SCons reads the build configuration
2. **Find source files**: Locates all .cpp files in src/ directory
3. **Compile each .cpp file**: Converts C++ code to object files (.o)
4. **Link object files**: Combines all .o files into a shared library (.so)
5. **Copy to bin/**: Places the final library in bin/linux/ directory

#### Visual representation:
```
player.cpp ──┐
enemy.cpp  ──┤
weapon.cpp ──┤──> Compiler ──> libgodotcon2024.linux.template_debug.x86_64.so
powerup.cpp──┤
register.cpp─┘
```

#### SCons advantages:
- **Dependency tracking**: Only recompiles changed files
- **Cross-platform**: Works on Windows, Linux, macOS
- **Parallel building**: Can compile multiple files simultaneously
- **Integration**: Works seamlessly with Godot's build system

---

## Shared Library Files (.so)

### What are .so files?
`.so` = "Shared Object" - Linux equivalent of Windows .dll files

### Purpose:
- **Runtime Loading**: Godot loads your C++ code at runtime
- **Dynamic Linking**: Your extension becomes part of Godot without rebuilding the engine
- **Platform Specific**: .so (Linux), .dll (Windows), .dylib (macOS)

### How it works:
```
Godot Engine ──> Loads ──> libgodotcon2024.so ──> Your C++ Classes Available
```

### In our project:
- **Source**: `src/*.cpp` files
- **Build**: `scons` creates the .so file
- **Location**: `demo/bin/linux/libgodotcon2024.linux.template_debug.x86_64.so`
- **Usage**: `example.gdextension` tells Godot where to find it

---

## C++ Code Breakdown

### Coming from Python: Key Differences

#### Python vs C++ Syntax:
```python
# Python
class Player:
    def __init__(self):
        self.speed = 5.0
    
    def move(self):
        pass
```

```cpp
// C++ Header (.hpp)
class Player : public CharacterBody3D {
private:
    double speed = 5.0;
public:
    void move();
};

// C++ Implementation (.cpp)
void Player::move() {
    // implementation here
}
```

### Detailed player.hpp Analysis:

```cpp
#ifndef PLAYER_H                    // Prevent multiple inclusions
#define PLAYER_H

#include <godot_cpp/classes/character_body3d.hpp>  // Import Godot's CharacterBody3D

namespace godot {                   // Godot's namespace

class Player : public CharacterBody3D {    // Inherit from CharacterBody3D
    GDCLASS(Player, CharacterBody3D)        // Godot macro for class registration

private:                           // Private members (only this class can access)
    double speed = 5.0;            // Member variable with default value
    double gravity = 9.82;
    double jump_velocity = 4.5;

public:                            // Public members (anyone can access)
    Player() {}                    // Constructor (like __init__ in Python)
    ~Player() {}                   // Destructor (cleanup when object destroyed)

    static void _bind_methods();   // Godot method binding function
    void _physics_process(double delta) override;  // Override Godot's physics function
    
    // Getter and setter methods (like @property in Python)
    double get_speed() const { return speed; }
    void set_speed(double p_speed) { speed = p_speed; }
};

}  // namespace godot

#endif // PLAYER_H
```

### Detailed player.cpp Analysis:

```cpp
#include "player.hpp"              // Include our header file
#include <godot_cpp/classes/input.hpp>    // Include Godot's Input class

using namespace godot;             // Use Godot namespace

void Player::_bind_methods() {     // This makes our C++ methods available to Godot
    // Bind getter/setter methods
    ClassDB::bind_method(D_METHOD("get_speed"), &Player::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Player::set_speed);
    
    // Add properties that appear in Godot editor
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");
}

void Player::_physics_process(double delta) {  // Called every physics frame
    // Don't run in editor mode
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Vector3 velocity = get_velocity();  // Get current velocity from CharacterBody3D

    // Apply gravity (like physics simulation)
    if (!is_on_floor())                 // If not touching ground
        velocity.y -= gravity * delta;  // Pull downward

    // Get input from keyboard
    Input* input = Input::get_singleton();  // Get Godot's input system
    Vector2 input_dir = Vector2(            // Create 2D direction vector
        (input->is_key_pressed(KEY_D) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_A) ? 1.0f : 0.0f),
        (input->is_key_pressed(KEY_S) ? 1.0f : 0.0f) - (input->is_key_pressed(KEY_W) ? 1.0f : 0.0f)
    ).normalized();

    // Convert 2D input to 3D world direction
    Vector3 direction = get_global_transform().basis.xform(Vector3(input_dir.x, 0, input_dir.y)).normalized();
    velocity.x = direction.x * speed;   // Set horizontal velocity
    velocity.z = direction.z * speed;

    // Handle jumping
    if (is_on_floor() && input->is_key_pressed(KEY_SPACE)) {
        velocity.y = jump_velocity;     // Apply upward velocity
    }

    set_velocity(velocity);             // Apply velocity to character
    move_and_slide();                   // Handle collision and movement
}
```

### Key C++ Concepts for Python Developers:

1. **Static Typing**: Must declare variable types (`double`, `Vector3`, etc.)
2. **Memory Management**: C++ gives you control over memory (though Godot handles most of it)
3. **Compilation**: Code must be compiled before running (unlike Python's interpretation)
4. **Headers vs Implementation**: Separation of declaration (.hpp) and implementation (.cpp)
5. **Namespaces**: Like Python modules, group related code (`godot::`)
6. **Const Correctness**: `const` keyword indicates values that won't change

---

## Adding New Features

### General Workflow:

#### 1. Plan Your Feature
- **Identify**: What type of object? (Character, Item, System)
- **Inherit**: What Godot class should it extend?
- **Behavior**: What should it do?

#### 2. Create Class Files
```bash
# Create header and implementation files
touch src/new_feature.hpp
touch src/new_feature.cpp
```

#### 3. Implement the Header (.hpp)
```cpp
#ifndef NEW_FEATURE_H
#define NEW_FEATURE_H

#include <godot_cpp/classes/[base_class].hpp>

namespace godot {

class NewFeature : public BaseClass {
    GDCLASS(NewFeature, BaseClass)

private:
    // Your member variables

public:
    NewFeature() {}
    ~NewFeature() {}
    
    static void _bind_methods();
    
    // Your public methods
};

}

#endif
```

#### 4. Implement the Code (.cpp)
```cpp
#include "new_feature.hpp"

using namespace godot;

void NewFeature::_bind_methods() {
    // Bind your methods and properties
}

// Implement your methods here
```

#### 5. Register the Class
Add to `register_types.cpp`:
```cpp
#include "new_feature.hpp"

void initialize_gdextension_types(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
    godot::ClassDB::register_class<godot::Player>();
    godot::ClassDB::register_class<godot::NewFeature>();  // Add this line
}
```

#### 6. Build and Test
```bash
scons target=template_debug
cp bin/linux/*.so demo/bin/linux/
```

### Specific Examples:

#### Adding a Weapon System:
1. **Create**: `weapon.hpp` and `weapon.cpp`
2. **Inherit**: From `Node3D` or `RigidBody3D`
3. **Features**: Damage, fire rate, ammunition
4. **Integration**: Player holds weapon, enemies take damage

#### Adding an Enemy:
1. **Create**: `enemy.hpp` and `enemy.cpp`
2. **Inherit**: From `CharacterBody3D`
3. **Features**: AI behavior, health, attack patterns
4. **Integration**: Spawn system, combat with player

#### Adding a Power-up:
1. **Create**: `powerup.hpp` and `powerup.cpp`
2. **Inherit**: From `Area3D` (for collision detection)
3. **Features**: Effect type, duration, visual feedback
4. **Integration**: Spawn in levels, player collection

### Best Practices:

1. **Start Simple**: Begin with basic functionality, add complexity gradually
2. **Test Often**: Build and test after each small change
3. **Use Godot Types**: Prefer `Vector3`, `Transform3D` over custom types
4. **Follow Naming**: Use clear, descriptive names for classes and methods
5. **Comment Your Code**: Especially complex game logic
6. **Version Control**: Commit working versions before major changes

### Common Patterns:

#### State Management:
```cpp
enum class PlayerState {
    IDLE,
    WALKING,
    JUMPING,
    ATTACKING
};

class Player : public CharacterBody3D {
    PlayerState current_state = PlayerState::IDLE;
    
    void update_state(double delta);
};
```

#### Component System:
```cpp
class HealthComponent : public Node {
    // Health logic
};

class WeaponComponent : public Node {
    // Weapon logic
};

class Player : public CharacterBody3D {
    // Uses components for modular functionality
};
```

---

## Conclusion

This guide covers the fundamentals of Godot GDExtension development. As you become more comfortable with C++, you'll discover that the combination of C++'s performance and Godot's ease of use creates a powerful game development environment.

Remember: Start with simple features and gradually build complexity. The C++ learning curve is worth it for the performance benefits and deeper engine integration!
