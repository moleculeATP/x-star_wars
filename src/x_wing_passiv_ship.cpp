#include "x_wing_passiv_ship.hpp"

namespace cgp {

    void x_wing_passiv_ship::initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader){
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

        float wing_angle = 0.22f;
        hierarchy["Top right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
        hierarchy["Top left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);
        hierarchy["Bottom left wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, -wing_angle);
        hierarchy["Bottom right wing"].transform_local.rotation = rotation_transform::from_axis_angle({1, 0, 0}, wing_angle);

    }
}