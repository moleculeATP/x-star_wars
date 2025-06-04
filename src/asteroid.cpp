
#include "asteroid.hpp"

using namespace cgp;

void asteroids::apply_perlin(perlin_noise_parameters& asteroid_perlin, perlin_noise_parameters& debris_perlin) {
	asteroid_perlin_params = asteroid_perlin;
	debris_perlin_params = debris_perlin;
	
	// On asteroids
	for (int k = 0; k < N_mesh; k++) {
		float volume_rate = 0.0f;
		for (int i = 0; i < meshes[k].position.size(); i++) {
			// float const noise = noise_perlin(meshes[k].position[i], asteroid_perlin_params.octave, asteroid_perlin_params.persistency, asteroid_perlin_params.frequency_gain);
			float const noise = asteroid_perlin_params.height * noise_perlin(original_meshes[k].position[i], asteroid_perlin_params.octave, asteroid_perlin_params.persistency, asteroid_perlin_params.frequency_gain);
			// vec3 normal = meshes[k].normal[i];
			vec3 normal = original_meshes[k].normal[i];
			// meshes[k].position[i] += normal * noise;
			meshes[k].position[i] = original_meshes[k].position[i] + normal * noise;
			volume_rate += norm(meshes[k].position[i]) / norm(original_meshes[k].position[i]);
			// meshes[k].color[i] = color * noise;
			meshes[k].color[i] = asteroid_perlin_params.color;
		}
		meshes[k].fill_empty_field();
		meshes[k].normal_update();

		drawables[k].vbo_position.update(meshes[k].position);
		drawables[k].vbo_normal.update(meshes[k].normal);
		drawables[k].vbo_color.update(meshes[k].color);
		
		// Ajusting collision radius
		volume_rate /= meshes[k].position.size();
		colision_radius[k] = original_colision_radius[k] * volume_rate;
	}

	// On debris
	for (int k = 0; k < N_debris_mesh; k++) {
		for (int i = 0; i < debris_meshes[k].position.size(); i++) {
			float const noise = debris_perlin_params.height * noise_perlin(original_debris_meshes[k].position[i], debris_perlin_params.octave, debris_perlin_params.persistency, debris_perlin_params.frequency_gain);
			vec3 normal = original_debris_meshes[k].normal[i];
			debris_meshes[k].position[i] = original_debris_meshes[k].position[i] + normal * noise;
			debris_meshes[k].color[i] = debris_perlin_params.color;
		}

		debris_meshes[k].fill_empty_field();
		debris_meshes[k].normal_update();

		debris_drawables[k].vbo_position.update(debris_meshes[k].position);
		debris_drawables[k].vbo_normal.update(debris_meshes[k].normal);
		debris_drawables[k].vbo_color.update(debris_meshes[k].color);
	}
}

void asteroids::idle_frame(float dt, vec3 next_center, numarray<vec3> const& lasers_position) {
	tmp_positions.resize_clear(0);
	tmp_alphas.resize_clear(0);
	for (int k = 0; k < N_asteroids; k++) {
		if (destroyed[k] == 1) {
			if (inactive_time[k] >= respawn_delay) {
				positions[k] = center + vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
				destroyed[k] = 0;
				inactive_time[k] = 0;
				--N_destroyed;
			} else {
				inactive_time[k] += dt;
				// Adding smoke to buffer
				for (int i = 0; i < N_quad; i++) {
					instance_positions[k * N_quad + i] = instance_positions[k * N_quad + i] + dx * instance_velocities[k * N_quad + i];
					tmp_positions.push_back(instance_positions[k * N_quad + i]);
					tmp_alphas.push_back(vec2(1.0f - fade_speed * inactive_time[k] / respawn_delay, 0.0));
				}
				// Updating debris
				vec2 debris_ids = asteroid2debris_index[k];
				for (int i = debris_ids[0]; i < debris_ids[1]; i++) {
					float angle = norm(debris_angular_velocities[i]) * dt;
					vec3 axis = normalize(debris_angular_velocities[i]);
					rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);
					debris_rotations[i] *= rT;
					debris_positions[i] += debris_velocities[i] * dt;
				}
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
				if (d < colision_radius[mesh_ref[k]]) {
					destroyed[k] = 1;
					is_destroyed = true;
					break;
				}
			}

			if (is_destroyed) {
				// Setting smoke quads
				for (int i = 0; i < N_quad; i++) {
					float theta = rand_uniform(0.0f, 2.0f*Pi);
					float phi = rand_uniform(0.0f, 2.0f*Pi);
					vec3 pos = positions[k] + rand_uniform(0.1, smoke_radius) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
					tmp_positions.push_back( pos );
					instance_positions[k * N_quad + i] = pos;
					instance_velocities[k * N_quad + i] = normalize(pos - positions[k]);
					tmp_alphas.push_back(vec2(1.0f, 0.0f));
				}
				// Setting debris parameters
				vec2 debris_ids = asteroid2debris_index[k];
				for (int i = debris_ids[0]; i < debris_ids[1]; i++) {
					float theta = rand_uniform(0.0f, 2.0f*Pi);
					float phi = rand_uniform(0.0f, 2.0f*Pi);
					vec3 pos = positions[k] + rand_uniform(0.1, smoke_radius) * vec3(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
					debris_positions[i] = pos;
					debris_velocities[i] = normalize(pos - positions[k]) * rand_uniform(0.1f, 5.0f);
					debris_angular_velocities[i] = 5.0f * vec3(rand_uniform(-1, 1), rand_uniform(-1, 1), rand_uniform(-1, 1));
				}
				++N_destroyed;
				continue;
			}

			// Update asteroid
			float angle = norm(angular_velocities[k]) * dt;
			vec3 axis = normalize(angular_velocities[k]);
			rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);

			rotations[k] *= rT;
			// positions[k] += (next_center - center) + velocities[k] * dt;
			positions[k] += velocities[k] * dt;
		}
	}
	center = next_center;
	if (tmp_positions.size()>0) smoke.update_supplementary_data_on_gpu(tmp_positions, 4);
	if (tmp_alphas.size()>0) smoke.update_supplementary_data_on_gpu(tmp_alphas, 5);
}

void asteroids::draw(environment_generic_structure const& environment, bool display_wireframe) {
	for (int i = 0; i < N_asteroids; i++) {
		if (destroyed[i] == 1) {
			vec2 debris_ids = asteroid2debris_index[i];
			for (int k = debris_ids[0]; k < debris_ids[1]; k++) {
				int ind = k % N_debris_mesh;
				debris_drawables[ind].model.translation = debris_positions[k];
				debris_drawables[ind].model.rotation = debris_rotations[k];
				cgp::draw(debris_drawables[ind], environment);
				if (display_wireframe) cgp::draw_wireframe(debris_drawables[ind], environment);
			}
		} else {
			drawables[mesh_ref[i]].model.translation = positions[i];
			drawables[mesh_ref[i]].model.rotation = rotations[i];
			cgp::draw(drawables[mesh_ref[i]], environment);
			if (display_wireframe) cgp::draw_wireframe(drawables[mesh_ref[i]], environment);
		}
	}
	// Drawing smoke last
	glEnable(GL_BLEND); // Color Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(false);
	cgp::draw(smoke, environment, N_destroyed*N_quad);
	glDisable(GL_BLEND);
	glDepthMask(true);
}

void asteroids::initialize(numarray<vec3> const& asteroid_scales, numarray<vec3> const& debris_scales, int Nuv_asteroids, int Nuv_debris, std::string const& texture_path, opengl_shader_structure const& shader) {
	meshes.resize(N_mesh);
	original_meshes.resize(N_mesh);
	drawables.resize(N_mesh);
	colision_radius.resize(N_mesh);
	original_colision_radius.resize(N_mesh);

	angular_velocities.resize(N_asteroids);
	velocities.resize(N_asteroids);
	positions.resize(N_asteroids);
	rotations.resize(N_asteroids);
	mesh_ref.resize(N_asteroids);
	destroyed.resize(N_asteroids);
	inactive_time.resize(N_asteroids);
	asteroid2debris_index.resize(N_asteroids);

	debris_meshes.resize(N_debris_mesh);
	original_debris_meshes.resize(N_debris_mesh);
	debris_drawables.resize(N_debris_mesh);

	assert(asteroid_scales.size() == N_mesh);
	assert(debris_scales.size() == N_debris_mesh);

	// Set a definitive number of debris per asteroid
	N_debris = 0;
	for (int i = 0; i < N_asteroids; i++) {
		int n = rand_uniform(2, max_debris);
		asteroid2debris_index[i] = vec2(N_debris, N_debris+n);
		N_debris += n;
	}
	debris_positions.resize(N_debris);
	debris_velocities.resize(N_debris);
	debris_angular_velocities.resize(N_debris);
	debris_rotations.resize(N_debris);

	for (int i = 0; i < N_mesh; i++) {
    	meshes[i] = mesh_primitive_ellipsoid(asteroid_scales[i], {0,0,0}, Nuv_asteroids, Nuv_asteroids);
		original_meshes[i] = mesh_primitive_ellipsoid(asteroid_scales[i], {0,0,0}, Nuv_asteroids, Nuv_asteroids);
		colision_radius[i] = (asteroid_scales[i][0] + asteroid_scales[i][1] + asteroid_scales[i][2]) / 3.0f;
		original_colision_radius[i] = (asteroid_scales[i][0] + asteroid_scales[i][1] + asteroid_scales[i][2]) / 3.0f;
	}
	for (int i = 0; i < N_debris_mesh; i++) {
		debris_meshes[i] = mesh_primitive_ellipsoid(debris_scales[i], {0,0,0}, Nuv_debris, Nuv_debris);
		original_debris_meshes[i] = mesh_primitive_ellipsoid(debris_scales[i], {0,0,0}, Nuv_debris, Nuv_debris);
	}
	for (int i = 0; i < N_asteroids; i++) {
		positions[i] = center + vec3(rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2);
		velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		angular_velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		int ind = i % N_mesh;
		mesh_ref[i] = ind;
		destroyed[i] = 0;
		inactive_time[i] = 0;
	}

	for (int k = 0; k < N_mesh; k++) {
		drawables[k].initialize_data_on_gpu(meshes[k]);
		drawables[k].texture.load_and_initialize_texture_2d_on_gpu(texture_path, GL_REPEAT, GL_REPEAT);
		drawables[k].shader = shader;
	}

	for (int k = 0; k < N_debris_mesh; k++) {
		debris_drawables[k].initialize_data_on_gpu(debris_meshes[k]);
		debris_drawables[k].texture.load_and_initialize_texture_2d_on_gpu(texture_path, GL_REPEAT, GL_REPEAT);
		debris_drawables[k].shader = shader;
	}

	// Smoke
	quad_mesh = mesh_primitive_quadrangle({ -0.5f, -0.5f, 0 }, { 0.5f, -0.5f, 0 }, { 0.5f, 0.5f, 0 }, { -0.5f, 0.5f, 0 });
	smoke.initialize_data_on_gpu(quad_mesh);
	smoke.texture.load_and_initialize_texture_2d_on_gpu(project::path + "assets/smoke.png");
	smoke.material.phong = { 1,0,0,1 };
	smoke.shader.load(project::path + "shaders/shading_custom/instancing.vert.glsl", project::path + "shaders/shading_custom/instancing.frag.glsl");
	N_instances = N_quad * N_asteroids;
	instance_positions.resize(N_instances);
	instance_velocities.resize(N_instances);
	instance_alphas.resize(N_instances);
	smoke.initialize_supplementary_data_on_gpu(instance_positions, 4, 1);
	smoke.initialize_supplementary_data_on_gpu(instance_alphas, 5, 1);
}