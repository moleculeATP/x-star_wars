#include "ai_ship.hpp"
#include <iostream> 

namespace cgp {
    void ai_ship::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader){
        ship::initialize(inputs, window, shader, laser_shader);
        float scaling = 0.05f;
        debris.resize(body.size());
        
        for (int k = 0; k < body.size(); ++k){
            body[k].model.scaling = scaling;
            std::string name = "Body " + str(k);
            hierarchy.add(body[k], name, "Vaisseau base");
            debris[k] = body[k];
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

    hierarchy["Vaisseau base"].transform_local.rotation =
        rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, forward, new_up);

    arrow_velocity.model.translation = new_pos;
    arrow_velocity.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;

    arrow_up.model.translation = new_pos;
    arrow_up.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;

    arrow_left.model.translation = new_pos;
    arrow_left.model.rotation = hierarchy["Vaisseau base"].transform_local.rotation;
}


}