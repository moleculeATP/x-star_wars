
#include "asteroid.hpp"

using namespace cgp;

void asteroids::apply_perlin() {
	for (int k = 0; k < N_mesh; k++) {
		for (int i = 0; i < meshes[k].position.size(); i++) {
			float const noise = noise_perlin(meshes[k].position[i], perlin_params[k].octave, perlin_params[k].persistency, perlin_params[k].frequency_gain);
			vec3 normal = meshes[k].normal[i];
			meshes[k].position[i] += normal * noise;
			meshes[k].color[i] = color * noise;
		}
		meshes[k].fill_empty_field();
		meshes[k].normal_update();

		drawables[k].vbo_position.update(meshes[k].position);
		drawables[k].vbo_normal.update(meshes[k].normal);
		drawables[k].vbo_color.update(meshes[k].color);
	}
}

void asteroids::idle_frame(float dt, vec3 next_center, numarray<vec3> const& lasers_position) {
	// numarray<vec3> _smoke_positions;
	// instance_positions.resize(N_destroyed*N_quad);
	tmp_positions.resize_clear(0);
	for (int k = 0; k < N_asteroids; k++) {
		if (destroyed[k] == 1) {
			if (inactive_time[k] >= respawn_delay) {
				positions[k] = center + vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
				destroyed[k] = 0;
				inactive_time[k] = 0;
				--N_destroyed;
			} else {
				inactive_time[k] += dt;
				for (int i = 0; i < N_quad; i++) tmp_positions.push_back(instance_positions[k * N_quad + i]);
			}
		} else {
			// Checking if out of bounds
			vec3 p1 = positions[k];
			vec3 p2 = p1 - velocities[k];

			// Compute intersection of asteroid trajectory with each face of the bounding box
			// The asteroid is moved on ther other side of the box, along its trajectory
			if (abs(p1[0] - center[0]) > bound || abs(p1[1] - center[1]) > bound || abs(p1[2] - center[2]) > bound) {
				float tx = p1[0] - center[0] > 0 ? (center[0] - bound - p1[0]) / p2[0] : (center[0] + bound - p1[0]) / p2[0];
				vec3 px = p1[0] - center[0] > 0 ? vec3(center[0] - bound, p1[1] + p2[1]*tx, p1[2] + p2[2]*tx) : vec3(center[0] + bound, p1[1] + p2[1]*tx, p1[2] + p2[2]*tx);

				float ty = p1[1] - center[1] > 0 ? (center[1] - bound - p1[1]) / p2[1] : (center[1] + bound - p1[1]) / p2[1];
				vec3 py = p1[1] - center[1] > 0 ? vec3(p1[0] + p2[0]*ty, center[1] - bound, p1[2] + p2[2]*ty) : vec3(p1[0] + p2[0]*ty, center[1] + bound, p1[2] + p2[2]*ty);

				float tz = p1[2] - center[2] > 0 ? (center[2] - bound - p1[2]) / p2[2] : (center[2] + bound - p1[2]) / p2[2];
				vec3 pz = p1[2] - center[2] > 0 ? vec3(p1[0] + p2[0]*tz, p1[1] + p2[1]*tz, center[2] - bound) : vec3(p1[0] + p2[0]*tz, p1[1] + p2[1]*tz, center[2] + bound);

				numarray<float> norms = numarray<float>(std::vector<float>{norm(p1 - px), norm(p1 - py), norm(p1 - pz)});
				float min_d = min(norms);
				if (norms[0] == min_d) positions[k] = px;
				else if (norms[1] == min_d) positions[k] = py;
				else positions[k] = pz;
			}

			// Checking colisions with lasers
			bool is_destroyed = false;
			for (int l = 0;  l < lasers_position.size(); l++) {
				float d = norm(positions[k] - lasers_position[l]);
				if (d < colision_radius[k]) {
					destroyed[k] = 1;
					is_destroyed = true;
					break;
				}
			}

			if (is_destroyed) {
				for (int i = 0; i < N_quad; i++) {
					float theta = rand_uniform(0.0f, 2.0f*Pi);
					float phi = rand_uniform(0.0f, 2.0f*Pi);
					vec3 pos = positions[k] + rand_uniform(0.0, smoke_radius) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
					tmp_positions.push_back( pos );
					instance_positions[k * N_quad + i] = pos;
				}
				++N_destroyed;
				continue;
			}

			// Update asteroid
			float angle = norm(angular_velocities[k]) * dt;
			vec3 axis = normalize(angular_velocities[k]);
			rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);

			rotations[k] *= rT;
			positions[k] += (next_center - center) + velocities[k] * dt;
		}
	}
	center = next_center;
	if (tmp_positions.size()>0) smoke.update_supplementary_data_on_gpu(tmp_positions, 4);
}

void asteroids::draw(environment_generic_structure const& environment, bool display_wireframe) {
	for (int i = 0; i < N_asteroids; i++) {
		if (destroyed[i] == 1) continue;
		drawables[mesh_ref[i]].model.translation = positions[i];
		drawables[mesh_ref[i]].model.rotation = rotations[i];
		cgp::draw(drawables[mesh_ref[i]], environment);
		if (display_wireframe) cgp::draw_wireframe(drawables[mesh_ref[i]], environment);
	}
	glEnable(GL_BLEND); // Color Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);
	cgp::draw(smoke, environment, N_destroyed*N_quad);
	glDisable(GL_BLEND);
	glDepthMask(true);
}

void asteroids::initialize(numarray<vec3> scales, int N, std::string const& texture_path, opengl_shader_structure const& shader) {
	meshes.resize(N_mesh);
	drawables.resize(N_mesh);
	perlin_params.resize(N_mesh);

	angular_velocities.resize(N_asteroids);
	velocities.resize(N_asteroids);
	positions.resize(N_asteroids);
	rotations.resize(N_asteroids);
	mesh_ref.resize(N_asteroids);
	colision_radius.resize(N_asteroids);
	destroyed.resize(N_asteroids);
	inactive_time.resize(N_asteroids);

	assert(scales.size() == N_mesh);
	for (int i = 0; i < N_mesh; i++) 
    	meshes[i] = mesh_primitive_ellipsoid(scales[i], {0,0,0}, N, N);
	for (int i = 0; i < N_asteroids; i++) {
		positions[i] = center + vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
		velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		angular_velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		int ind = i % N_mesh;
		mesh_ref[i] = ind;
		colision_radius[i] = (scales[ind][0] + scales[ind][1] + scales[ind][2]) / 3.0f;
		destroyed[i] = 0;
		inactive_time[i] = 0;
	}

	for (int k = 0; k < N_mesh; k++) {
		drawables[k].initialize_data_on_gpu(meshes[k]);
		drawables[k].texture.load_and_initialize_texture_2d_on_gpu(texture_path, GL_REPEAT, GL_REPEAT);
		drawables[k].shader = shader;
	}

	// Smoke
	// quad_mesh = mesh_primitive_quadrangle({ -0.5f,0,0 }, { 0.5f,0,0 }, { 0.5f,0,1 }, { -0.5f,0,1 });
	quad_mesh = mesh_primitive_quadrangle({ -0.5f, -0.5f, 0 }, { 0.5f, -0.5f, 0 }, { 0.5f, 0.5f, 0 }, { -0.5f, 0.5f, 0 });
	// quad_mesh.scale(10.0f);
	smoke.initialize_data_on_gpu(quad_mesh);
	smoke.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/smoke.png");
	smoke.material.phong = { 1,0,0,1 };
	smoke.shader.load(project::path + "shaders/shading_custom/instancing.vert.glsl", project::path + "shaders/shading_custom/instancing.frag.glsl");
	N_instances = N_quad * N_asteroids;
	instance_positions.resize(N_instances);
	//for(int i=0; i < N_instances; ++i) instance_positions[i] = { 0.0f, 0.0f, 0.0f };
	smoke.initialize_supplementary_data_on_gpu(instance_positions, 4, 1);
}