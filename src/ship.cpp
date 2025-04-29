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
        rotation_transform rT = rotation_transform::from_axis_angle({1,0,0}, -magnitude*roll_speed);
        body.model.rotation = rT * body.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_E)){
        rotation_transform rT = rotation_transform::from_axis_angle({1,0,0}, magnitude*roll_speed);
        body.model.rotation = rT * body.model.rotation;
    }

    // up
    if (inputs->keyboard.is_pressed(GLFW_KEY_W))
    {
        rotation_transform rT = rotation_transform::from_axis_angle({0,1,0}, -magnitude*up_speed);
        body.model.rotation = rT * body.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_S)){
        rotation_transform rT = rotation_transform::from_axis_angle({0,1,0}, magnitude*up_speed);
        body.model.rotation = rT * body.model.rotation;
    }

    // turn
    if (inputs->keyboard.is_pressed(GLFW_KEY_A))
    {
        rotation_transform rT = rotation_transform::from_axis_angle({0,0,1}, magnitude*turn_speed);
        body.model.rotation = rT * body.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
        rotation_transform rT = rotation_transform::from_axis_angle({0,0,1}, -magnitude*turn_speed);
        body.model.rotation = rT * body.model.rotation;
    }

    body.model.translation = body.model.translation + velocity * speed;


}
}