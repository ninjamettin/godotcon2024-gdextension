extends Node2D

var gravity_sim: ExampleClass
var planet_sprite: Sprite2D
var satellite_sprite: Sprite2D
var trail_line: Line2D

var trail_points: Array[Vector2] = []
var max_trail_points: int = 200

func _ready():
	# Create the gravity simulator
	gravity_sim = ExampleClass.new()
	
	# Set up the planet (center of screen, large mass)
	gravity_sim.set_planet(Vector2(400, 300), 2000.0)
	gravity_sim.set_gravitational_constant(50000.0)
	
	# Set up satellite with initial position and velocity for orbit
	gravity_sim.set_satellite(Vector2(400, 150), Vector2(80, 0), 1.0)
	
	# Create visual elements
	create_visual_elements()

func create_visual_elements():
	# Create planet sprite
	planet_sprite = Sprite2D.new()
	add_child(planet_sprite)
	# Create a simple colored rectangle for the planet
	var planet_texture = ImageTexture.new()
	var planet_image = Image.create(40, 40, false, Image.FORMAT_RGB8)
	planet_image.fill(Color.BLUE)
	planet_texture.set_image(planet_image)
	planet_sprite.texture = planet_texture
	planet_sprite.position = gravity_sim.get_planet_position()
	
	# Create satellite sprite
	satellite_sprite = Sprite2D.new()
	add_child(satellite_sprite)
	var satellite_texture = ImageTexture.new()
	var satellite_image = Image.create(10, 10, false, Image.FORMAT_RGB8)
	satellite_image.fill(Color.RED)
	satellite_texture.set_image(satellite_image)
	satellite_sprite.texture = satellite_texture
	
	# Create trail line
	trail_line = Line2D.new()
	add_child(trail_line)
	trail_line.width = 2.0
	trail_line.default_color = Color.GREEN

func _process(delta):
	# Update physics
	gravity_sim.update_physics(delta)
	
	# Update satellite position
	var sat_pos = gravity_sim.get_satellite_position()
	satellite_sprite.position = sat_pos
	
	# Add to trail
	trail_points.append(sat_pos)
	if trail_points.size() > max_trail_points:
		trail_points.pop_front()
	
	# Update trail line
	trail_line.clear_points()
	for point in trail_points:
		trail_line.add_point(point)

func _input(event):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_R:
				# Reset simulation
				gravity_sim.set_satellite(Vector2(400, 150), Vector2(80, 0), 1.0)
				trail_points.clear()
			KEY_SPACE:
				# Add some velocity to satellite
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 1.1, 1.0)
			KEY_B:
				# Brake satellite
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 0.9, 1.0)
