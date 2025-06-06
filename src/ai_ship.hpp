#pragma once
#include "ship.hpp"

namespace cgp {
struct ai_ship : ship {
    ship* target;
    std::vector<mesh_drawable> body;

    void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader);
    void idle_frame(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius);
    void laser_idle_frame();

    void set_target(ship* target) { this->target = target; }
};
}
