#pragma once
#include "ship.hpp"


namespace cgp{


ship::ship()
{

    inputs = nullptr;
    window = nullptr;
    is_cursor_trapped = false;

}

void ship::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader)
{
    this->inputs = &inputs;
    this->window = &window;
    this->shader = &shader;
    this->laser_shader = &laser_shader;

    // Create the arrow pointing in the up direction
    mesh arrow_up_mesh = mesh_primitive_cylinder(0.002f, {0, 0, 0}, up/3);
    arrow_up.initialize_data_on_gpu(arrow_up_mesh);
    arrow_up.material.color = {0, 0, 1};

    // Create the arrow pointing in the velocity direction
    mesh arrow_velocity_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, velocity/2);
    arrow_velocity.initialize_data_on_gpu(arrow_velocity_mesh);
    arrow_velocity.material.color = {1, 0, 0};

    // Create the arrow pointing in the velocity direction
    mesh arrow_left_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, left/2);
    arrow_left.initialize_data_on_gpu(arrow_left_mesh);
    arrow_left.material.color = {0, 1, 0};

    // wings initialization
    // we only need one wing, we will use it 4 times (the one stocked on wing is top right)
    hierarchy = hierarchy_mesh_drawable();
    mesh_drawable  base;
    base.initialize_data_on_gpu(mesh_primitive_sphere(0.001, {0, 0, 0}));
    hierarchy.add(base, "Vaisseau base");
    hierarchy["Vaisseau base"].drawable.shader = shader;

    STOP = false;
    destruction = false;

    
    // Lasers
    mesh laser_mesh = mesh_primitive_cylinder(0.03f, {0,0,0}, {0,0,0.2f}, 10, 10, true);
    laser.initialize_data_on_gpu(laser_mesh);
    laser.shader = laser_shader;
    lasers_pos.resize(N_lasers);
    lasers_velocity.resize(N_lasers);
    lasers_orientation.resize(N_lasers);
    const std::vector<int> tmp = std::vector<int>(N_lasers, 0);
    lasers_active = numarray<int>(tmp);
    
}

void ship::draw(environment_generic_structure const& environment){
    hierarchy.update_local_to_global_coordinates();
    if(destruction)
        for(int k = 0; k < debris.size(); ++k) {
            cgp::draw(debris[k], environment);  
        } 
    else {
        cgp::draw(hierarchy, environment);
        // Lasers
        for (int i = 0; i < lasers_pos.size(); i++) {
            if (lasers_active[i] == 1) {
                laser.model.rotation = lasers_orientation[i];
                laser.model.translation = lasers_pos[i];
                cgp::draw(laser, environment);
            }
        }
    }
}

void ship::destruction_trigger(vec3 impact_pos, vec3 normal_destruction){
    destruction = true;
    STOP = true;
    impact_pos = impact_pos;
    normal_destruction = normalize(normal_destruction);
    this->normal_destruction = normal_destruction;
    directions_destruction = std::vector<vec3> (debris.size());
    angular_velocity_destruction = std::vector<vec3> (debris.size());

    for(int k = 0; k < debris.size(); ++k){
        debris[k].model.translation = hierarchy["Vaisseau base"].transform_local.translation;
        debris[k].model.rotation = arrow_velocity.model.rotation;

        directions_destruction[k] = {rand_normal(normal_destruction.x, 1), rand_normal(normal_destruction.y, 1), rand_normal(normal_destruction.z, 1)};
        angular_velocity_destruction[k] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
    }
}

void ship::respawn(vec3 position, rotation_transform rotation){
    hierarchy["Vaisseau base"].transform_local.translation = position;
    hierarchy["Vaisseau base"].transform_local.rotation = rotation;

    destruction = false;
    STOP = false;
    laser_dt = 0;
    last_laser = 0;
    lasers_active.fill(0);

}

void ship::idle_frame()
{
    // Update the ship's position and rotation based on its speed and velocity
    // Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);

	float const magnitude = inputs->time_interval;

    if(!destruction){
        // flight mod
        vec3 angular_acc = {0, 0, 0};  // valeur temporaire locale

        if (inputs->keyboard.is_pressed(GLFW_KEY_Q)) // roll left
            angular_acc += -roll_speed * velocity; // * magnitude if needed
        if (inputs->keyboard.is_pressed(GLFW_KEY_E)) // roll right
            angular_acc +=  roll_speed * velocity;

        if (inputs->keyboard.is_pressed(GLFW_KEY_W)) // pitch up
            angular_acc += -up_speed * left;
        if (inputs->keyboard.is_pressed(GLFW_KEY_S)) // pitch down
            angular_acc +=  up_speed * left;

        if (inputs->keyboard.is_pressed(GLFW_KEY_A)) // yaw left
            angular_acc +=  turn_speed * up;
        if (inputs->keyboard.is_pressed(GLFW_KEY_D)) // yaw right
            angular_acc += -turn_speed * up;

        if (inputs->keyboard.is_pressed(GLFW_KEY_G))
            STOP = !STOP;

        if(STOP) return;
        angular_velocity += angular_speed * angular_acc * magnitude;
        float angle = norm(angular_velocity) * magnitude;
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

        if (inputs->keyboard.is_pressed(GLFW_KEY_J)){
            destruction_trigger(hierarchy["Vaisseau base"].transform_local.translation + 0.5 * velocity, -velocity);
        }

        // body.model.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);
        hierarchy["Vaisseau base"].transform_local.translation = hierarchy["Vaisseau base"].transform_local.translation + velocity * speed;
        hierarchy["Vaisseau base"].transform_local.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);

        arrow_up.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
        arrow_left.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
        arrow_velocity.model.translation = hierarchy["Vaisseau base"].transform_local.translation;

    }else{
        // destruction mod
        for(int k = 0; k < debris.size(); ++k){
            
            vec3 incd = debris[k].model.translation - impact_pos;
            vec3 dir = normalize(normal_destruction);
            debris[k].model.translation += derive_speed * directions_destruction[k];

            float angle = norm(angular_velocity_destruction[k]) * inputs->time_interval;
		    vec3 axis = normalize(angular_velocity_destruction[k]);
            rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);
		    debris[k].model.rotation *= rT;

            if(inputs -> keyboard.is_pressed(GLFW_KEY_K)){
                respawn(vec3{0, 0, 0}, rotation_transform::from_axis_angle({0, 1, 0}, 0));
            }
        }
    }
}
}