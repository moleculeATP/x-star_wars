#include "scene.hpp"

using namespace cgp;


bool show_asteroids = true;

// This function is called only once at the beginning of the program
// This function can contain any complex operation that can be pre-computed once
void scene_structure::initialize()
{
	
	// Set the behavior of the camera and its initial position
	// ********************************************** //
	if(camera_fixe){
		camera_control_fixe.initialize(inputs, window);
	}
	else{
		camera_control.initialize(inputs, window);
	}
	
	//camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control_fixe.look_at(
		{-2.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f} // up direction
	);
	
	/** 
	// parametre controle camera mode combat
	camera_control.speed = 0.4f;
	camera_control.speed_increase = 1.02f;
	camera_control.speed_max = 1.2f;
	camera_control.speed_min = 0.4f;
		*/

	display_info();

	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Skybox
	image_structure image_skybox_template = image_load_file(project::path+"assets/skybox_05.png");
	// Split the image into a grid of 4 x 3 sub-images
	std::vector<image_structure> image_grid = image_split_grid(image_skybox_template, 4, 3);
	skybox.initialize_data_on_gpu();
	skybox.texture.initialize_cubemap_on_gpu(image_grid[10], image_grid[4], image_grid[9], image_grid[11], image_grid[1], image_grid[7]);

	// Create the shapes seen in the 3D scene
	// ********************************************** //
	cube.initialize_data_on_gpu(mesh_primitive_cube(/*center*/{ 0,0,0 }, /*edge length*/ 1.0f));
	cube.material.color = { 1,1,0 };  
	cube.model.translation = { 1,3,0 }; 

	// Same process for the ground which is a plane 
	//  A quadrangle is defined a plane with 4-extremal corners.
	int N = 10;
	float L_ground = 10.0f;
	float z_ground = -0.51f;
	mesh ground_mesh = mesh_primitive_grid(
		{ -L_ground, -L_ground, z_ground },
		{  L_ground, -L_ground, z_ground },
		{  L_ground,  L_ground, z_ground },
		{ -L_ground,  L_ground, z_ground },
		N,
		N);
	
	numarray<vec2> uv = ground_mesh.uv;
	for (int i = 0; i < ground_mesh.uv.size(); i++) {
		ground_mesh.uv[i] = {N*uv[i][0], N*uv[i][1]};
	}
	ground.initialize_data_on_gpu(ground_mesh);
	ground.material.color = { 0.6, 0.6, 0.6 };
	ground.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/ground.png", GL_REPEAT, GL_REPEAT);

	// A Sphere
	mesh sphere_mesh = mesh_primitive_sphere();
	sphere.initialize_data_on_gpu(sphere_mesh);
	sphere.model.scaling = 0.2f; // coordinates are multiplied by 0.2 in the shader
	sphere.model.translation = { 1,2,0 }; // coordinates are offseted by {1,2,0} in the shader
	sphere.material.color = { 1,0.5f,0.5f }; // sphere will appear red (r,g,b components in [0,1])

	// Camel: mesh_load_file_obj: read external obj file
	camel.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/camel.obj"));
	camel.material.color = { 0.8, 0.7, 0.3 };
	camel.model.scaling = 0.1f;
	camel.model.translation = { -1,1,0.5f };


	// Asteroid mesh
	if (show_asteroids) {
		int N_mesh = 100;
		asteroid_set.N_asteroids = 26;
		std::vector<vec3> scales = std::vector<vec3>(asteroid_set.N_asteroids, {rand_uniform(0.5, 1.5), rand_uniform(0.5, 1.5), rand_uniform(0.5, 1.5)});
		asteroid_set.create_meshes(scales, N_mesh);
		asteroid_set.initialize();
		asteroid_set.update_asteroids();
		for (int k = 0; k < asteroid_set.N_asteroids; k++) {
			asteroid_set.drawables[k].vbo_color.update(asteroid_set.meshes[k].color);
			asteroid_set.drawables[k].model.translation = asteroid_set.positions[k];
		}
	}

	// Sphere used to display the position of a light
	sphere_light.initialize_data_on_gpu(mesh_primitive_sphere(0.2f));
	laser_shot.initialize_data_on_gpu(mesh_primitive_ellipsoid());

	// Remove warnings for unset uniforms
	cgp_warning::max_warning = 0;


	// Load the custom shader
	opengl_shader_structure shader_mesh;
	shader_mesh.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	// Affect the loaded shader to the mesh_drawable
	camel.shader = shader_mesh;
	cube.shader = shader_mesh;
	sphere.shader = shader_mesh;

	opengl_shader_structure shader_custom;
	shader_custom.load(
		project::path + "shaders/shading_custom/shading_custom.vert.glsl",
		project::path + "shaders/shading_custom/shading_custom.frag.glsl"
	);

	ground.shader = shader_custom;

	if (show_asteroids) {
		for (int k = 0; k < asteroid_set.N_asteroids; k++){
			//asteroid_set.drawables[k].texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/rock.png", GL_REPEAT, GL_REPEAT);
			asteroid_set.drawables[k].shader = shader_custom; 
		}
	}

	xwing_ship = x_wing();
	auto struct_body = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__body.obj");
	auto struct_wing = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__wing.obj");
	xwing_ship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	xwing_ship.wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	// xwing_ship.set_shader(shader_custom);
	xwing_ship.initialize(inputs, window, shader_custom);
	

	aiship.initialize(inputs, window, shader_custom);
	aiship.target = &xwing_ship;
	mesh_drawable cone;
	cone.initialize_data_on_gpu(mesh_primitive_cone(0.1, 0.5, {0, 0, 0}, {1, 0, 0}));
	aiship.hierarchy.add(cone, "ship", "Vaisseau base", {0, 0, 0});
	// xwing_ship.laser.shader = shader_custom;

}

// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	// Update time
	float dt = timer.update();

	// Skybox
	//  Must be called before drawing the other shapes and without writing in the Depth Buffer
	glDepthMask(GL_FALSE); // disable depth-buffer writing
	draw(skybox, environment);
	glDepthMask(GL_TRUE);  // re-activate depth-buffer write


	// Set additional uniform parameters to the shader
	environment.uniform_generic.uniform_float["ambiant"] = gui.ambiant;
	environment.uniform_generic.uniform_float["diffus"] = gui.diffus;
	environment.uniform_generic.uniform_float["coef_spec"] = gui.coef_spec;
	environment.uniform_generic.uniform_float["coef_exp"] = gui.exp_spec;
	environment.uniform_generic.uniform_vec3["light_color"] = gui.light_color;
	environment.uniform_generic.uniform_vec3["brume_color"] = gui.brume_color;
	environment.uniform_generic.uniform_vec3["light_position"] = gui.light_position;

	environment.uniform_generic.uniform_int["N_lights"] = 20;
	environment.uniform_generic.uniform_vec3["light_colors[0]"] = gui.light_color;
	environment.uniform_generic.uniform_vec3["light_positions[0]"] = gui.light_position;
	environment.uniform_generic.uniform_float["d_light_max[0]"] = -1.0f;
	environment.uniform_generic.uniform_int["active_lights[0]"] = 1;

	sphere_light.model.translation = gui.light_position;
	sphere_light.material.color = gui.light_color * 0.8f;
	sphere_light.material.phong.ambient = 1;
	sphere_light.material.phong.diffuse = 0;
	sphere_light.material.phong.specular = 0;
	draw(sphere_light, environment);

	
	environment.uniform_generic.uniform_float["distance_xwing"] = 1.0f;
	environment.uniform_generic.uniform_vec3["camera_pos"] = camera_control.camera_model.position();
	//environment.uniform_generic.uniform_mat4["view"] = camera_control.camera_model.matrix_view();

	/**
	vec3 xwing_position = camera_control.camera_model.position() + camera_control.camera_model.front();
	vec3 front = normalize(camera_control.camera_model.front());
	vec3 up_cam = normalize(camera_control.camera_model.up());
	vec3 right = normalize(cross(up_cam, front));
	vec3 up = cross(front, right);  // Recalculer up pour assurer l'orthogonalité

	mat3 R = {right, up, front};


	xwing.model.translation = xwing_position;
	xwing.model.rotation = rotation_transform::from_matrix(R);
	**/

	// Lasers
	for (int i = 1; i < xwing_ship.N_lasers; i ++) {
		std::string uniform_color = "light_colors[" + std::to_string(i) + "]";
		std::string uniform_pos = "light_positions[" + std::to_string(i) + "]";
		std::string uniform_dl_max = "d_light_max[" + std::to_string(i) + "]";
		std::string uniform_active_light = "active_lights[" + std::to_string(i) + "]";
		if (xwing_ship.lasers_active[i] == 0) {
			environment.uniform_generic.uniform_int[uniform_active_light] = 0;
		} else {
			environment.uniform_generic.uniform_vec3[uniform_color] = xwing_ship.lasers_color;
			environment.uniform_generic.uniform_vec3[uniform_pos] = xwing_ship.lasers_pos[i];
			environment.uniform_generic.uniform_float[uniform_dl_max] = xwing_ship.d_light_max;
			environment.uniform_generic.uniform_int[uniform_active_light] = 1;
		}
	}

	xwing_ship.idle_frame();
	xwing_ship.draw(environment); //equivalent to draw(xwing, environment);

	//aiship.idle_frame();
	//aiship.draw(environment);

	if (show_asteroids) asteroid_set.idle_frame(dt);

	idle_frame();
	
	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);

	// the general syntax to display a mesh is:
	//   draw(mesh_drawableName, environment);
	// Note: scene is used to set the uniform parameters associated to the camera, light, etc. to the shader
	draw(ground, environment);
	draw(cube, environment);	
	draw(sphere, environment);
	draw(camel, environment);
	if (show_asteroids) {
		for (int k = 0; k < asteroid_set.N_asteroids; k++) 
			draw(asteroid_set.drawables[k], environment);
	}

	if (gui.display_wireframe) {
		draw_wireframe(ground, environment);
		draw_wireframe(sphere, environment);
		draw_wireframe(cube, environment);
		draw_wireframe(camel, environment);

		if (show_asteroids) {
			for (int k = 0; k < asteroid_set.N_asteroids; k++) 
				draw_wireframe(asteroid_set.drawables[k], environment);
		}
	}

	environment.background_color = gui.brume_color;

	if (gui.display_ship_arrow) {
		draw(xwing_ship.arrow_up, environment);
		draw(xwing_ship.arrow_velocity, environment);
		draw(xwing_ship.arrow_left, environment);
	}

}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("ship_arrow", &gui.display_ship_arrow);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

	ImGui::ColorEdit3("Light color", &gui.light_color[0]);
	ImGui::ColorEdit3("Brume", &gui.brume_color[0]);
	ImGui::SliderFloat3("Light position", &gui.light_position[0], -3.0f, 3.0f);

	ImGui::SliderFloat("Ambiant", &gui.ambiant, 0.0f, 1.0f);
	ImGui::SliderFloat("Diffus", &gui.diffus, 0.0f, 1.0f);
	ImGui::SliderFloat("Spéculaire", &gui.coef_spec, 0.0f, 1.0f);
	ImGui::SliderFloat("Spéculaire_exp", &gui.exp_spec, 0.0f, 256.f);
}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);	
}
void scene_structure::idle_frame()
{
	if(camera_fixe)
		camera_control_fixe.idle_frame(environment.camera_view);
	else
		camera_control.idle_frame(environment.camera_view, xwing_ship);	
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;


	std::cout << "\nSCENE INFO:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "Exercise on Shading on GPU." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}