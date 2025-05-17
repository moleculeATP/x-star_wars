#pragma once
#include "ship.hpp"

namespace cgp {
struct ai_ship : ship {
    ship* target;

    void idle_frame();
    
};
}
