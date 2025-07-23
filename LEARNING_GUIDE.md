# Physics Simulation Learning Guide

## What You Have Now:
- **Clean C++ Template**: `src/example_class.h` and `src/example_class.cpp`
- **Simple GDScript Template**: `demo/learning_template.gd`
- **Basic Scene**: `demo/learning_template.tscn`

## Learning Path - Build Your Physics Simulation Step by Step:

### Step 1: Basic Object with Position
**Goal**: Create a simple object that has a position in 3D space

**C++ Tasks**:
1. Add a `Vector3 position` variable to your class
2. Create `get_position()` and `set_position(Vector3 pos)` methods
3. Bind these methods in `_bind_methods()`

**GDScript Tasks**:
1. Create a sphere mesh to visualize your object
2. Set the sphere's position using your C++ methods
3. Test by setting different positions

### Step 2: Add Velocity and Movement
**Goal**: Make your object move through space

**C++ Tasks**:
1. Add `Vector3 velocity` variable
2. Create `get_velocity()` and `set_velocity(Vector3 vel)` methods
3. Create `update_physics(float delta_time)` method that updates position: `position += velocity * delta_time`
4. Bind all new methods

**GDScript Tasks**:
1. Call `update_physics(delta)` in your `_process()` function
2. Update the visual sphere position each frame
3. Experiment with different initial velocities

### Step 3: Add Acceleration/Forces
**Goal**: Apply forces that change velocity over time

**C++ Tasks**:
1. Add variables for mass, forces
2. Create methods to apply forces
3. Update physics: `velocity += (force / mass) * delta_time`

**GDScript Tasks**:
1. Add input handling to apply forces (keyboard controls)
2. Visualize force vectors (optional)

### Step 4: Add Gravity Simulation
**Goal**: Create gravitational attraction between objects

**C++ Tasks**:
1. Add a second object (planet) with position and mass
2. Calculate gravitational force: `F = G * m1 * m2 / r²`
3. Apply this force to update velocity

**GDScript Tasks**:
1. Create visual representations for both objects
2. Watch orbital mechanics emerge!

### Step 5: Add UI Controls
**Goal**: Interactive parameter adjustment

**C++ Tasks**:
1. Add getter/setter methods for simulation parameters
2. Create preset configuration methods

**GDScript Tasks**:
1. Create sliders to control parameters
2. Connect slider changes to C++ methods

## Physics Formulas You'll Need:
- **Position Update**: `position += velocity * delta_time`
- **Velocity Update**: `velocity += acceleration * delta_time`
- **Newton's 2nd Law**: `acceleration = force / mass`
- **Gravitational Force**: `F = G * m1 * m2 / distance²`
- **Distance**: `distance = (pos1 - pos2).length()`

## Key C++ Concepts:
- **Class Member Variables**: Store your simulation state
- **Method Binding**: Make C++ methods callable from GDScript
- **Vector3 Operations**: Position, velocity calculations
- **const Methods**: Methods that don't modify the object

## Key GDScript Concepts:
- **_process(delta)**: Called every frame for updates
- **MeshInstance3D**: Visual representation of objects
- **Input Handling**: User interaction
- **Connecting C++ to Visuals**: Bridge between logic and graphics

## Build Tips:
1. **Start Simple**: Get one thing working before adding complexity
2. **Test Often**: Build and test after each small addition
3. **Debug with Print**: Use `print()` in GDScript to see values
4. **Visual Feedback**: Always have visual representation of your data
5. **Incremental Development**: Add one feature at a time

## Next Steps After Template:
1. Follow the learning path above
2. Experiment with different force models
3. Add collision detection
4. Try multiple objects
5. Create different types of simulations (springs, pendulums, etc.)

Good luck with your physics simulation journey! 🚀
