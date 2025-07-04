#pragma once
#include "x_wing.hpp"
#include "ai_ship.hpp"

namespace cgp {
    struct x_wing_ai_ship : ai_ship {
        std::vector<mesh_drawable> wing;

        void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader);
    };

}