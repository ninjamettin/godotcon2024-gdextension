extends Node3D

var gravity_sim: ExampleClass
var planet_mesh: MeshInstance3D
var satellite_mesh: MeshInstance3D
var trail_mesh: MeshInstance3D
var camera: Camera3D

# UI Controls (created in C++ logic)
var ui_panel: Panel
var sliders: Dictionary = {}
var labels: Dictionary = {}

var trail_points: Array[Vector3] = []
var max_trail_points: int = 500

func _ready():
	# Create the gravity simulator (all logic in C++)
	gravity_sim = ExampleClass.new()
	gravity_sim.set_planet(Vector3(0, 0, 0), 5000.0, 0.6)
	gravity_sim.set_gravitational_constant(50.0)
	gravity_sim.reset_to_default()
	
	# Create visual elements
	create_visual_elements()
	setup_camera()
	create_cpp_ui_controls()

func create_visual_elements():
	# Create planet (large blue sphere at origin)
	planet_mesh = MeshInstance3D.new()
	add_child(planet_mesh)
	var planet_sphere = SphereMesh.new()
	planet_sphere.radius = 0.5
	planet_sphere.height = 1.0
	planet_mesh.mesh = planet_sphere
	
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
	camera = Camera3D.new()
	add_child(camera)
	camera.position = Vector3(10, 8, 10)
	camera.look_at(Vector3.ZERO, Vector3.UP)

func create_cpp_ui_controls():
	# Create minimal UI that connects to C++ methods
	var ui_container = Control.new()
	add_child(ui_container)
	ui_container.set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	
	ui_panel = Panel.new()
	ui_container.add_child(ui_panel)
	ui_panel.position = Vector2(20, 20)
	ui_panel.size = Vector2(350, 320)
	
	var panel_style = StyleBoxFlat.new()
	panel_style.bg_color = Color(0, 0, 0, 0.8)
	panel_style.corner_radius_top_left = 10
	panel_style.corner_radius_top_right = 10
	panel_style.corner_radius_bottom_left = 10
	panel_style.corner_radius_bottom_right = 10
	ui_panel.add_theme_stylebox_override("panel", panel_style)
	
	# Title
	var title = Label.new()
	ui_panel.add_child(title)
	title.text = "C++ Orbital Simulator (Real-time)"
	title.position = Vector2(10, 10)
	title.add_theme_color_override("font_color", Color.WHITE)
	title.add_theme_font_size_override("font_size", 16)
	
	# Distance control
	create_slider_control("Distance", 40, 1.0, 15.0, gravity_sim.get_ui_distance(), 
		func(value): gravity_sim.set_ui_distance(value))  # Distance changes need apply button
	
	# Magnitude control  
	create_slider_control("Velocity Mag", 80, 0.0, 8.0, gravity_sim.get_ui_velocity_magnitude(),
		func(value): 
			gravity_sim.set_ui_velocity_magnitude(value)
			gravity_sim.apply_ui_parameters_realtime()  # Real-time update
	)
	
	# Direction controls
	var vel_dir = gravity_sim.get_ui_velocity_direction()
	create_slider_control("Dir X", 120, -1.0, 1.0, vel_dir.x,
		func(value): 
			update_direction_component(0, value)
			gravity_sim.apply_ui_parameters_realtime()  # Real-time update
	)
	create_slider_control("Dir Y", 160, -1.0, 1.0, vel_dir.y,
		func(value): 
			update_direction_component(1, value)
			gravity_sim.apply_ui_parameters_realtime()  # Real-time update
	)
	create_slider_control("Dir Z", 200, -1.0, 1.0, vel_dir.z,
		func(value): 
			update_direction_component(2, value)
			gravity_sim.apply_ui_parameters_realtime()  # Real-time update
	)
	
	# Apply button
	var apply_btn = Button.new()
	ui_panel.add_child(apply_btn)
	apply_btn.text = "Apply Distance\n& Reset Position"
	apply_btn.position = Vector2(10, 240)
	apply_btn.size = Vector2(200, 30)
	apply_btn.connect("pressed", func(): 
		gravity_sim.apply_ui_parameters()  # Full reset with new position
		trail_points.clear()
		update_all_labels()
	)
	
	# Preset buttons
	var preset_y = 280
	create_preset_button("Circular", Vector2(10, preset_y), func(): 
		gravity_sim.set_circular_orbit(gravity_sim.get_ui_distance())
		trail_points.clear()
		update_all_sliders_and_labels()
	)
	create_preset_button("Elliptical", Vector2(90, preset_y), func(): 
		gravity_sim.set_elliptical_orbit(gravity_sim.get_ui_distance(), 0.5)
		trail_points.clear()
		update_all_sliders_and_labels()
	)
	create_preset_button("Inclined", Vector2(170, preset_y), func(): 
		gravity_sim.set_inclined_orbit(gravity_sim.get_ui_distance(), 45.0)
		trail_points.clear()
		update_all_sliders_and_labels()
	)
	create_preset_button("Reset", Vector2(250, preset_y), func(): 
		gravity_sim.reset_to_default()
		trail_points.clear()
		update_all_sliders_and_labels()
	)

func create_slider_control(name: String, y_pos: int, min_val: float, max_val: float, initial_val: float, callback: Callable):
	var label = Label.new()
	ui_panel.add_child(label)
	label.text = name + ":"
	label.position = Vector2(10, y_pos)
	label.add_theme_color_override("font_color", Color.WHITE)
	
	var value_label = Label.new()
	ui_panel.add_child(value_label)
	value_label.position = Vector2(280, y_pos)
	value_label.add_theme_color_override("font_color", Color.YELLOW)
	labels[name] = value_label
	
	var slider = HSlider.new()
	ui_panel.add_child(slider)
	slider.position = Vector2(80, y_pos)
	slider.size = Vector2(190, 20)
	slider.min_value = min_val
	slider.max_value = max_val
	slider.value = initial_val
	slider.step = 0.01 if max_val <= 1.0 else 0.1
	slider.connect("value_changed", func(value): 
		callback.call(value)
		update_label(name, value)
	)
	sliders[name] = slider
	update_label(name, initial_val)

func create_preset_button(name: String, pos: Vector2, callback: Callable):
	var btn = Button.new()
	ui_panel.add_child(btn)
	btn.text = name
	btn.position = pos
	btn.size = Vector2(70, 25)
	btn.connect("pressed", callback)

func update_direction_component(component: int, value: float):
	var dir = gravity_sim.get_ui_velocity_direction()
	if component == 0:
		dir.x = value
	elif component == 1:
		dir.y = value
	else:
		dir.z = value
	gravity_sim.set_ui_velocity_direction(dir)

func update_label(name: String, value: float):
	if labels.has(name):
		labels[name].text = "%.2f" % value

func update_all_labels():
	update_label("Distance", gravity_sim.get_ui_distance())
	update_label("Velocity Mag", gravity_sim.get_ui_velocity_magnitude())
	var dir = gravity_sim.get_ui_velocity_direction()
	update_label("Dir X", dir.x)
	update_label("Dir Y", dir.y)
	update_label("Dir Z", dir.z)

func update_all_sliders_and_labels():
	sliders["Distance"].value = gravity_sim.get_ui_distance()
	sliders["Velocity Mag"].value = gravity_sim.get_ui_velocity_magnitude()
	var dir = gravity_sim.get_ui_velocity_direction()
	sliders["Dir X"].value = dir.x
	sliders["Dir Y"].value = dir.y
	sliders["Dir Z"].value = dir.z
	update_all_labels()

func _process(delta):
	# All physics handled in C++
	gravity_sim.update_physics(delta)
	
	# Update visuals
	satellite_mesh.position = gravity_sim.get_satellite_position()
	
	# Change satellite color if colliding
	var satellite_material = satellite_mesh.material_override as StandardMaterial3D
	if gravity_sim.is_satellite_colliding():
		satellite_material.albedo_color = Color.YELLOW
		satellite_material.emission = Color.YELLOW * 0.3
	else:
		satellite_material.albedo_color = Color.RED
		satellite_material.emission = Color.RED * 0.3
	
	# Update trail
	trail_points.append(gravity_sim.get_satellite_position())
	if trail_points.size() > max_trail_points:
		trail_points.pop_front()
	update_trail_mesh()
	
	# Rotate camera
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

func _input(event):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_R:
				gravity_sim.reset_to_default()
				trail_points.clear()
				update_all_sliders_and_labels()
			KEY_1:
				gravity_sim.set_circular_orbit(5.0)
				trail_points.clear()
				update_all_sliders_and_labels()
			KEY_2:
				gravity_sim.set_elliptical_orbit(5.0, 0.8)
				trail_points.clear()
				update_all_sliders_and_labels()
			KEY_3:
				gravity_sim.set_inclined_orbit(5.0, 60.0)
				trail_points.clear()
				update_all_sliders_and_labels()
			KEY_C:
				set_process(not is_processing())
