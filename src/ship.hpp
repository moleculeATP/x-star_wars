#pragma once
#include "cgp/cgp.hpp"


namespace cgp{
struct ship {
    mesh_drawable body;;
    vec3 position;
    vec3 velocity;
    vec3 rotation;
    float speed;

    // Pointers to the global state of the inputs (keyboard, mouse, etc)
    input_devices* inputs;
    // Pointer to the global state of the window
    window_structure* window;


    // Constructeur
    ship(const mesh_drawable& body_init);
    ship();

    void draw(environment_generic_structure const& environment);

    void idle_frame(mat4& camera_matrix_view);

protected:
    bool is_cursor_trapped; // true = cursor captured (/infinite motion), false = cursor free
};
}