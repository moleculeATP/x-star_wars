#pragma once
#include "ship.hpp"


using namespace cgp;

ship::ship(const mesh_drawable& body_init)
{
    // Initialize the ship with a body
    this->body = body_init;
}

void ship::draw(mesh_drawable const& drawable, environment_generic_structure const& environment){
    draw(body, environment);
}