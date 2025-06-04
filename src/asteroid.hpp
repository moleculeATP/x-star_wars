
#include "cgp/cgp.hpp"
#include "environment.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.1f;
	float frequency_gain = 2.0f;
	int octave = 20;
	float height = 1.0f;
	vec3 color = {1,1,1};
};

namespace cgp {
	struct asteroids {
		int N_asteroids; // Number of asteroids in the scene
		int N_mesh; // Number of differents asteroid models
		int bound = 200; // Max distance allowed
		vec3 center;
		// vec3 color = vec3(88.0f/255.0f, 57.0f/255.0f, 39.0f/255.0f);
		vec3 color = vec3(1, 1, 1);
		float respawn_delay = 20.0f;
		perlin_noise_parameters asteroid_perlin_params;

		// Size N_mesh
		numarray<mesh> meshes;
		numarray<mesh> original_meshes;
		numarray<mesh_drawable> drawables;
		numarray<float> colision_radius;
		numarray<float> original_colision_radius;

		// Size N_asteroids
		numarray<vec3> velocities; 
		numarray<vec3> angular_velocities;
		numarray<vec3> positions;
		numarray<rotation_transform> rotations;
		numarray<int> mesh_ref; // asteroid id -> mesh model in meshes array
		
		int N_destroyed = 0;
		numarray<int> destroyed;
		numarray<float> inactive_time;

		// Smoke billboard
		mesh quad_mesh;
		mesh_drawable smoke;
		int N_quad = 100;
		int N_instances;
		float smoke_radius = 1.0f;
		float dx = 0.005;
		float fade_speed = 1.1f;
		numarray<vec3> instance_positions;
		numarray<vec3> instance_velocities;
		numarray<vec2> instance_alphas;
		numarray<vec3> tmp_positions;
		numarray<vec2> tmp_alphas;

		// Debris
		int N_debris_mesh;
		int N_debris;
		int max_debris = 10;
		numarray<mesh> debris_meshes;
		numarray<mesh> original_debris_meshes;
		numarray<mesh_drawable> debris_drawables;
		numarray<vec2> asteroid2debris_index; // For each asteroid, have a start and end index in the list of debris to avoid looping over unused debris
		numarray<vec3> debris_velocities;
		numarray<vec3> debris_angular_velocities;
		numarray<vec3> debris_positions;
		numarray<rotation_transform> debris_rotations;
		perlin_noise_parameters debris_perlin_params;
		

		void idle_frame(float dt, vec3 next_center, numarray<vec3> const& lasers_position);
		void draw(environment_generic_structure const& environment, bool display_wireframe);
		void apply_perlin(perlin_noise_parameters& asteroid_perlin, perlin_noise_parameters& debris_perlin);
		void initialize(numarray<vec3> const& asteroid_scales, numarray<vec3> const& debris_scales, int Nuv_asteroids, int Nuv_debris, std::string const& texture_path, opengl_shader_structure const& shader);
	};
}
	