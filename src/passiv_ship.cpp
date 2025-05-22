#include "passiv_ship.hpp"
#include <iostream> 
#include <cstdlib>

namespace cgp{

    void passiv_ship::idle_frame(){
        assert_cgp_no_msg(inputs != nullptr);
        assert_cgp_no_msg(window != nullptr);

        
        if(time_remaining > 0.0f){
            time_remaining -= inputs->time_interval;

            angular_velocity += angular_speed * current_accel_component * inputs->time_interval;

            float angle = norm(angular_velocity) * inputs->time_interval;
            if (angle > 0.0001) {
                vec3 axis = normalize(angular_velocity);
                rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);

                up = normalize(rT * up);
                left = normalize(rT * left);
                velocity = normalize(rT * velocity);

                arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
                arrow_up.model.rotation = rT * arrow_up.model.rotation;
                arrow_left.model.rotation = rT * arrow_left.model.rotation;
            }

            angular_velocity *= amorti_angulaire; 

            hierarchy["Vaisseau base"].transform_local.translation = hierarchy["Vaisseau base"].transform_local.translation + velocity * speed;
            hierarchy["Vaisseau base"].transform_local.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);

            arrow_up.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
            arrow_left.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
            arrow_velocity.model.translation = hierarchy["Vaisseau base"].transform_local.translation;

        } else {

            time_remaining = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/time_max));

            switch(rand() % 10){
                case 0:
                    current_accel_component = -roll_speed * velocity;
                    break;
                case 1:
                    current_accel_component = roll_speed * velocity;
                    break;
                case 2:
                    current_accel_component = -up_speed * left;
                    break;
                case 3:
                    current_accel_component = up_speed * left;
                    break;
                case 4:
                    current_accel_component = -turn_speed * up;
                    break;
                case 5:
                    current_accel_component = turn_speed * up;
                    break;
                default:
                    current_accel_component = {0, 0, 0};
                    break;
            }
        }
    }

    void passiv_ship::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader){
        ship::initialize(inputs, window, shader, laser_shader);
        float scaling = 0.07f;
        debris.resize(body.size());

        
        for (int k = 0; k < body.size(); ++k){
            body[k].model.scaling = scaling;
            std::string name = "Body " + str(k);
            hierarchy.add(body[k], name, "Vaisseau base");
            debris[k] = body[k];
            hierarchy[name].drawable.shader = shader;
        }
    }
}