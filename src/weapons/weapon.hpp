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