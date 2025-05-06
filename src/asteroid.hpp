
#include "cgp/cgp.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.35f;
	float frequency_gain = 2.0f;
	int octave = 6;
	float asteroid_height = 0.5f;
};

// Initialize the mesh of the asteroid
cgp::mesh create_asteroid_mesh(float radius, int N);
void update_asteroid(cgp::mesh& asteroid, cgp::mesh_drawable& asteroid_visual, float radius, int N, perlin_noise_parameters const& param);