#version 330 core


// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;

// Settings for texture display
struct texture_settings_structure {
	bool use_texture;       // Switch the use of texture on/off
	bool texture_inverse_v; // Reverse the texture in the v component (1-v)
	bool two_sided;         // Display a two-sided illuminated surface (doesn't work on Mac)
};

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object
	texture_settings_structure texture_settings; // Additional settings for the texture
}; 

uniform material_structure material;

// Texture
uniform sampler2D image_texture;

// Phong
uniform float ambiant_reactor;
uniform float diffus_reactor;
uniform float coef_spec_reactor;
uniform float coef_exp_reactor;

// View matrix
uniform mat4 view;

// Light sources
uniform int N_lights_reactor; // Number of lights
uniform vec3 light_color_reactor;
uniform vec3 light_positions_reactor[4];
uniform float d_light_max_reactor;
uniform float intensities[4];

void main()
{
   // Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;

	// Renormalize normal
	vec3 N = normalize(fragment.normal);

    vec3 end_color = vec3(0, 0, 0);
    for (int i = 0; i < N_lights_reactor; i++) {

        // Light decrease with distance
        float light_intensity = 1.0f;
        float d_lmax = d_light_max_reactor;
        if (d_lmax > 0) {
            float d_light = length(light_positions_reactor[i] - fragment.position);
            float decrease_scale = min(d_light / d_lmax, 1.0f);
            light_intensity = (1.0f - decrease_scale);
        }

        // Phong coefficient (diffuse, specular)
        // *************************************** //

        // Unit direction toward the light
        vec3 L = normalize(light_positions_reactor[i]-fragment.position);

        // Diffuse coefficient
        float diffuse_component = max(dot(N,L),0.0);

        // Specular coefficient
        float specular_component = 0.0;
        if(diffuse_component > 0.0){
            vec3 R = reflect(-L,N); // reflection of light vector relative to the normal.
            vec3 V = normalize(camera_position-fragment.position);
            specular_component = pow( max(dot(R,V),0.0), coef_exp_reactor );
        }
        
        // Compute Shading
        // *************************************** //

        // Compute the base color of the object based on: vertex color, uniform color, texture and light color
        vec3 color_object = material.color;

        // Compute the final shaded color using Phong model
        float Ka = ambiant_reactor;
        float Kd = diffus_reactor;
        float Ks = coef_spec_reactor;
        vec3 color_shading = Ka * color_object + (Ks * specular_component + Kd * diffuse_component) * light_color_reactor * intensities[i];
        
        // Output color, with the alpha component
        end_color += color_shading * light_intensity;
    }
    FragColor = vec4(end_color, 1.0);
}