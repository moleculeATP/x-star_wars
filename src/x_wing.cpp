#include "x_wing.hpp"
#include "ship.hpp"
#include <iostream> 

namespace cgp {

    void x_wing::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader, opengl_shader_structure& reactor_shader){
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
        
        float offset_x = .8f;
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
            hierarchy.add(wing_, "Bottom left wing " + str(k), "Bottom left wing", {0, 0.042f, -0.006f});
            hierarchy["Bottom left wing " + str(k)].drawable.shader = shader;

            wing_.model.scaling_xyz = {1, 1, -1};
            hierarchy.add(wing_, "Bottom right wing " + str(k), "Bottom right wing", {0, -0.042f, -0.006f});
            hierarchy["Bottom right wing " + str(k)].drawable.shader = shader;
        }

        for (int k = 0; k < gun.size(); ++k){
            mesh_drawable gun_ = gun[k];
            gun_.model.scaling = scaling;
            hierarchy.add(gun_, "Top right gun", "Top right wing", {0, -0.042f, 0.008f});
            hierarchy["Top right gun"].drawable.shader = shader;

            gun_.model.scaling_xyz = {1, -1, 1};
            hierarchy.add(gun_, "Top left gun", "Top left wing", {0, 0.042f, 0.008f});
            hierarchy["Top left gun"].drawable.shader = shader;

            gun_.model.scaling_xyz = {1, -1, -1};
            hierarchy.add(gun_, "Bottom left gun", "Bottom left wing", {0, 0.042f, -0.006f});
            hierarchy["Bottom left gun"].drawable.shader = shader;

            gun_.model.scaling_xyz = {1, 1, -1};
            hierarchy.add(gun_, "Bottom right gun", "Bottom right wing", {0, -0.042f, -0.006f});
            hierarchy["Bottom right gun"].drawable.shader = shader;
        }
        
        mesh_drawable reactor_ = reactor;
        reactor_.model.scaling = scaling;
        hierarchy.add(reactor_, "Top right reactor", "Top right wing", {0, -0.042f, 0.008f});
        hierarchy["Top right reactor"].drawable.shader = reactor_shader;
        hierarchy["Top right reactor"].drawable.material.color = vec3(.2, .07, .0);

        reactor_.model.scaling_xyz = {1, -1, 1};
        hierarchy.add(reactor_, "Top left reactor", "Top left wing", {0, 0.042f, 0.008f});
        hierarchy["Top left reactor"].drawable.shader = reactor_shader;
        hierarchy["Top left reactor"].drawable.material.color = vec3(.2, .07, .0);

        reactor_.model.scaling_xyz = {1, -1, -1};
        hierarchy.add(reactor_, "Bottom left reactor", "Bottom left wing", {0, 0.042f, -0.006f});
        hierarchy["Bottom left reactor"].drawable.shader = reactor_shader;
        hierarchy["Bottom left reactor"].drawable.material.color = vec3(.2, .07, .0);

        reactor_.model.scaling_xyz = {1, 1, -1};
        hierarchy.add(reactor_, "Bottom right reactor", "Bottom right wing", {0, -0.042f, -0.006f});
        hierarchy["Bottom right reactor"].drawable.shader = reactor_shader;
        hierarchy["Bottom right reactor"].drawable.material.color = vec3(.2, .07, .0);
    
        //mesh_drawable tmp2 = mesh_drawable();
        //tmp2.initialize_data_on_gpu(mesh_primitive_sphere(0.02, {-0.147, -0.072, 0.036}));
        //hierarchy.add(tmp2, "Top right reactor sphere", "Top right wing");

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
        }
        float sup = 1.f;
        if(inputs->keyboard.is_pressed(GLFW_KEY_SPACE)){
            sup = 1.5;
            for(int i=0; i < 4; i++){
                intensities[i] = std::min(2.f, intensities[i] + 2*magnitude*coef_reactor);
            }
        }

        // coloration reacteurs
        else{
            if (inputs->keyboard.is_pressed(GLFW_KEY_Q)){
                intensities[0] = std::min(sup, intensities[0] + magnitude*coef_reactor);
                intensities[3] = std::min(sup, intensities[3] + magnitude*coef_reactor);
            }
                
            if (inputs->keyboard.is_pressed(GLFW_KEY_E)) {
                intensities[2] = std::min(sup, intensities[2] + magnitude*coef_reactor);
                intensities[1] = std::min(sup, intensities[1] + magnitude*coef_reactor);
            }

            if (inputs->keyboard.is_pressed(GLFW_KEY_W)){
                intensities[2] = std::min(sup, intensities[2] + magnitude*coef_reactor);
                intensities[3] = std::min(sup, intensities[3] + magnitude*coef_reactor);
            } 
            if (inputs->keyboard.is_pressed(GLFW_KEY_S)){
                intensities[0] = std::min(sup, intensities[0] + magnitude*coef_reactor);
                intensities[1] = std::min(sup, intensities[1] + magnitude*coef_reactor);
            }

            if (inputs->keyboard.is_pressed(GLFW_KEY_A)) {
                intensities[0] = std::min(sup, intensities[0] + magnitude*coef_reactor);
                intensities[2] = std::min(sup, intensities[2] + magnitude*coef_reactor);
            }
            if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
                intensities[1] = std::min(sup, intensities[1] + magnitude*coef_reactor);
                intensities[3] = std::min(sup, intensities[3] + magnitude*coef_reactor);
            }
        }

        // animation canon
        float k = 15; float s = 0.7;
        for (int i = 0; i < 4; i++) {
            if (guns_trigered[i]) {
                guns_anim_time[i] += magnitude;
                float offset = sin(pow((k * guns_anim_time[i]), s))* 0.05f;
                hierarchy[guns_name[i]].transform_local.translation.x = -offset;
                if(offset < 0){
                    hierarchy[guns_name[i]].transform_local.translation.x = 0;
                    guns_trigered[i] = false;
                }
            }
        }
        

        for(int i = 0; i < 4; i++){
            intensities[i] = std::max(intensities[i] - disp_reactor*magnitude, 0.f);
        }

        // NE PAS DEPLACER 
        // il faut executer cette section apres l actualisation des positions des vaisseaux
        float x_offset = -0.158f;
        float y_offset = -0.028;
        float z_offset = 0.03f;

        std::vector<vec3> reactor_light_pos = {
            hierarchy["Top right reactor"].drawable.hierarchy_transform_model * (vec3(x_offset, y_offset, z_offset)),
            hierarchy["Top left reactor"].drawable.hierarchy_transform_model * (vec3(x_offset, -y_offset, z_offset)),
            hierarchy["Bottom right reactor"].drawable.hierarchy_transform_model * (vec3(x_offset, y_offset, -z_offset)),
            hierarchy["Bottom left reactor"].drawable.hierarchy_transform_model * (vec3(x_offset, -y_offset, -z_offset))
        };

        environment->uniform_generic.uniform_vec3["light_positions_reactor[0]"] = reactor_light_pos[0];
        environment->uniform_generic.uniform_float["intensities[0]"] = intensities[0];

        environment->uniform_generic.uniform_vec3["light_positions_reactor[1]"] = reactor_light_pos[1];
        environment->uniform_generic.uniform_float["intensities[1]"] = intensities[1];

        environment->uniform_generic.uniform_vec3["light_positions_reactor[2]"] = reactor_light_pos[2];
        environment->uniform_generic.uniform_float["intensities[2]"] = intensities[2];

        environment->uniform_generic.uniform_vec3["light_positions_reactor[3]"] = reactor_light_pos[3];
        environment->uniform_generic.uniform_float["intensities[3]"] = intensities[3];

        environment->uniform_generic.uniform_vec3["light_color_reactor"] = vec3(1, .8f, .2f);
        environment->uniform_generic.uniform_float["d_light_max_reactor"] = .2;
        environment->uniform_generic.uniform_int["N_lights_reactor"] = 4;


        environment->uniform_generic.uniform_float["ambiant_reactor"] = 0.2f;
        environment->uniform_generic.uniform_float["diffus_reactor"] = .7f;
        environment->uniform_generic.uniform_float["coef_spec_reactor"] = .4f;
        environment->uniform_generic.uniform_float["coef_exp_reactor"] = 64;

        laser_idle_frame();

        
    }

    void x_wing::laser_idle_frame() {
        // Lasers
        float dt = inputs->time_interval;
        if (inputs->keyboard.is_pressed(GLFW_KEY_P) && laser_dt >= laser_delay) {
            laser_dt = 0;
            if (last_laser == N_lasers - 1) 
                last_laser = 0;
            else last_laser += 1;

            lasers_pos[last_laser] = hierarchy[canons_name[last_laser % 4]].drawable.hierarchy_transform_model.translation;
            lasers_velocity[last_laser] = lasers_speed * normalize(velocity);
            lasers_orientation[last_laser] = hierarchy["Vaisseau base"].transform_local.rotation * rotation_transform::from_axis_angle({0,1,0}, Pi/2.0f);
            lasers_active[last_laser] = 1;

            guns_trigered[last_laser % 4] = true;
            guns_anim_time[last_laser % 4] = 0.0f;
            
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



    
    
    
    