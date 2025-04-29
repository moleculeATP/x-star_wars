#pragma once
#include "ship.hpp"


namespace cgp{
ship::ship(const mesh_drawable& body_init)
{
    // Initialize the ship with a body
    body = body_init;
    position = { 0,0,0 };
    velocity = { 0,0,0 };
    rotation = { 0,0,0 };
    speed = 0.1f;

    inputs = nullptr;
    window = nullptr;
    is_cursor_trapped = false;
}

ship::ship()
{
    // Default constructor
    body = mesh_drawable();
    position = { 0,0,0 };
    velocity = { 0,0,0 };
    rotation = { 0,0,0 };
    speed = 0.1f;

    inputs = nullptr;
    window = nullptr;
    is_cursor_trapped = false;
}

void ship::draw(environment_generic_structure const& environment){
    cgp::draw(body, environment);
}

void ship::idle_frame(mat4& camera_matrix_view)
{
    // Update the ship's position and rotation based on its speed and velocity
    // Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);

	float const magnitude = inputs->time_interval;




    position += velocity * speed;
    body.model.translation = position;


}