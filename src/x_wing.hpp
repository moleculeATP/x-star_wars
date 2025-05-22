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

    numarray<std::string> canons_name = {"Top right laser", "Bottom left laser", "Top left laser", "Bottom right laser"};

    void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader);
    void idle_frame(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius);
    void draw_lasers(environment_generic_structure const& environment);
};

}