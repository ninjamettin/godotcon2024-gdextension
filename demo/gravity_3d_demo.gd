extends Node3D

var gravity_sim: ExampleClass
var planet_mesh: MeshInstance3D
var satellite_mesh: MeshInstance3D
var trail_mesh: MeshInstance3D
var camera: Camera3D

var trail_points: Array[Vector3] = []
var max_trail_points: int = 500

func _ready():
	# Create the gravity simulator
	gravity_sim = ExampleClass.new()
	
	# Set up the planet at origin with radius for collision detection
	gravity_sim.set_planet(Vector3(0, 0, 0), 5000.0, 0.6)
	gravity_sim.set_gravitational_constant(50.0)
	
	# Set up satellite with proper circular orbit velocity
	# For circular orbit: v = sqrt(GM/r)
	gravity_sim.set_satellite(Vector3(5, 0, 0), Vector3(0, 0, 3.16), 1.0)
	
	# Create visual elements
	create_visual_elements()
	setup_camera()

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

func _input(event):
	if event is InputEventKey and event.pressed:
		match event.keycode:
			KEY_R:
				# Reset simulation to stable circular orbit
				gravity_sim.set_satellite(Vector3(5, 0, 0), Vector3(0, 0, 3.16), 1.0)
				trail_points.clear()
			KEY_SPACE:
				# Speed up satellite (careful not to make it too fast)
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 1.05, 1.0)
			KEY_B:
				# Brake satellite
				var current_vel = gravity_sim.get_satellite_velocity()
				gravity_sim.set_satellite(gravity_sim.get_satellite_position(), current_vel * 0.95, 1.0)
			KEY_1:
				# Stable circular orbit
				gravity_sim.set_satellite(Vector3(5, 0, 0), Vector3(0, 0, 3.16), 1.0)
				trail_points.clear()
			KEY_2:
				# Stable elliptical orbit
				gravity_sim.set_satellite(Vector3(5, 0, 0), Vector3(0, 0.5, 3.5), 1.0)
				trail_points.clear()
			KEY_3:
				# Stable inclined orbit
				gravity_sim.set_satellite(Vector3(4, 2, 2), Vector3(1, 1, 2.5), 1.0)
				trail_points.clear()
			KEY_4:
				# High orbit (slower)
				gravity_sim.set_satellite(Vector3(8, 0, 0), Vector3(0, 0, 2.5), 1.0)
				trail_points.clear()
			KEY_C:
				# Toggle camera rotation
				set_process(not is_processing())
