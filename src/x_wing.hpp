#pragma once
#include "ship.hpp"

namespace cgp {

struct x_wing : ship {
    std::vector<mesh_drawable> body;
    std::vector<mesh_drawable> wing; 
    std::vector<mesh_drawable> gun;
    mesh_drawable reactor;

    float scaling_factor = 0.04f;
    bool deployed = false;
    float wing_min_angle = 0.f;
    float wing_max_angle = 0.22f;
    float wing_angle = 0.f;
    float wing_speed = 0.3f;

    std::vector<float> intensities = {0, 0, 0, 0};
    std::vector<vec3> reactor_light_pos;
    float coef_reactor = 1.f;
    float disp_reactor = .5;

    // Guns
    numarray<std::string> canons_name = {"Top right laser", "Bottom left laser", "Top left laser", "Bottom right laser"};
    numarray<std::string> guns_name = {"Top right gun", "Bottom left gun", "Top left gun", "Bottom right gun"};
    std::vector<bool> guns_trigered = {0, 0, 0, 0}; 
    numarray<float> guns_anim_time = {0, 0, 0, 0};

    void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader, opengl_shader_structure& reactor_shader);
    void idle_frame(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius);
    void laser_idle_frame();
};

}