#include "x_wing.hpp"
#include "ship.hpp"
#include <iostream> // Pour debug avec std::cout si besoin

namespace cgp {

    void x_wing::initialize(input_devices& inputs, window_structure& window){
        ship::initialize(inputs, window);

        // obsolete
        hierarchy.add(body, "Body", "Vaisseau base");
        hierarchy.add(wing, "Top right wing", "Vaisseau base", {0, -0.042f, 0.008f});

        wing.model.scaling_xyz = {1, -1, 1};
        hierarchy.add(wing, "Top left wing", "Vaisseau base", {0, 0.042f, 0.008f});

        wing.model.scaling_xyz = {1, -1, -1};
        hierarchy.add(wing, "Bottom left wing", "Vaisseau base", {0, 0.04f, -0.006f});

        wing.model.scaling_xyz = {1, 1, -1};
        hierarchy.add(wing, "Bottom right wing", "Vaisseau base", {0, -0.04f, -0.006f});
    }
    

    void x_wing::idle_frame() {
        ship::idle_frame();
        float const magnitude = inputs->time_interval;

        if (inputs -> keyboard.is_pressed(GLFW_KEY_SPACE)){
            speed = std::min(speed * speed_increase, speed_max);
            wing_angle = std::max(wing_angle - wing_speed * magnitude, wing_min_angle);
        }
        else{
            speed = std::max(speed / speed_increase, speed_min);
            wing_angle = std::min(wing_angle + wing_speed * magnitude, wing_max_angle);
        }

        hierarchy["Top right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
        hierarchy["Top left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);
        hierarchy["Bottom left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
        hierarchy["Bottom right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);
    }

}

    
    
    
    