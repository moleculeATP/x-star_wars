#pragma once
#include "cgp/cgp.hpp"

using namespace cgp;

struct ship {
    mesh_drawable body;
    vec3 position = { 0,0,0 };
    vec3 velocity = { 0,0,0 };
    vec3 rotation = { 0,0,0 };
    float speed = 0.1f;

    // Constructeur
    ship(const mesh_drawable& body_init);
};