#pragma once
#include "cgp/cgp.hpp"
#include "environment.hpp"

namespace cgp {
struct ship {
    mesh_drawable arrow_up;
    mesh_drawable arrow_velocity;
    mesh_drawable arrow_left;

    hierarchy_mesh_drawable hierarchy;
    std::vector<mesh_drawable> debris;
    std::vector<mesh> debris_mesh; // used to center the debris around their barycenter

    vec3 velocity={1,0,0 };
    vec3 up={0,0,1 };
    vec3 left={0, 1, 0 };
    vec3 angular_velocity = {0, 0, 0};

    vec3 impact_pos = {0, 0, 0};
    vec3 normal_destruction = {-1, 0, 0};
    std::vector<vec3> directions_destruction;
    std::vector<vec3> angular_velocity_destruction;

    bool STOP = false;

    float up_speed=1.f;
    float roll_speed=1.2f;
    float turn_speed=.8f;

    float speed=.03f;
    float speed_increase=1.01f;
    float speed_max=0.1f;
    float speed_min=0.04f;

    float angular_speed=2.f;
    float amorti_angulaire=0.96f;

    bool destruction=false;
    float derive_speed = 0.005f;
    float colision_radius = 0.5f;

    float respawn_timer = 0;

    // Lasers
    mesh_drawable laser;
    int N_lasers = 19;
    int last_laser = 0;
    vec3 lasers_color = {1.0f, 0.1f, 0.88f};
    vec3 laser_material_color = vec3(0.97, 0.78, 0.92);
    float laser_bound = 100.0f;
    float lasers_speed = 70.01f;
    float laser_delay = 0.05f;
    float laser_dt = 0.0f;
    float d_light_max = 10.0f;
    float laser_radius = 0.03f;
    numarray<vec3> lasers_velocity;
    numarray<vec3> lasers_pos;
    numarray<rotation_transform> lasers_orientation;
    numarray<int> lasers_active;


    // complex animation
    bool is_turning = false;
    std::vector<rotation_transform> rotation_turning;
    std::vector<vec3> positions_turning;
    std::vector<vec3> derivative_turning;
    float timer_turning = 0.0f;
    float ampl_turn = 1;
    std::vector<float> steps_times = {0.0f, .8f, 1.6f, 2.6f};

    // Pointers to the global state of the inputs (keyboard, mouse, etc)
    input_devices* inputs;
    // Pointer to the global state of the window
    window_structure* window;

    // Shaders
    opengl_shader_structure* shader;
    opengl_shader_structure* laser_shader;
    environment_structure* environment;

    ship();

    void initialize(input_devices& inputs, window_structure& window, opengl_shader_structure& shader, opengl_shader_structure& laser_shader);

    void draw(environment_generic_structure const& environment);
    void draw_lasers(environment_generic_structure const& environment);

    void idle_frame(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius);
    bool check_colision(numarray<vec3> const& damaging_pos, numarray<float> const& damaging_radius);
	void destructed_idle_frame();

    void destruction_trigger(vec3 impact_pos, vec3 normal_destruction);
    void respawn(vec3 position, rotation_transform rotation);

    void set_mesh_shader(opengl_shader_structure& shader);
    void set_laser_shader(opengl_shader_structure& laser_shader);

protected:
    bool is_cursor_trapped; // true = cursor captured (/infinite motion), false = cursor free
};
}
