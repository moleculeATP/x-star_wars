#pragma once


#include "cgp/cgp.hpp"
#include "ship.hpp"
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

	vec3 light_color = { 1,1,1 };
	vec3 brume_color = { 0.5, 0.5, 0. };
	vec3 light_position = { 0, 0, 2 };

	float ambiant = 0.3f;
	float diffus = 0.8f;
	float coef_spec = 0.8f;
	float exp_spec = 20.f;
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
	camera_combat_mode camera_control;
	//camera_controller_orbit camera_control; 
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	timer_basic timer;
	mesh_drawable ground;
	mesh_drawable cube;

	mesh_drawable sphere;
	mesh_drawable camel;

	mesh_drawable sphere_light;
	mesh_drawable xwing;

	ship xwing_ship;
	asteroids asteroid_set;

	skybox_drawable skybox;




	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();
};





