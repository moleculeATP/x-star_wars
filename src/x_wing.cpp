#include "x_wing.hpp"
#include "ship.hpp"
#include <iostream> 

namespace cgp {

    void x_wing::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader){
        ship::initialize(inputs, window, shader);
        float scaling = 0.04f;
        debris.resize(body.size() + 2 * wing.size());

        for (int k = 0; k < body.size(); ++k){
            body[k].model.scaling = scaling;
            std::string name = "Body " + str(k);
            hierarchy.add(body[k], name, "Vaisseau base");
            debris[k] = body[k];
            hierarchy[name].drawable.shader = shader;
        }

        hierarchy.add(mesh_drawable(), "Top right wing", "Vaisseau base");
        hierarchy.add(mesh_drawable(), "Top left wing", "Vaisseau base");
        hierarchy.add(mesh_drawable(), "Bottom left wing", "Vaisseau base");
        hierarchy.add(mesh_drawable(), "Bottom right wing", "Vaisseau base");

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

    void x_wing::respawn() {
        x_wing::initialize(*inputs, *window, *shader);
    }
    

    void x_wing::idle_frame() {
        float const magnitude = inputs->time_interval;
        ship::idle_frame();
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
        }else{
            if(inputs -> keyboard.is_pressed(GLFW_KEY_K)){
                respawn();
            }
        }

        // Lasers
        float dt = magnitude;
        if (inputs->keyboard.is_pressed(GLFW_KEY_P) && laser_dt >= laser_delay) {
            laser_dt = 0;
            if (last_laser == N_lasers - 1) 
                last_laser = 0;
            else last_laser += 1;
            lasers_pos[last_laser] = hierarchy["Vaisseau base"].transform_local.translation + 2.0f * normalize(velocity);
            lasers_velocity[last_laser] = lasers_speed * (lasers_pos[last_laser] - hierarchy["Vaisseau base"].transform_local.translation);
            lasers_orientation[last_laser] = hierarchy["Vaisseau base"].transform_local.rotation * rotation_transform::from_axis_angle({0,1,0}, Pi/2.0f);
            lasers_active[last_laser] = 1;
        }else {
            laser_dt += dt;
        }

        for (int i = 0; i < N_lasers; i++) {
            if (lasers_active[i] == 0) continue;
            vec3 velocity = lasers_velocity[i];
            lasers_pos[i] += velocity * dt;
            if (abs(lasers_pos[i][0]) > laser_bound || abs(lasers_pos[i][1]) > laser_bound || abs(lasers_pos[i][2]) > laser_bound)
                lasers_active[i] = 0;
        }
    }

    // void x_wing::set_shader(opengl_shader_structure &shader) {
    //     for (mesh_drawable md: body) md.shader = shader;
    //     for (mesh_drawable md: wing) md.shader = shader;
    // }

}

    
    
    
    