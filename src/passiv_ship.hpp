#pragma once
#include "ship.hpp"

namespace cgp {
struct passiv_ship : ship {
    float time_remaining = 0.0f;
    float time_max = 2.0f;
    vec3 current_accel_component = {0, 0, 0};

    void idle_frame();
};
}