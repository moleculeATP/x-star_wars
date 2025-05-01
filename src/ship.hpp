#pragma once
#include "cgp/cgp.hpp"

namespace cgp {
struct ship {
    mesh_drawable body;
    mesh_drawable arrow_up;
    mesh_drawable arrow_velocity;
    mesh_drawable arrow_left;

    vec3 velocity={1,0,0 };
    vec3 up={0,0,1 };
    vec3 left={0, 1, 0 };

    float up_speed=1.f;
    float roll_speed=1.2f;
    float turn_speed=.8f;
    float speed=.01f;
    float speed_increase=1.1f;
    float speed_max=0.03f;
    float speed_min=0.01f;

    // Pointers to the global state of the inputs (keyboard, mouse, etc)
    input_devices* inputs;
    // Pointer to the global state of the window
    window_structure* window;


    // Constructeur
    ship(const mesh_drawable& body_init);
    ship();

    void initialize(input_devices& inputs, window_structure& window);

    void draw(environment_generic_structure const& environment);

    void idle_frame();

protected:
    bool is_cursor_trapped; // true = cursor captured (/infinite motion), false = cursor free
};
}
