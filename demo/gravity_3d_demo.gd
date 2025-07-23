extends Node3D

var gravity_sim: ExampleClass
var planet_mesh: MeshInstance3D
var satellite_mesh: MeshInstance3D
var trail_mesh: MeshInstance3D
var camera: Camera3D

# UI Controls
var distance_slider: HSlider
var velocity_x_slider: HSlider
var velocity_y_slider: HSlider
var velocity_z_slider: HSlider
var magnitude_slider: HSlider
var distance_label: Label
var velocity_label: Label
var magnitude_label: Label

var trail_points: Array[Vector3] = []
var max_trail_points: int = 500

var current_distance: float = 5.0
var current_velocity: Vector3 = Vector3(0, 0, 3.16)

func _ready():
	# Create the gravity simulator
	gravity_sim = ExampleClass.new()
	
	# Set up the planet at origin with radius for collision detection
	gravity_sim.set_planet(Vector3(0, 0, 0), 5000.0, 0.6)
	gravity_sim.set_gravitational_constant(50.0)
	
	# Set up satellite with proper circular orbit velocity
	gravity_sim.set_satellite(Vector3(current_distance, 0, 0), current_velocity, 1.0)
	
	# Create visual elements
	create_visual_elements()
	setup_camera()
	create_ui_controls()

func create_visual_elements():
	# Create planet (large blue sphere at origin)
	planet_mesh = MeshInstance3D.new()
	add_child(planet_mesh)
	var planet_sphere = SphereMesh.new()
	planet_sphere.radius = 0.5
	planet_sphere.height = 1.0
	planet_mesh.mesh = planet_sphere
	
	# Create material for planet
	var planet_material = StandardMaterial3D.new()
	planet_material.albedo_color = Color.BLUE
	planet_material.emission = Color.BLUE * 0.2
	planet_mesh.material_override = planet_material
	planet_mesh.position = gravity_sim.get_planet_position()
	
	# Create satellite (small red sphere)
	satellite_mesh = MeshInstance3D.new()
	add_child(satellite_mesh)
	var satellite_sphere = SphereMesh.new()
	satellite_sphere.radius = 0.1
	satellite_sphere.height = 0.2
	satellite_mesh.mesh = satellite_sphere
	
	# Create material for satellite
	var satellite_material = StandardMaterial3D.new()
	satellite_material.albedo_color = Color.RED
	satellite_material.emission = Color.RED * 0.3
	satellite_mesh.material_override = satellite_material
	
	# Create trail mesh
	trail_mesh = MeshInstance3D.new()
	add_child(trail_mesh)
	var trail_material = StandardMaterial3D.new()
	trail_material.albedo_color = Color.GREEN
	trail_material.emission = Color.GREEN * 0.5
	trail_material.flags_unshaded = true
	trail_material.vertex_color_use_as_albedo = true
	trail_mesh.material_override = trail_material

func setup_camera():
	# Create and position camera
	camera = Camera3D.new()
	add_child(camera)
	camera.position = Vector3(10, 8, 10)
	camera.look_at(Vector3.ZERO, Vector3.UP)

func create_ui_controls():
	# Create UI container
	var ui_container = Control.new()
	add_child(ui_container)
	ui_container.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	
	# Create control panel background
	var panel = Panel.new()
	ui_container.add_child(panel)
	panel.position = Vector2(20, 20)
	panel.size = Vector2(350, 280)
	
	var panel_style = StyleBoxFlat.new()
	panel_style.bg_color = Color(0, 0, 0, 0.7)
	panel_style.corner_radius_top_left = 10
	panel_style.corner_radius_top_right = 10
	panel_style.corner_radius_bottom_left = 10
	panel_style.corner_radius_bottom_right = 10
	panel.add_theme_stylebox_override("panel", panel_style)
	
	# Title
	var title_label = Label.new()
	panel.add_child(title_label)
	title_label.text = "Orbital Parameters"
	title_label.position = Vector2(10, 10)
	title_label.add_theme_color_override("font_color", Color.WHITE)
	title_label.add_theme_font_size_override("font_size", 18)
	
	# Distance controls
	var distance_title = Label.new()
	panel.add_child(distance_title)
	distance_title.text = "Starting Distance:"
	distance_title.position = Vector2(10, 40)
	distance_title.add_theme_color_override("font_color", Color.WHITE)
	
	distance_label = Label.new()
	panel.add_child(distance_label)
	distance_label.position = Vector2(250, 40)
	distance_label.add_theme_color_override("font_color", Color.YELLOW)
	update_distance_label()
	
	distance_slider = HSlider.new()
	panel.add_child(distance_slider)
	distance_slider.position = Vector2(10, 60)
	distance_slider.size = Vector2(320, 20)
	distance_slider.min_value = 1.0
	distance_slider.max_value = 55.0
	distance_slider.value = current_distance
	distance_slider.step = 0.1
	distance_slider.connect("value_changed", _on_distance_changed)
	
	# Velocity magnitude controls
	var magnitude_title = Label.new()
	panel.add_child(magnitude_title)
	magnitude_title.text = "Velocity Magnitude:"
	magnitude_title.position = Vector2(10, 90)
	magnitude_title.add_theme_color_override("font_color", Color.WHITE)
	
	magnitude_label = Label.new()
	panel.add_child(magnitude_label)
	magnitude_label.position = Vector2(250, 90)
	magnitude_label.add_theme_color_override("font_color", Color.YELLOW)
	update_magnitude_label()
	
	magnitude_slider = HSlider.new()
	panel.add_child(magnitude_slider)
	magnitude_slider.position = Vector2(10, 110)
	magnitude_slider.size = Vector2(320, 20)
	magnitude_slider.min_value = 0.0
	magnitude_slider.max_value = 80.0
	magnitude_slider.value = current_velocity.length()
	magnitude_slider.step = 0.1
	magnitude_slider.connect("value_changed", _on_magnitude_changed)
	
	# Velocity direction controls
	var velocity_title = Label.new()
	panel.add_child(velocity_title)
	velocity_title.text = "Velocity Direction (X, Y, Z):"
	velocity_title.position = Vector2(10, 140)
	velocity_title.add_theme_color_override("font_color", Color.WHITE)
	
	velocity_label = Label.new()
	panel.add_child(velocity_label)
	velocity_label.position = Vector2(10, 250)
	velocity_label.add_theme_color_override("font_color", Color.CYAN)
	update_velocity_label()
	
	# X velocity
	var x_label = Label.new()
	panel.add_child(x_label)
	x_label.text = "X:"
	x_label.position = Vector2(10, 160)
	x_label.add_theme_color_override("font_color", Color.WHITE)
	
	velocity_x_slider = HSlider.new()
	panel.add_child(velocity_x_slider)
	velocity_x_slider.position = Vector2(30, 160)
	velocity_x_slider.size = Vector2(200, 20)
	velocity_x_slider.min_value = -1.0
	velocity_x_slider.max_value = 1.0
	velocity_x_slider.value = current_velocity.normalized().x
	velocity_x_slider.step = 0.01
	velocity_x_slider.connect("value_changed", _on_velocity_direction_changed)
	
	# Y velocity
	var y_label = Label.new()
	panel.add_child(y_label)
	y_label.text = "Y:"
	y_label.position = Vector2(10, 185)
	y_label.add_theme_color_override("font_color", Color.WHITE)
	
	velocity_y_slider = HSlider.new()
	panel.add_child(velocity_y_slider)
	velocity_y_slider.position = Vector2(30, 185)
	velocity_y_slider.size = Vector2(200, 20)
	velocity_y_slider.min_value = -1.0
	velocity_y_slider.max_value = 1.0
	velocity_y_slider.value = current_velocity.normalized().y
	velocity_y_slider.step = 0.01
	velocity_y_slider.connect("value_changed", _on_velocity_direction_changed)
	
	# Z velocity
	var z_label = Label.new()
	panel.add_child(z_label)
	z_label.text = "Z:"
	z_label.position = Vector2(10, 210)
	z_label.add_theme_color_override("font_color", Color.WHITE)
	
	velocity_z_slider = HSlider.new()
	panel.add_child(velocity_z_slider)
	velocity_z_slider.position = Vector2(30, 210)
	velocity_z_slider.size = Vector2(200, 20)
	velocity_z_slider.min_value = -1.0
	velocity_z_slider.max_value = 1.0
	velocity_z_slider.value = current_velocity.normalized().z
	velocity_z_slider.step = 0.01
	velocity_z_slider.connect("value_changed", _on_velocity_direction_changed)
	
	# Apply button
	var apply_button = Button.new()
	panel.add_child(apply_button)
	apply_button.text = "Apply & Reset Orbit"
	apply_button.position = Vector2(240, 160)
	apply_button.size = Vector2(100, 60)
	apply_button.connect("pressed", _on_apply_pressed)

func _process(delta):
	# Update physics
	gravity_sim.update_physics(delta)
	
	# Update satellite position
	var sat_pos = gravity_sim.get_satellite_position()
	satellite_mesh.position = sat_pos
	
	# Change satellite color if colliding
	var satellite_material = satellite_mesh.material_override as StandardMaterial3D
	if gravity_sim.is_satellite_colliding():
		satellite_material.albedo_color = Color.YELLOW
		satellite_material.emission = Color.YELLOW * 0.3
	else:
		satellite_material.albedo_color = Color.RED
		satellite_material.emission = Color.RED * 0.3
	
	# Add to trail
	trail_points.append(sat_pos)
	if trail_points.size() > max_trail_points:
		trail_points.pop_front()
	
	# Update trail mesh
	update_trail_mesh()
	
	# Rotate camera around the system
	camera.position = camera.position.rotated(Vector3.UP, delta * 0.2)
	camera.look_at(Vector3.ZERO, Vector3.UP)

func update_trail_mesh():
	if trail_points.size() < 2:
		return
	
	var array_mesh = ArrayMesh.new()
	var arrays = []
	arrays.resize(Mesh.ARRAY_MAX)
	
	var vertices = PackedVector3Array()
	var colors = PackedColorArray()
	
	for i in range(trail_points.size() - 1):
		var point1 = trail_points[i]
		var point2 = trail_points[i + 1]
		var alpha = float(i) / float(trail_points.size())
		
		vertices.append(point1)
		vertices.append(point2)
		colors.append(Color(0, 1, 0, alpha * 0.5))
		colors.append(Color(0, 1, 0, alpha * 0.5))
	
	arrays[Mesh.ARRAY_VERTEX] = vertices
	arrays[Mesh.ARRAY_COLOR] = colors
	
	array_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_LINES, arrays)
	trail_mesh.mesh = array_mesh

func _on_distance_changed(value: float):
	current_distance = value
	update_distance_label()

func _on_magnitude_changed(value: float):
	var direction = Vector3(velocity_x_slider.value, velocity_y_slider.value, velocity_z_slider.value).normalized()
	current_velocity = direction * value
	update_magnitude_label()
	update_velocity_label()

func _on_velocity_direction_changed(_value: float):
	var direction = Vector3(velocity_x_slider.value, velocity_y_slider.value, velocity_z_slider.value).normalized()
	var magnitude = magnitude_slider.value
	current_velocity = direction * magnitude
	update_velocity_label()

func _on_apply_pressed():
	# Calculate starting position based on distance
	var start_position = Vector3(current_distance, 0, 0)
	
	# Apply new parameters
	gravity_sim.set_satellite(start_position, current_velocity, 1.0)
	trail_points.clear()
	
	print("Applied new orbit: Distance=", current_distance, " Velocity=", current_velocity)

func update_distance_label():
	distance_label.text = "%.1f" % current_distance

func update_magnitude_label():
	magnitude_label.text = "%.2f" % current_velocity.length()

func update_velocity_label():
	velocity_label.text = "Velocity: (%.2f, %.2f, %.2f)" % [current_velocity.x, current_velocity.y, current_velocity.z]

func _input(event):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_R:
				# Reset simulation to stable circular orbit
				current_distance = 5.0
				current_velocity = Vector3(0, 0, 3.16)
				distance_slider.value = current_distance
				magnitude_slider.value = current_velocity.length()
				velocity_x_slider.value = 0.0
				velocity_y_slider.value = 0.0
				velocity_z_slider.value = 1.0
				_on_apply_pressed()
				update_distance_label()
				update_magnitude_label()
				update_velocity_label()
			KEY_SPACE:
				# Speed up satellite (slightly)
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 1.05, 1.0)
			KEY_B:
				# Brake satellite
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 0.95, 1.0)
			KEY_1:
				# Stable circular orbit
				current_distance = 5.0
				current_velocity = Vector3(0, 0, 3.16)
				_update_sliders_and_apply()
			KEY_2:
				# Stable elliptical orbit
				current_distance = 5.0
				current_velocity = Vector3(0, 0.5, 3.5)
				_update_sliders_and_apply()
			KEY_3:
				# Stable inclined orbit
				current_distance = 5.0
				current_velocity = Vector3(1, 1, 2.5)
				_update_sliders_and_apply()
			KEY_4:
				# High orbit (slower)
				current_distance = 8.0
				current_velocity = Vector3(0, 0, 2.5)
				_update_sliders_and_apply()
			KEY_C:
				# Toggle camera rotation
				set_process(not is_processing())

func _update_sliders_and_apply():
	distance_slider.value = current_distance
	magnitude_slider.value = current_velocity.length()
	var normalized_vel = current_velocity.normalized()
	velocity_x_slider.value = normalized_vel.x
	velocity_y_slider.value = normalized_vel.y
	velocity_z_slider.value = normalized_vel.z
	_on_apply_pressed()
	update_distance_label()
	update_magnitude_label()
	update_velocity_label()
