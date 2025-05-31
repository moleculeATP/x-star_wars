#include "scene.hpp"

using namespace cgp;

bool show_asteroids = true;
int nb_of_ia_combat = 0; // 2 AI ship fighting each other
bool SKYBOX = true;

void scene_structure::initialize()
{
	if(camera_fixe){
		camera_control_fixe.initialize(inputs, window);
	}
	else{
		camera_control.initialize(inputs, window);
	}

	camera_control_fixe.look_at(
		{-2.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 1.0f} // up direction
	);

	display_info();

	// Create the global (x,y,z) frame
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// ---------------- Loading shaders ----------------
	
	shader_mesh.load(
		project::path + "shaders/mesh/mesh.vert.glsl",
		project::path + "shaders/mesh/mesh.frag.glsl");

	
	shader_custom.load(
		project::path + "shaders/shading_custom/shading_custom.vert.glsl",
		project::path + "shaders/shading_custom/shading_custom.frag.glsl"
	);

	laser_shader.load(
		project::path + "shaders/shading_custom/shading_custom.vert.glsl",
		project::path + "shaders/shading_custom/laser.frag.glsl"
	);

	reactor_shader.load(
		project::path + "shaders/shading_custom/shading_custom.vert.glsl",
		project::path + "shaders/shading_custom/reactor.frag.glsl"
	);

	// ---------------- Skybox ----------------
	if(SKYBOX){
		image_structure image_skybox_template = image_load_file(project::path+"assets/skybox_05.png");
		// Split the image into a grid of 4 x 3 sub-images
		std::vector<image_structure> image_grid = image_split_grid(image_skybox_template, 4, 3);
		skybox.initialize_data_on_gpu();
		skybox.texture.initialize_cubemap_on_gpu(image_grid[10], image_grid[4], image_grid[9], image_grid[11], image_grid[1], image_grid[7]);
	}
	// ---------------------------- Shapes ----------------------------
	cube.initialize_data_on_gpu(mesh_primitive_cube(/*center*/{ 0,0,0 }, /*edge length*/ 1.0f));
	cube.material.color = { 1,1,0 };  
	cube.model.translation = { 1,3,0 }; 

	// Ground
	/**
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
	*/

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

	// Sphere used to display the position of a light
	sphere_light.initialize_data_on_gpu(mesh_primitive_sphere(0.2f));
	laser_shot.initialize_data_on_gpu(mesh_primitive_ellipsoid());

	// Asteroids
	if (show_asteroids) {
		int N_uv = 100;
		asteroid_set.N_mesh = 10;
		asteroid_set.N_asteroids = 30;
		std::vector<vec3> scales = std::vector<vec3>(asteroid_set.N_mesh, {rand_uniform(0.5, 6.0), rand_uniform(0.5, 1.5), rand_uniform(0.5, 1.5)});
		asteroid_set.initialize(scales, N_uv, project::path + "assets/asteroid1.jpg", shader_custom);
		asteroid_set.apply_perlin();
	}

	// Remove warnings for unset uniforms
	cgp_warning::max_warning = 0;

	// Affect the loaded shader to the mesh_drawable
	camel.shader = shader_mesh;
	cube.shader = shader_mesh;
	sphere.shader = shader_mesh;
	//ground.shader = shader_custom;

	
	auto struct_body = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__body.obj");
	auto struct_wing = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__wing.obj");
	//auto struct_body_2 = mesh_load_file_obj_advanced(project::path + "assets/tie_model/", "tie.obj");
	auto struct_reactor = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "reactor.obj");
	xwing_ship.wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	xwing_ship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	xwing_ship.reactor.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/x_wing_model/reactor.obj"));
	
	xwing_ship.initialize(inputs, window, shader_custom, laser_shader, reactor_shader);
	xwing_ship.environment = &environment;

	/** code test IA
	xwing_aiship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	xwing_aiship.wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	xwing_aiship.initialize(inputs, window, shader_custom, laser_shader);
	xwing_aiship.hierarchy["Vaisseau base"].transform_local.translation = {8, 5, 2};
	xwing_aiship.set_target(&passivship);
	xwing_aiship.speed = 0.025f;

	auto struct_body_2 = mesh_load_file_obj_advanced(project::path + "assets/tie_model/", "tie.obj");
	passivship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body_2);
	passivship.initialize(inputs, window, shader_custom, laser_shader);
	passivship.hierarchy["Vaisseau base"].transform_local.translation = {5, 0, 0};
	passivship.speed = 0.028f;
	
	auto struct_body_2 = mesh_load_file_obj_advanced(project::path + "assets/tie_model/", "tie.obj");
	passivship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body_2);

	passivship.initialize(inputs, window, shader_custom, laser_shader);
	passivship.hierarchy["Vaisseau base"].transform_local.translation = {5, 0, 0};
	*/
	/**
	std::vector<mesh_drawable> x_wing_body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	std::vector<mesh_drawable> x_wing_wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	std::vector<mesh_drawable> tie_body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body_2);
	std::vector<mesh> mesh_tie;
	mesh_tie.resize(tie_body.size());
	for (int i = 0; i < tie_body.size(); i++) {
		mesh_tie[i] = struct_body_2[i].mesh_element;
	}

	victims.resize(nb_of_ia_combat);
	chads.resize(nb_of_ia_combat);
	
	for(int i = 0; i < nb_of_ia_combat; i++){

		if(i % 2 == 0){
			x_wing_passiv_ship victim;
			ai_ship chad;
			victim.body = x_wing_body;
			victim.wing = x_wing_wing;
			victim.initialize(inputs, window, shader_custom, laser_shader);
			chad.body = tie_body;
			chad.debris_mesh = mesh_tie;
			chad.initialize(inputs, window, shader_custom, laser_shader);

			victims[i] = victim;
			chads[i] = chad;
		}else{
			x_wing_ai_ship chad;
			passiv_ship victim;
			chad.body = x_wing_body;
			chad.wing = x_wing_wing;
			chad.initialize(inputs, window, shader_custom, laser_shader);
			victim.body = tie_body;
			victim.debris_mesh = mesh_tie;
			victim.initialize(inputs, window, shader_custom, laser_shader);

			victims[i] = victim;
			chads[i] = chad;
		}
		
		chads[i].speed = 0.04f;
		victims[i].speed = 0.043f;

		chads[i].set_target(&victims[i]);

		victims[i].hierarchy["Vaisseau base"].transform_local.translation = 
			vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
		chads[i].hierarchy["Vaisseau base"].transform_local.translation = 
			vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
	}
			**/
}

void scene_structure::display_frame()
{
	// Update time
	float dt = timer.update();

	if(SKYBOX){
		// Skybox
		//  Must be called before drawing the other shapes and without writing in the Depth Buffer
		glDepthMask(GL_FALSE); // disable depth-buffer writing
		draw(skybox, environment);
		glDepthMask(GL_TRUE);  // re-activate depth-buffer write
	}
	// Set additional uniform parameters to the shader
	environment.uniform_generic.uniform_float["ambiant"] = gui.ambiant;
	environment.uniform_generic.uniform_float["diffus"] = gui.diffus;
	environment.uniform_generic.uniform_float["coef_spec"] = gui.coef_spec;
	environment.uniform_generic.uniform_float["coef_exp"] = gui.exp_spec;
	environment.uniform_generic.uniform_vec3["light_color"] = gui.light_color;
	environment.uniform_generic.uniform_vec3["brume_color"] = gui.brume_color;
	environment.uniform_generic.uniform_vec3["light_position"] = gui.light_position;
	environment.uniform_generic.uniform_float["distance_xwing"] = 1.0f;
	environment.uniform_generic.uniform_vec3["camera_pos"] = camera_control.camera_model.position();
	environment.uniform_generic.uniform_mat4["view"] = camera_control.camera_model.matrix_view();
	// environment.background_color = gui.brume_color;

	environment.uniform_generic.uniform_int["N_lights"] = 20;
	environment.uniform_generic.uniform_vec3["light_colors[0]"] = gui.light_color;
	environment.uniform_generic.uniform_vec3["light_positions[0]"] = gui.light_position;
	environment.uniform_generic.uniform_float["d_light_max[0]"] = -1.0f;
	environment.uniform_generic.uniform_int["active_lights[0]"] = 1;

	// Uniform variables for Lasers lights
	environment.uniform_generic.uniform_vec3["color"] = xwing_ship.laser.material.color;
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

	sphere_light.model.translation = gui.light_position;
	sphere_light.material.color = gui.light_color * 0.8f;
	sphere_light.material.phong.ambient = 1;
	sphere_light.material.phong.diffuse = 0;
	sphere_light.material.phong.specular = 0;

	// Update scene objects
	numarray<vec3> asteroids_pos;
	numarray<float> asteroids_radius;
	for (int i = 0; i < asteroid_set.positions.size(); i++) {
		if (asteroid_set.destroyed[i] == 0) {
			asteroids_pos.push_back(asteroid_set.positions[i]);
			asteroids_radius.push_back(asteroid_set.colision_radius[i]);
		}
	}

	numarray<vec3> lasers_pos;
	for (int i = 0; i < xwing_ship.lasers_pos.size(); i++)
		if (xwing_ship.lasers_active[i]) lasers_pos.push_back(xwing_ship.lasers_pos[i]);
	xwing_ship.idle_frame(asteroids_pos, asteroids_radius);

	for(int i = 0; i < nb_of_ia_combat; i++){
		victims[i].idle_frame(lasers_pos);
		chads[i].idle_frame(lasers_pos);

		vec3 center = xwing_ship.hierarchy["Vaisseau base"].transform_local.translation;

		AI_ship_check_bounds(victims[i], center);
		AI_ship_check_bounds(chads[i], center);
	}

	if (show_asteroids) asteroid_set.idle_frame(dt, xwing_ship.hierarchy["Vaisseau base"].drawable.model.translation, lasers_pos);

	/**
	passivship.idle_frame();
	passivship.draw(environment);

	xwing_aiship.idle_frame();
	xwing_aiship.draw(environment);
	*/
		

	idle_frame();


	/**
	mesh_drawable tmp3;
	
	tmp3.initialize_data_on_gpu(mesh_primitive_sphere(.01f));
	tmp3.model.translation = reactor_light_pos[3];
	draw(tmp3, environment);

	**/
	
	// conditional display of the global frame (set via the GUI)
	if (gui.display_frame)
		draw(global_frame, environment);

	// the general syntax to display a mesh is:
	//   draw(mesh_drawableName, environment);
	// Note: scene is used to set the uniform parameters associated to the camera, light, etc. to the shader
	//draw(ground, environment);
	draw(cube, environment);	
	draw(sphere, environment);
	draw(camel, environment);
	draw(sphere_light, environment);
	xwing_ship.draw(environment);
	xwing_ship.draw_lasers(environment);

	for(int i = 0; i < nb_of_ia_combat; i++){
		victims[i].draw(environment);
		chads[i].draw(environment);
		chads[i].draw_lasers(environment);
	}

	if (show_asteroids) asteroid_set.draw(environment, gui.display_wireframe);
	if (gui.display_frame) draw(global_frame, environment);
	if (gui.display_ship_arrow) {
		draw(xwing_ship.arrow_up, environment);
		draw(xwing_ship.arrow_velocity, environment);
		draw(xwing_ship.arrow_left, environment);
	}

	if (gui.display_wireframe) {
		//draw_wireframe(ground, environment);
		draw_wireframe(sphere, environment);
		draw_wireframe(cube, environment);
		draw_wireframe(camel, environment);
	}

	environment.background_color = gui.brume_color;

	if (gui.display_ship_arrow) {
		draw(xwing_ship.arrow_up, environment);
		draw(xwing_ship.arrow_velocity, environment);
		draw(xwing_ship.arrow_left, environment);
	}

}

void scene_structure::AI_ship_check_bounds(ship& ship, vec3 center)
{
	vec3 ship_position = ship.hierarchy["Vaisseau base"].transform_local.translation;

	if(norm(ship_position - center) > bound){
		vec3 pos = center - xwing_ship.velocity * 0.5 * bound;
		// apply a normal (gaussian) noise to pos
		pos += vec3(rand_normal(0.0f, 3.f), rand_normal(0.0f, 3.f), rand_normal(0.0f, 3.f));
		ship.hierarchy["Vaisseau base"].transform_local.translation = pos;
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
	if (!inputs.keyboard.shift){
		if(camera_fixe)
			camera_control_fixe.action_mouse_move(environment.camera_view);
		else
			camera_control.action_mouse_move(environment.camera_view);
	}
}
void scene_structure::mouse_click_event()
{
	if(camera_fixe)
		camera_control_fixe.action_mouse_click(environment.camera_view);
	else
		camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	if(camera_fixe)
		camera_control_fixe.action_keyboard(environment.camera_view);
	else
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