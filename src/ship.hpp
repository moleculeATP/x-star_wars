#pragma once
#include "cgp/cgp.hpp"


namespace cgp{
struct ship {
    mesh_drawable body;;
    vec3 position;
    vec3 velocity;
    vec3 rotation;
    float speed;

    // Constructeur
    ship(const mesh_drawable& body_init);
    ship();

    void draw(environment_generic_structure const& environment);
};
}