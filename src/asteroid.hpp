
#include "cgp/cgp.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.35f;
	float frequency_gain = 2.0f;
	int octave = 6;
	float asteroid_height = 0.5f;
};

namespace cgp {
	struct asteroids {
		int N_asteroids;
		int bound = 50;
		numarray<vec3> scales;
		numarray<vec3> velocities;
		numarray<vec3> angular_velocities;
		numarray<mesh> meshes;
		numarray<mesh_drawable> drawables;
		perlin_noise_parameters param;
		numarray<vec3> positions;
	
		void idle_frame(float dt);
		void update_asteroids();
		void create_meshes(cgp::numarray<cgp::vec3> scales, int N);
		void initialize();
	};
}
	