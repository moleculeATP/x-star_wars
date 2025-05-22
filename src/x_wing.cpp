#include "x_wing.hpp"
#include "ship.hpp"
#include <iostream> 

namespace cgp {

    void x_wing::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader){
        ship::initialize(inputs, window, shader, laser_shader);
        float scaling = 0.04f;
        debris.resize(body.size() + 2 * wing.size());

        
        for (int k = 0; k < body.size(); ++k){
            body[k].model.scaling = scaling;
            std::string name = "Body " + str(k);
            hierarchy.add(body[k], name, "Vaisseau base");
            debris[k] = body[k];
            hierarchy[name].drawable.shader = shader;
        }

        mesh_drawable tmp = mesh_drawable();
        tmp.initialize_data_on_gpu(mesh_primitive_triangle());
        tmp.model.scaling = 0.0001f;

        hierarchy.add(tmp, "Top right wing", "Vaisseau base");
        hierarchy.add(tmp, "Top left wing", "Vaisseau base");
        hierarchy.add(tmp, "Bottom left wing", "Vaisseau base");
        hierarchy.add(tmp, "Bottom right wing", "Vaisseau base");
        
        float offset_x = 1.2f;
        float offset_y = 0.95f;
        float top_offset_z = -0.3f;
        float bottom_offset_z = -0.4f;

        hierarchy.add(tmp, "Top right laser", "Top right wing", {offset_x, -offset_y, top_offset_z});
        hierarchy.add(tmp, "Top left laser", "Top left wing", {offset_x, offset_y, top_offset_z});
        hierarchy.add(tmp, "Bottom right laser", "Bottom right wing", {offset_x, -offset_y, bottom_offset_z});
        hierarchy.add(tmp, "Bottom left laser", "Bottom left wing", {offset_x, offset_y, bottom_offset_z});

        for (int k = 0; k < wing.size(); ++k){
            mesh_drawable wing_ = wing[k];
            wing_.model.scaling = scaling;

            debris[body.size() + k] = wing_;
            hierarchy.add(wing_, "Top right wing " + str(k), "Top right wing", {0, -0.042f, 0.008f});
            hierarchy["Top right wing " + str(k)].drawable.shader = shader;

            wing_.model.scaling_xyz = {1, -1, 1};
            hierarchy.add(wing_, "Top left wing " + str(k), "Top left wing", {0, 0.042f, 0.008f});
            hierarchy["Top left wing " + str(k)].drawable.shader = shader;

            wing_.model.scaling_xyz = {1, -1, -1};
            debris[body.size() + wing.size() + k] = wing_;
            hierarchy.add(wing_, "Bottom left wing " + str(k), "Bottom left wing", {0, 0.04f, -0.006f});
            hierarchy["Bottom left wing " + str(k)].drawable.shader = shader;

            wing_.model.scaling_xyz = {1, 1, -1};
            hierarchy.add(wing_, "Bottom right wing " + str(k), "Bottom right wing", {0, -0.04f, -0.006f});
            hierarchy["Bottom right wing " + str(k)].drawable.shader = shader;
        }
        

        /**
        debris = std::vector<mesh_drawable> (10);
        for(int i = 0; i < 10; i++){
            debris[i].initialize_data_on_gpu(mesh_primitive_sphere(0.02, {0, 0, 0}));
        }
        */
    }    

    void x_wing::idle_frame(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius) {
        float const magnitude = inputs->time_interval;
        ship::idle_frame(damaging_pos, damaging_radius);
        if(!destruction){
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
            
            // Lasers
            float dt = magnitude;
            if (inputs->keyboard.is_pressed(GLFW_KEY_P) && laser_dt >= laser_delay) {
                laser_dt = 0;
                if (last_laser == N_lasers - 1) 
                    last_laser = 0;
                else last_laser += 1;

                lasers_pos[last_laser] = hierarchy[canons_name[last_laser % 4]].drawable.hierarchy_transform_model.translation;
                lasers_velocity[last_laser] = lasers_speed * normalize(velocity);
                lasers_orientation[last_laser] = hierarchy["Vaisseau base"].transform_local.rotation * rotation_transform::from_axis_angle({0,1,0}, Pi/2.0f);
                lasers_active[last_laser] = 1;
                
            } else laser_dt += dt;

            for (int i = 0; i < N_lasers; i++) {
                if (lasers_active[i] == 0) continue;
                vec3 velocity = lasers_velocity[i];
                lasers_pos[i] += velocity * dt;
                vec3 center = hierarchy["Vaisseau base"].transform_local.translation;
                if (norm(center - lasers_pos[i]) > laser_bound)
                    lasers_active[i] = 0;
            }
        }

        
    }

    void x_wing::draw_lasers(environment_generic_structure const& environment) {
        // Lasers
        for (int i = 0; i < lasers_pos.size(); i++) {
            if (lasers_active[i] == 1) {
                laser.model.rotation = lasers_orientation[i];
                laser.model.translation = lasers_pos[i];
                // laser.material.color = x_wing::lasers_color;
                cgp::draw(laser, environment);
            }
        }
    }
}



    
    
    
    