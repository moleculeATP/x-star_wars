#pragma once

#include "cgp/cgp.hpp"
#include "ship.hpp"

namespace cgp {

struct camera_combat_mode : cgp::camera_controller_first_person
{
	float speed = 0.05f; // default speed of the fly mode
	float roll = 0.8f;   // roll magnitude
	float pitch = 0.5f;  //pitch magnitude

	float speed_increase = 1.02f;
	float speed_max = 0.5f;
	float speed_min = 0.0001f;

	// void action_mouse_move(mat4& camera_matrix_view);
	void idle_frame(mat4& camera_matrix_view, ship& ship_to_control);

	std::string doc_usage() const;
};

}
