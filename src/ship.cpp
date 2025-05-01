#pragma once
#include "ship.hpp"


namespace cgp{
ship::ship(const mesh_drawable& body_init)
{
    // Initialize the ship with a body
    ship();
    body = body_init;
}

ship::ship()
{
    // Default constructor
    body = mesh_drawable();

    inputs = nullptr;
    window = nullptr;
    is_cursor_trapped = false;
}

void ship::initialize(input_devices& inputs, window_structure& window)
{
    this->inputs = &inputs;
    this->window = &window;

    // Create the arrow pointing in the up direction
    mesh arrow_up_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, up/3);
    arrow_up.initialize_data_on_gpu(arrow_up_mesh);
    arrow_up.material.color = {0, 0, 1};

    // Create the arrow pointing in the velocity direction
    mesh arrow_velocity_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, velocity/2);
    arrow_velocity.initialize_data_on_gpu(arrow_velocity_mesh);
    arrow_velocity.material.color = {1, 0, 0};

    // Create the arrow pointing in the velocity direction
    mesh arrow_left_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, left/2);
    arrow_left.initialize_data_on_gpu(arrow_left_mesh);
    arrow_left.material.color = {0, 1, 0};
}

void ship::draw(environment_generic_structure const& environment){
    cgp::draw(body, environment);
}

void ship::idle_frame()
{
    // Update the ship's position and rotation based on its speed and velocity
    // Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);

	float const magnitude = inputs->time_interval;
    // roll
    if (inputs->keyboard.is_pressed(GLFW_KEY_Q))
    {
        // apply a rotation around the x-axis to the up vector
        rotation_transform rT = rotation_transform::from_axis_angle(velocity, -magnitude*roll_speed);
        up = normalize(rT * up);
        left = normalize(rT * left);
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_E)){
        rotation_transform rT = rotation_transform::from_axis_angle(velocity, magnitude*roll_speed);
        up = normalize(rT * up);
        left = normalize(rT * left);
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }

    // up
    if (inputs->keyboard.is_pressed(GLFW_KEY_W))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(left, -magnitude*up_speed);
        up = normalize(rT * up);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_S))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(left, magnitude*up_speed);
        up = normalize(rT * up);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
    }

    // turn
    if (inputs->keyboard.is_pressed(GLFW_KEY_A))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(up, magnitude*turn_speed);
        body.model.rotation = rT * body.model.rotation;
        left = normalize(rT * left);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
        rotation_transform rT = rotation_transform::from_axis_angle(up, -magnitude*turn_speed);
        body.model.rotation = rT * body.model.rotation;
        left = normalize(rT * left);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }

    // translation to velocity vector
    body.model.translation = body.model.translation + velocity * speed;

    // make the velocity and up vector orthogonal (changing the up vector if necessary)
    vec3 left = normalize(cross(up, velocity));
    vec3 up = normalize(cross(velocity, left));  // Recalculer up pour assurer l'orthogonalité

    
    
    body.model.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);


}
}