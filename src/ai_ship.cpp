#include "ai_ship.hpp"
#include <iostream> 

namespace cgp {
    void ai_ship::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader){
        ship::initialize(inputs, window, shader, laser_shader);
        float scaling = 0.05f;
        debris.resize(debris_mesh.size());
        
        for (int k = 0; k < body.size(); ++k){
            body[k].model.scaling = scaling;
            std::string name = "Body " + str(k);
            hierarchy.add(body[k], name, "Vaisseau base");
            debris[k] = body[k];

            
            numarray<vec3> pos = debris_mesh[k].position;
            int n = 0;
            vec3 barycenter = {0, 0, 0};
            for(int j = 0; j < pos.size(); ++j){
                barycenter += pos[j];
                n++;
            }
            
            //debris_mesh[k] = debris_mesh[k].centered();
            //debris[k] = mesh_drawable();
            //debris[k].initialize_data_on_gpu(debris_mesh[k]);
            //debris[k].model.scaling = scaling;
            debris[k].model.translation = hierarchy["Vaisseau base"].transform_local.translation - barycenter*scaling;
        
        
            hierarchy[name].drawable.shader = shader;
        }
    }

    void ai_ship::idle_frame(numarray<vec3> const& damaging_pos)
{
    assert_cgp_no_msg(inputs != nullptr);
    assert_cgp_no_msg(window != nullptr);

    float const dt = inputs->time_interval;

    if (destruction) {
        destructed_idle_frame();
        return;
    }

    // Check colisions
    for (int i = 0; i < damaging_pos.size(); i++) {
        float d = norm(damaging_pos[i] - hierarchy["Vaisseau base"].transform_local.translation);
        if (colision_radius > d) {
            destruction_trigger(hierarchy["Vaisseau base"].transform_local.translation + 0.5 * velocity, -velocity);
        }
    }

    if (STOP) return;

    vec3 pos_ai = hierarchy["Vaisseau base"].transform_local.translation;
    vec3 pos_target = target->hierarchy["Vaisseau base"].transform_local.translation;
    vec3 to_target = pos_target - pos_ai;
    float distance = norm(to_target);
    if (distance < 1e-5f) return;

    vec3 dir_to_target = normalize(to_target);

    float ampl = 30;
    float var_max = 0.015f;
    vec3 tmp = ampl * turn_speed * dir_to_target * dt;
    velocity += normalize(tmp) * std::min(var_max, norm(tmp));

    if (norm(velocity) > speed_max)
        velocity = ampl * speed_max * normalize(velocity);

    vec3 new_pos = pos_ai + velocity * speed;
    hierarchy["Vaisseau base"].transform_local.translation = new_pos;

    vec3 forward = normalize(velocity);
    vec3 world_up = {0, 0, 1};
    vec3 new_left = normalize(cross(world_up, forward));
    vec3 new_up = normalize(cross(forward, new_left));

    left = new_left;
    up = new_up;

    if (norm(cross(forward, new_up)) > 1e-3f) {
        hierarchy["Vaisseau base"].transform_local.rotation =
        rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, forward, new_up);
    }

    arrow_velocity.model.translation = new_pos;
    arrow_velocity.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;

    arrow_up.model.translation = new_pos;
    arrow_up.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;

    arrow_left.model.translation = new_pos;
    arrow_left.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;

    laser_idle_frame();
    }


    void ai_ship::laser_idle_frame() {
        // Lasers
        float dt = inputs->time_interval;
        float distance_tir_max = 100;
        float distance_tir_min = 5;
        laser_delay = 0.05f;
        
        float dist = norm(hierarchy["Vaisseau base"].drawable.hierarchy_transform_model.translation - target->hierarchy["Vaisseau base"].drawable.hierarchy_transform_model.translation);
        bool dist_ok = distance_tir_min< dist < distance_tir_max;
        vec3 vect1 = normalize(velocity);
        vec3 vect2 = target->hierarchy["Vaisseau base"].drawable.hierarchy_transform_model.translation - hierarchy["Vaisseau base"].drawable.hierarchy_transform_model.translation;
        // Check if the angle between the ship and the target is small enough to fire
        bool angle_ok = false;
        if(norm(vect2) < 1e-5f)
            angle_ok = true;
        else{
            vect2 = normalize(vect2);
            angle_ok = dot(vect1, vect2) > 0.7f;
        }

        if (dist_ok && angle_ok && laser_dt >= laser_delay) {
            laser_dt = 0;
            if (last_laser == N_lasers - 1) 
                last_laser = 0;
            else last_laser += 1;

            lasers_pos[last_laser] = hierarchy["Vaisseau base"].drawable.hierarchy_transform_model.translation;
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