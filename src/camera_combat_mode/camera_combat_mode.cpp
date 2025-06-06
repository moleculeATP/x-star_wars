#include "camera_combat_mode.hpp"

namespace cgp
{

void camera_combat_mode::idle_frame(mat4& camera_matrix_view, ship& ship_to_control)
{
	// Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);


	// Update the camera position and orientation
	vec3 pos;

	if(ship_to_control.is_turning){
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
        ship_to_control.hierarchy["Vaisseau base"].transform_local.translation, // Target position
        ship_to_control.up // Up vector
    );

	camera_matrix_view = camera_model.matrix_view();
}
std::string camera_combat_mode::doc_usage() const
{
	std::string doc;
	doc += "Camera doc";
	return doc;
}

}
