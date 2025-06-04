#pragma once

#include "cgp/cgp.hpp"

#include "ship.hpp"
#include "x_wing.hpp"
#include "ai_ship.hpp"
#include "passiv_ship.hpp"
#include "x_wing_ai_ship.hpp"
#include "x_wing_passiv_ship.hpp"

#include "environment.hpp"
#include "camera_combat_mode/camera_combat_mode.hpp"
#include "asteroid.hpp"

// This definitions allow to use the structures: mesh, mesh_drawable, etc. without mentionning explicitly cgp::
using cgp::mesh;
using cgp::mesh_drawable;
using cgp::vec3;
using cgp::numarray;
using cgp::timer_basic;

// Variables associated to the GUI (buttons, etc)
struct gui_parameters {
	bool display_frame = false;
	bool display_wireframe = false;	
	bool display_ship_arrow = false;
	// bool display_ground = false;

	vec3 light_color = { 1,1,1 };

	float ambiant = 0.3f;
	float diffus = 0.8f;
	float coef_spec = 0.8f;
	float exp_spec = 20.f;

	float debris_persistency = 0.1f;
	float debris_frequency_gain = 2.0f;
	int debris_octave = 20;
	float debris_height = 1.0f;

	float asteroids_persistency = 0.1f;
	float asteroids_frequency_gain = 2.0f;
	int asteroids_octave = 20;
	float asteroids_height = 1.0f;
	vec3 asteroids_color = vec3(1, 1, 1);
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	//camera_controller_orbit_euler camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage

	// COMBAT MODE
	bool camera_fixe = false; // bugged
	camera_combat_mode camera_control;
	camera_controller_orbit camera_control_fixe; 
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;
	//mesh_drawable ground;
	mesh_drawable cube;

	mesh_drawable sphere;
	mesh_drawable camel;

	mesh_drawable sphere_light;
	mesh_drawable xwing;
	std::vector<mesh_drawable> shapes;
	mesh_drawable laser_shot;

	x_wing xwing_ship;
	ai_ship aiship;
	passiv_ship passivship;
	x_wing_passiv_ship xwing_passivship;
	x_wing_ai_ship xwing_aiship;
	asteroids asteroid_set;

	std::vector<passiv_ship> victims;
	std::vector<ai_ship> chads;
	float bound = 30.0f;

	skybox_drawable skybox;

	// Shaders
	opengl_shader_structure shader_mesh;
	opengl_shader_structure shader_custom;
	opengl_shader_structure laser_shader;
	opengl_shader_structure reactor_shader;

	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop

	void AI_ship_check_bounds(ship& ship, vec3 center);

	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();
};





