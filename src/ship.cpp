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
}

ship::ship()
{
    // Default constructor
    body = mesh_drawable();
    position = { 0,0,0 };
    velocity = { 0,0,0 };
    rotation = { 0,0,0 };
    speed = 0.1f;
}

void ship::draw(environment_generic_structure const& environment){
    cgp::draw(body, environment);
}
}