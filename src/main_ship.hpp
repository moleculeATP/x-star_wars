#pragma once

#include "cgp/cgp.hpp"

using namespace cgp;

struct main_ship:
{
    mesh_drawable body;
    // Position of the ship
    vec3 position = { 0,0,0 };
    // Rotation of the ship
    vec3 rotation = { 0,0,0 };
    // Scale of the ship
    vec3 scale = { 1,1,1 };

    // Speed of the ship
    float speed = 0.1f;

    void initialize_data_on_gpu();
    void draw(environment_structure const& environment);
};
