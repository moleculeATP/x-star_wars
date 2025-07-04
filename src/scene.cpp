#include "scene.hpp"

using namespace cgp;

bool show_asteroids = true;
int nb_of_ia_combat = 2; // 2 AI ship fighting each other
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

	// Sphere used to display the po sition of a light
	sphere_light.initialize_data_on_gpu(mesh_primitive_sphere(0.2f));

	// Asteroids
	if (show_asteroids) {
		int Nuv_asteroids = 70;
		int Nuv_debris = 20;
		asteroid_set.N_mesh = 10;
		asteroid_set.N_asteroids = 100;
		asteroid_set.bound = 200;
		asteroid_set.N_debris_mesh = 10;
		std::vector<vec3> asteroid_scales;
		std::vector<vec3> debris_scales;
		for (int i = 0; i < asteroid_set.N_mesh; i++) 
			asteroid_scales.push_back(2.0f * vec3(rand_uniform(0.1, 1.0), rand_uniform(0.1, 0.5), rand_uniform(0.1, 0.8)));
		for (int i = 0; i < asteroid_set.N_debris_mesh; i++)
			debris_scales.push_back(0.1f * vec3(rand_uniform(0.1, 1.0), rand_uniform(0.1, 0.5), rand_uniform(0.1, 0.8)));
		asteroid_set.initialize(asteroid_scales, debris_scales, Nuv_asteroids, Nuv_debris, project::path + "assets/asteroid1.jpg", shader_custom);
		perlin_noise_parameters asteroid_perlin;
		perlin_noise_parameters debris_perlin = {gui.debris_persistency, gui.debris_frequency_gain, gui.debris_octave, gui.debris_height};
		asteroid_set.apply_perlin(asteroid_perlin, debris_perlin);
	}

	// Remove warnings for unset uniforms
	cgp_warning::max_warning = 0;
	
	auto struct_body = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__body.obj");
	auto struct_wing = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__wing.obj");
	auto struct_body_2 = mesh_load_file_obj_advanced(project::path + "assets/tie_model/", "tie.obj");
	auto struct_reactor = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "reactor.obj");
	auto struct_gun = mesh_load_file_obj_advanced(project::path + "assets/x_wing_model/", "x-wing2__gun.obj");
	xwing_ship.wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	xwing_ship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	xwing_ship.gun = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_gun);
	xwing_ship.reactor.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/x_wing_model/reactor.obj"));
	
	std::vector<mesh_drawable> x_wing_body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body);
	std::vector<mesh_drawable> x_wing_wing = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_wing);
	std::vector<mesh_drawable> tie_body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body_2);
	std::vector<mesh> mesh_tie;

	xwing_ship.initialize(inputs, window, shader_custom, laser_shader, reactor_shader);

	aiship.initialize(inputs, window, shader_custom, laser_shader);
	aiship.set_target(&xwing_ship);
	aiship.body = mesh_obj_advanced_loader::convert_to_mesh_drawable(struct_body_2);
	aiship.respawn(vec3(-10, 0, 0), rotation_axis_angle(vec3(1, 0, 0), 0));
	
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

	t = 0;
			
}

void scene_structure::display_frame()
{
	// Update time
	float dt = timer.update();
	t += dt;
	
	sphere_light.material.color = gui.light_color * 0.8f;
	sphere_light.material.phong.ambient = 1;
	sphere_light.material.phong.diffuse = 0;
	sphere_light.material.phong.specular = 0;

	// ------ Update scene objects ------

	// Storing objects that can destroy the x_wing, other ships or asteroids
	numarray<vec3> xwing_damaging_pos;
	numarray<float> xwing_damaging_radius;
	numarray<vec3> enemies_damaging_pos;
	numarray<float> enemies_damaging_radius;
	numarray<vec3> allies_damaging_pos;
	numarray<float> allies_damaging_radius;
	numarray<vec3> asteroids_damaging_pos;
	numarray<float> asteroids_damaging_radius;

	// Immunity for the first seconds
	if (t > 3) {
		// Storing asteroids positions
		for (int i = 0; i < asteroid_set.positions.size(); i++) {
			if (asteroid_set.destroyed[i] == 0) {
				xwing_damaging_pos.push_back(asteroid_set.positions[i]);
				xwing_damaging_radius.push_back(asteroid_set.colision_radius[asteroid_set.mesh_ref[i]]);
			}
		}

		// Storing xwing lasers positions
		for (int i = 0; i < xwing_ship.lasers_pos.size(); i++) {
			if (xwing_ship.lasers_active[i]) {
				enemies_damaging_pos.push_back(xwing_ship.lasers_pos[i]);
				enemies_damaging_radius.push_back(xwing_ship.laser_radius);
				asteroids_damaging_pos.push_back(xwing_ship.lasers_pos[i]);
				asteroids_damaging_radius.push_back(xwing_ship.laser_radius);
			}
		}

		// Storing lasers positions of shooters
		for (int i = 0; i < nb_of_ia_combat; i++) {
			for (int j = 0; j < chads[i].lasers_pos.size(); j++) {
				if (chads[i].lasers_active[j]) {
					if (i % 2 == 0) { // Shooter is a T-fighter bot
						xwing_damaging_pos.push_back(chads[i].lasers_pos[j]);
						xwing_damaging_radius.push_back(chads[i].laser_radius);
						allies_damaging_pos.push_back(chads[i].lasers_pos[j]);
						allies_damaging_radius.push_back(chads[i].laser_radius);
					} else { // Shooter is a X_wing bot
						enemies_damaging_pos.push_back(chads[i].lasers_pos[j]);
						enemies_damaging_radius.push_back(chads[i].laser_radius);
					}
				}
			}
		}
	}

	xwing_ship.idle_frame(xwing_damaging_pos, xwing_damaging_radius);

	vec3 center = xwing_ship.hierarchy["Vaisseau base"].transform_local.translation;
	for(int i = 0; i < nb_of_ia_combat; i++){
		if (i % 2 == 0) {
			victims[i].idle_frame(allies_damaging_pos, allies_damaging_radius);
			chads[i].idle_frame(enemies_damaging_pos, enemies_damaging_radius);
		} else {
			victims[i].idle_frame(enemies_damaging_pos, enemies_damaging_radius);
			chads[i].idle_frame(allies_damaging_pos, allies_damaging_radius);
		}

		AI_ship_check_bounds(victims[i], center);
		AI_ship_check_bounds(chads[i], center);
		rotation_transform rT0_ = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, {1, 0, 0}, {0, 0, 1});

		if(victims[i].destruction and victims[i].respawn_timer < 0){
			victims[i].respawn(vec3(0, 0, 0), rT0_);
		}else{
			victims[i].respawn_timer -= dt;
		}
		
		if(chads[i].destruction and chads[i].respawn_timer < 0){
			chads[i].respawn(vec3(0, 0, 0), rT0_);
		}else{
			chads[i].respawn_timer -= dt;
		}
	}

	if (show_asteroids) 
		asteroid_set.idle_frame(dt, xwing_ship.hierarchy["Vaisseau base"].drawable.model.translation, asteroids_damaging_pos, asteroids_damaging_radius);

	idle_frame();

	// Set additional uniform parameters to the shader
	environment.uniform_generic.uniform_float["ambiant"] = gui.ambiant;
	environment.uniform_generic.uniform_float["diffus"] = gui.diffus;
	environment.uniform_generic.uniform_float["coef_spec"] = gui.coef_spec;
	environment.uniform_generic.uniform_float["coef_exp"] = gui.exp_spec;
	environment.uniform_generic.uniform_vec3["light_color"] = gui.light_color;
	environment.uniform_generic.uniform_float["distance_xwing"] = 1.0f;
	environment.uniform_generic.uniform_vec3["camera_pos"] = camera_control.camera_model.position();
	environment.uniform_generic.uniform_mat4["view"] = camera_control.camera_model.matrix_view();

	environment.uniform_generic.uniform_int["N_lights"] = 20;
	environment.uniform_generic.uniform_vec3["light_positions[0]"] = sphere_light.model.translation;
	environment.uniform_generic.uniform_vec3["light_colors[0]"] = gui.light_color;
	environment.uniform_generic.uniform_float["d_light_max[0]"] = -1.0f;
	environment.uniform_generic.uniform_int["active_lights[0]"] = 1;

	// Uniform variables for Lasers lights
	environment.uniform_generic.uniform_vec3["color"] = xwing_ship.laser.material.color;
	for (int i = 1; i < 20; i ++) {
		std::string uniform_color = "light_colors[" + std::to_string(i) + "]";
		std::string uniform_pos = "light_positions[" + std::to_string(i) + "]";
		std::string uniform_dl_max = "d_light_max[" + std::to_string(i) + "]";
		std::string uniform_active_light = "active_lights[" + std::to_string(i) + "]";
		if (xwing_ship.lasers_active[i-1] == 0) {
			environment.uniform_generic.uniform_int[uniform_active_light] = 0;
		} else {
			environment.uniform_generic.uniform_vec3[uniform_color] = xwing_ship.lasers_color;
			environment.uniform_generic.uniform_vec3[uniform_pos] = xwing_ship.lasers_pos[i-1];
			environment.uniform_generic.uniform_float[uniform_dl_max] = xwing_ship.d_light_max;
			environment.uniform_generic.uniform_int[uniform_active_light] = 1;
		}
	}

	std::vector<vec3> reactor_light_pos = xwing_ship.reactor_light_pos;
	std::vector<float> intensities = xwing_ship.intensities;
	environment.uniform_generic.uniform_vec3["light_positions_reactor[0]"] = reactor_light_pos[0];
	environment.uniform_generic.uniform_float["intensities[0]"] = intensities[0];

	environment.uniform_generic.uniform_vec3["light_positions_reactor[1]"] = reactor_light_pos[1];
	environment.uniform_generic.uniform_float["intensities[1]"] = intensities[1];

	environment.uniform_generic.uniform_vec3["light_positions_reactor[2]"] = reactor_light_pos[2];
	environment.uniform_generic.uniform_float["intensities[2]"] = intensities[2];

	environment.uniform_generic.uniform_vec3["light_positions_reactor[3]"] = reactor_light_pos[3];
	environment.uniform_generic.uniform_float["intensities[3]"] = intensities[3];

	environment.uniform_generic.uniform_vec3["light_color_reactor"] = vec3(1, .8f, .2f);
	environment.uniform_generic.uniform_float["d_light_max_reactor"] = .2;
	environment.uniform_generic.uniform_int["N_lights_reactor"] = 4;


	environment.uniform_generic.uniform_float["ambiant_reactor"] = 0.2f;
	environment.uniform_generic.uniform_float["diffus_reactor"] = .7f;
	environment.uniform_generic.uniform_float["coef_spec_reactor"] = .4f;
	environment.uniform_generic.uniform_float["coef_exp_reactor"] = 64;

	if(SKYBOX){
		// Skybox
		//  Must be called before drawing the other shapes and without writing in the Depth Buffer
		glDepthMask(GL_FALSE); // disable depth-buffer writing
		draw(skybox, environment);
		glDepthMask(GL_TRUE);  // re-activate depth-buffer write
	}

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

	if (ImGui::CollapsingHeader("Light")) {
		ImGui::ColorEdit3("Light color", &gui.light_color[0]);
		ImGui::SliderFloat("Ambiant", &gui.ambiant, 0.0f, 1.0f);
		ImGui::SliderFloat("Diffus", &gui.diffus, 0.0f, 1.0f);
		ImGui::SliderFloat("Spéculaire", &gui.coef_spec, 0.0f, 1.0f);
		ImGui::SliderFloat("Spéculaire_exp", &gui.exp_spec, 0.0f, 256.f);
	}

	if (ImGui::CollapsingHeader("Perlin params")) {
		bool update = false;
		ImGui::Spacing();
		ImGui::Text("Asteroids");
		update |= ImGui::ColorEdit3("Asteroids color", &gui.asteroids_color[0]);
		update |= ImGui::SliderFloat("Asteroids persistency", &gui.asteroids_persistency, 0.01f, 1.0f);
		update |= ImGui::SliderFloat("Asteroids frequency gain", &gui.asteroids_frequency_gain, 0.01f, 10.0f);
		update |= ImGui::SliderInt("Asteroids octave", &gui.asteroids_octave, 1, 100);
		update |= ImGui::SliderFloat("Asteroids height", &gui.asteroids_height, 0.01f, 10.0f);
		ImGui::Spacing();
		ImGui::Text("Debris");
		update |= ImGui::SliderFloat("Debris persistency", &gui.debris_persistency, 0.01f, 1.0f);
		update |= ImGui::SliderFloat("Debris frequency gain", &gui.debris_frequency_gain, 0.01f, 10.0f);
		update |= ImGui::SliderInt("Debris octave", &gui.debris_octave, 1, 100);
		update |= ImGui::SliderFloat("Debris height", &gui.debris_height, 0.01f, 10.0f);

		if (update) {
			perlin_noise_parameters asteroid_perlin = {gui.asteroids_persistency, gui.asteroids_frequency_gain, gui.asteroids_octave, gui.asteroids_height, gui.asteroids_color};
			perlin_noise_parameters debris_perlin = {gui.debris_persistency, gui.debris_frequency_gain, gui.debris_octave, gui.debris_height, gui.asteroids_color};
			asteroid_set.apply_perlin(asteroid_perlin, debris_perlin);
		}
	}
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
	std::cout << "Welcome to X-Star-Wars." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}