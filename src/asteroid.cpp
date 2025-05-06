
#include "asteroid.hpp"

using namespace cgp;

void asteroids::create_meshes(numarray<vec3> scales, int N) {
	meshes.resize(N_asteroids);
	drawables.resize(N_asteroids);
	angular_velocities.resize(N_asteroids);
	velocities.resize(N_asteroids);
	positions.resize(N_asteroids);
	scales = scales;
	for (int i = 0; i < N_asteroids; i++) {
    	meshes[i] = mesh_primitive_ellipsoid(scales[i], {0,0,0}, N, N);
		positions[i] = {rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2, rand_uniform(-bound, bound)/2};
		velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		angular_velocities[i] = {rand_uniform(-5, 5), rand_uniform(-5, 5), rand_uniform(-5, 5)};
		drawables[i].model.translation = positions[i];
	}
}


void asteroids::update_asteroids() {
	for (int k = 0; k < N_asteroids; k++) {
		for (int i = 0; i < meshes[k].position.size(); i++) {
			float const noise = noise_perlin(meshes[k].position[i], param.octave, param.persistency, param.frequency_gain);
			vec3 normal = meshes[k].normal[i];
			meshes[k].position[i] += normal * noise;
			meshes[k].color[i] = vec3(88.0f/255.0f, 57.0f/255.0f, 39.0f/255.0f)*noise;
		}
		meshes[k].fill_empty_field();
		meshes[k].normal_update();

		drawables[k].vbo_position.update(meshes[k].position);
		drawables[k].vbo_normal.update(meshes[k].normal);
		drawables[k].vbo_color.update(meshes[k].color);
	}
}

void asteroids::idle_frame(float dt) {
	for (int k = 0; k < N_asteroids; k++) {
		vec3 velocity = velocities[k];
		vec3 angular_velocity = angular_velocities[k];

		vec3 translat = drawables[k].model.translation;
		if (abs(translat[0]) > bound) drawables[k].model.translation = {-translat[0], translat[1], translat[2]};
		if (abs(translat[1]) > bound) drawables[k].model.translation = {translat[0], -translat[1], translat[2]};
		if (abs(translat[2]) > bound) drawables[k].model.translation = {translat[0], translat[1], -translat[2]};
		

		float angle = norm(angular_velocity) * dt;
		vec3 axis = normalize(angular_velocity);
        rotation_transform rT = rotation_transform::from_axis_angle(axis, angle);

		drawables[k].model.rotation *= rT;
		drawables[k].model.translation += velocity * dt;
	}
}

void asteroids::initialize() {
	for (int k = 0; k < N_asteroids; k++) 
		drawables[k].initialize_data_on_gpu(meshes[k]);
}