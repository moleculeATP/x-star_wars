
#include "cgp/cgp.hpp"
#include "environment.hpp"

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
		int bound = 200; // Max distance allowed
		vec3 center;
		// vec3 color = vec3(88.0f/255.0f, 57.0f/255.0f, 39.0f/255.0f);
		vec3 color = vec3(1, 1, 1);
		float respawn_delay = 100.0f;

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
		numarray<float> colision_radius;
		int N_destroyed = 0;
		numarray<int> destroyed;
		numarray<float> inactive_time;

		// Smoke billboard
		mesh quad_mesh;
		mesh_drawable smoke;
		int N_quad = 100;
		int N_instances;
		float smoke_radius = 1.0f;
		numarray<vec3> instance_positions;
		numarray<vec3> tmp_positions;
		numarray<vec3> tmp_rotations;

		void idle_frame(float dt, vec3 next_center, numarray<vec3> const& lasers_position);
		void draw(environment_generic_structure const& environment, bool display_wireframe);
		void apply_perlin();
		void initialize(numarray<vec3> scales, int N, std::string const& texture_path, opengl_shader_structure const& shader);
	};
}
	