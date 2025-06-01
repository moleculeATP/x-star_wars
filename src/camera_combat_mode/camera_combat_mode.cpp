#include "camera_combat_mode.hpp"

namespace cgp
{

/**
void camera_combat_mode::action_mouse_move(mat4& camera_matrix_view)
{
	// Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);
	if (!is_active) return;

	vec2 const& p1 = inputs->mouse.position.current;
	vec2 const& p0 = inputs->mouse.position.previous;
	vec2 const dp = p1 - p0;

	bool const event_valid = !inputs->mouse.on_gui;
	bool const click_left = inputs->mouse.click.left;

	if (event_valid) 
        camera_model.manipulator_rotate_roll_pitch_yaw(0, -dp.y, dp.x);

	update(camera_matrix_view);
}
	**/


void camera_combat_mode::idle_frame(mat4& camera_matrix_view, ship& ship_to_control)
{
	// Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);


	// Update the camera position and orientation
	vec3 angular_velocity = ship_to_control.angular_velocity;
	vec3 pos;

	if(ship_to_control.is_turning){
		float timer_turning = ship_to_control.timer_turning;
		if(timer_turning >= ship_to_control.steps_times[2]){
			
		}

		mat3 O = transpose(mat3(camera_matrix_view));    
		vec4 tmp = (camera_matrix_view*vec4(0.0, 0.0, 0.0, 1.0));
		vec3 last_col = vec3(tmp[0], tmp[1], tmp[2]); // get the last column
		vec3 camera_position = -O*last_col;

		look_at(camera_position, // Camera position
			ship_to_control.hierarchy["Vaisseau base"].transform_local.translation, // Target position
			ship_to_control.up - ship_to_control.velocity); // could crash
		return;
	}

	float const magnitude = inputs->time_interval;
	float angle = 5 * norm(ship_to_control.angular_velocity) * magnitude;

	if (angle > 0.0001) {
		vec3 axis = normalize(ship_to_control.angular_velocity);
		rotation_transform rT = rotation_transform::from_axis_angle(axis, -angle);
		pos = ship_to_control.hierarchy["Vaisseau base"].transform_local.translation + rT * (-ship_to_control.velocity + ship_to_control.up/3);
	}else{
		pos = ship_to_control.hierarchy["Vaisseau base"].transform_local.translation - ship_to_control.velocity + ship_to_control.up/3;
	}

		
	
	look_at(
        pos, // Camera position
        ship_to_control.hierarchy["Vaisseau base"].transform_local.translation,                           // Target position
        ship_to_control.up                                                // Up vector
    );

	camera_matrix_view = camera_model.matrix_view();

	

	// Preconditions
	/**
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);
	if (!is_active) return;

	float const magnitude = inputs->time_interval;


	if (inputs->keyboard.up || inputs->keyboard.is_pressed(GLFW_KEY_W))
		camera_model.manipulator_rotate_roll_pitch_yaw(0, pitch * magnitude, 0);
	if (inputs->keyboard.down || inputs->keyboard.is_pressed(GLFW_KEY_S))
		camera_model.manipulator_rotate_roll_pitch_yaw(0, -pitch * magnitude, 0);

	if (inputs->keyboard.left || inputs->keyboard.is_pressed(GLFW_KEY_A))
		camera_model.manipulator_rotate_roll_pitch_yaw(roll * magnitude, 0.0f, 0.0f);
	if (inputs->keyboard.right || inputs->keyboard.is_pressed(GLFW_KEY_D))
		camera_model.manipulator_rotate_roll_pitch_yaw(-roll * magnitude, 0.0, 0.0f);

	if (inputs->keyboard.is_pressed(GLFW_KEY_KP_ADD) || inputs->keyboard.is_pressed(GLFW_KEY_R))
		speed = std::min(speed * speed_increase, speed_max);
	if (inputs->keyboard.is_pressed(GLFW_KEY_KP_SUBTRACT) || inputs->keyboard.is_pressed(GLFW_KEY_F))
		speed = std::max(speed / speed_increase, speed_min);

    if (inputs -> keyboard.is_pressed(GLFW_KEY_SPACE))
        speed = std::min(speed * speed_increase, speed_max);
    else
        speed = std::max(speed / speed_increase, speed_min);

	camera_model.manipulator_translate_front(speed * magnitude);

	update(camera_matrix_view);
	**/
	
}
std::string camera_combat_mode::doc_usage() const
{
	std::string doc;
	doc += "PAS DE DOC HOUHOUHOUHOUHOUHOUHOU !!!!";

	return doc;
}

}
