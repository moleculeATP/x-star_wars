#pragma once
#include "ship.hpp"


using namespace cgp;

ship::ship(const mesh_drawable& body_init)
{
    // Initialize the ship with a body
    this->body = body_init;
}