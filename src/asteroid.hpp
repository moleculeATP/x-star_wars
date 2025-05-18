
#include "cgp/cgp.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.1f;
	float frequency_gain = 2.0f;
	int octave = 20;
	float asteroid_height = 0.5f;
};

namespace cgp {
	struct asteroids {
		int N_asteroids; // Number of asteroids in the scene
		int N_mesh; // Number of differents asteroid models
		int bound = 50; // Max distance allowed
		vec3 center;
		// vec3 color = vec3(88.0f/255.0f, 57.0f/255.0f, 39.0f/255.0f);
		vec3 color = vec3(1, 1, 1);

		// Size N_mesh
		numarray<mesh> meshes; 
		numarray<mesh_drawable> drawables;
		numarray<perlin_noise_parameters> perlin_params;

		// Size N_asteroids
		numarray<vec3> velocities; 
		numarray<vec3> angular_velocities;
		numarray<vec3> positions;
		numarray<rotation_transform> rotations;
		numarray<int> mesh_ref; // asteroid id -> mesh model in meshes array
		
		void idle_frame(float dt, vec3 next_center);
		void draw(environment_generic_structure const& environment, bool display_wireframe);
		void apply_perlin();
		void initialize(numarray<vec3> scales, int N, std::string const& texture_path, opengl_shader_structure const& shader);
	};
}
	