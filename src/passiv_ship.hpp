#pragma once
#include "ship.hpp"

namespace cgp {
struct passiv_ship : ship {
    float time_remaining = 0.0f;
    float time_max = 2.0f;
    vec3 current_accel_component = {0, 0, 0};
    std::vector<mesh_drawable> body;

    void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader);

    void idle_frame(numarray<vec3> const& damaging_pos);
};
}