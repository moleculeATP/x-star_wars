
#include "asteroid.hpp"

using namespace cgp;

mesh create_asteroid_mesh(float radius, int N) {
    mesh asteroid = mesh_primitive_sphere(radius, {0,0,0}, N, N);
    return asteroid;
}

void update_asteroid(mesh& asteroid, mesh_drawable& asteroid_visual, float radius, int N, perlin_noise_parameters const& param) {
    // Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			
			// Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

			int const idx = ku*N+kv;

			// Compute the Perlin noise
			float const noise =(1.0f + noise_perlin({u, v}, param.octave, param.persistency, param.frequency_gain)) / 2.0f;

			// use the noise as height value
            // vec3 normal = normalize(asteroid.position[idx]);
            vec3 normal = asteroid.normal[idx];
			asteroid.position[idx] += 0.1 * normal * noise;

			// use also the noise as color value
			asteroid.color[idx] = 0.3f*vec3(0,0.5f,0)+0.7f*vec3(1,1,1);
		}
	}

	// Update the normal of the mesh structure
	asteroid.normal_update();

    asteroid_visual.vbo_position.update(asteroid.position);
	asteroid_visual.vbo_normal.update(asteroid.normal);
	asteroid_visual.vbo_color.update(asteroid.color);
}