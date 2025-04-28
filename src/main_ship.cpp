#pragma once

#include "cgp/cgp.hpp"

using namespace cgp;
/**
void main_ship::initialize_data_on_gpu()
{
    // Load the custom shader
    opengl_shader_structure shader_custom;
    shader_custom.load(
        project::path + "shaders/shading_custom/shading_custom.vert.glsl",
        project::path + "shaders/shading_custom/shading_custom.frag.glsl");

    // Affect the loaded shader to the mesh_drawable
    body.shader = shader_custom;

    // Load the mesh from file
    body.initialize_data_on_gpu(mesh_load_file_obj(project::path + "assets/ship.obj"));
}

void main_ship::draw(environment_structure const& environment)
{
    // Set the model matrix
    body.model.translation = position;
    body.model.rotation = rotation_transform::from_euler(rotation);
    body.model.scaling = scale;

    // Draw the ship
    draw(body, environment);
}
    **/