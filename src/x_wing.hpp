#pragma once
#include "ship.hpp"

namespace cgp {

struct x_wing : ship {
    std::vector<mesh_drawable> body;
    std::vector<mesh_drawable> wing; 

    bool deployed = false;
    float wing_min_angle = 0.f;
    float wing_max_angle = 0.22f;
    float wing_angle = 0.f;
    float wing_speed = 0.3f;

    void respawn();
    void initialize(input_devices& inputs, window_structure& window);
    void idle_frame();
};

}