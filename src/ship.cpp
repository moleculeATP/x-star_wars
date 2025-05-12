#pragma once
#include "ship.hpp"


namespace cgp{
ship::ship(const mesh_drawable& body_init)
{
    // Initialize the ship with a body
    ship();
    body = body_init;
}

ship::ship()
{
    // Default constructor
    body = mesh_drawable();

    inputs = nullptr;
    window = nullptr;
    is_cursor_trapped = false;

}

void ship::initialize(input_devices& inputs, window_structure& window)
{
    this->inputs = &inputs;
    this->window = &window;

    // Create the arrow pointing in the up direction
    mesh arrow_up_mesh = mesh_primitive_cylinder(0.005f, {0, 0, 0}, up/3);
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
    hierarchy.add(body, "Vaisseau base");
    hierarchy.add(wing, "Top right wing", "Vaisseau base", {0, -0.042f, 0.008f});

    wing.model.scaling_xyz = {1, -1, 1};
    hierarchy.add(wing, "Top left wing", "Vaisseau base", {0, 0.042f, 0.008f});

    wing.model.scaling_xyz = {1, -1, -1};
    hierarchy.add(wing, "Bottom left wing", "Vaisseau base", {0, 0.04f, -0.006f});

    wing.model.scaling_xyz = {1, 1, -1};
    hierarchy.add(wing, "Bottom right wing", "Vaisseau base", {0, -0.04f, -0.006f});

    if(STOP) {
        speed = 0;
        speed_min = 0;
    }

}

void ship::draw(environment_generic_structure const& environment){
    hierarchy.update_local_to_global_coordinates();
    cgp::draw(hierarchy, environment);
    // cgp::draw(body, environment);
}

void ship::idle_frame()
{
    // Update the ship's position and rotation based on its speed and velocity
    // Preconditions
	assert_cgp_no_msg(inputs != nullptr);
	assert_cgp_no_msg(window != nullptr);

	float const magnitude = inputs->time_interval;
    vec3 angular_acc = {0, 0, 0};  // valeur temporaire locale

    if (inputs->keyboard.is_pressed(GLFW_KEY_Q)) // roll left
        angular_acc += -roll_speed * velocity;
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

    if (inputs->keyboard.is_pressed(GLFW_KEY_G)) // yaw right
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
        body.model.rotation = rT * body.model.rotation;
    }

    angular_velocity *= amorti_angulaire; // par ex. 0.98






    /**
    // roll
    if (inputs->keyboard.is_pressed(GLFW_KEY_Q))
    {
        // apply a rotation around the x-axis to the up vector
        rotation_transform rT = rotation_transform::from_axis_angle(velocity, -magnitude*roll_speed);
        up = normalize(rT * up);
        left = normalize(rT * left);
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_E)){
        rotation_transform rT = rotation_transform::from_axis_angle(velocity, magnitude*roll_speed);
        up = normalize(rT * up);
        left = normalize(rT * left);
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }

    // up
    if (inputs->keyboard.is_pressed(GLFW_KEY_W))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(left, -magnitude*up_speed);
        up = normalize(rT * up);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_S))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(left, magnitude*up_speed);
        up = normalize(rT * up);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_up.model.rotation = rT * arrow_up.model.rotation;
    }

    // turn
    if (inputs->keyboard.is_pressed(GLFW_KEY_A))
    {
        rotation_transform rT = rotation_transform::from_axis_angle(up, magnitude*turn_speed);
        // body.model.rotation = rT * body.model.rotation;
        hierarchy["Vaisseau base"].transform_local.rotation = rT * hierarchy["Vaisseau base"].transform_local.rotation;
        left = normalize(rT * left);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }
    if (inputs->keyboard.is_pressed(GLFW_KEY_D)){
        rotation_transform rT = rotation_transform::from_axis_angle(up, -magnitude*turn_speed);
        // body.model.rotation = rT * body.model.rotation;
        hierarchy["Vaisseau base"].transform_local.rotation = rT * hierarchy["Vaisseau base"].transform_local.rotation;
        left = normalize(rT * left);
        velocity = normalize(rT * velocity);
        arrow_velocity.model.rotation = rT * arrow_velocity.model.rotation;
        arrow_left.model.rotation = rT * arrow_left.model.rotation;
    }
    

    // translation to velocity vector
    // body.model.translation = body.model.translation + velocity * speed;
    

    // make the velocity and up vector orthogonal (changing the up vector if necessary)
    vec3 left = normalize(cross(up, velocity));
    vec3 up = normalize(cross(velocity, left));  // Recalculer up pour assurer l'orthogonalité
    **/

    if (inputs -> keyboard.is_pressed(GLFW_KEY_SPACE)){
        speed = std::min(speed * speed_increase, speed_max);
        wing_angle = std::max(wing_angle - wing_speed * magnitude, wing_min_angle);
    }
    else{
        speed = std::max(speed / speed_increase, speed_min);
        wing_angle = std::min(wing_angle + wing_speed * magnitude, wing_max_angle);
    }
    
    
    
    // body.model.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);
    hierarchy["Vaisseau base"].transform_local.translation = hierarchy["Vaisseau base"].transform_local.translation + velocity * speed;
    hierarchy["Vaisseau base"].transform_local.rotation = rotation_transform::from_frame_transform({1,0,0}, {0,0,1}, velocity, up);

    hierarchy["Top right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
    hierarchy["Top left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);
    hierarchy["Bottom left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
    hierarchy["Bottom right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);

    arrow_up.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
    arrow_left.model.translation = hierarchy["Vaisseau base"].transform_local.translation;
}
}