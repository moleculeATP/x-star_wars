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
uniform float ambiant;
uniform float diffus;
uniform float coef_spec;
uniform float coef_exp;

// View matrix
uniform mat4 view;

// Light sources
uniform int N_lights_reactor; // Number of lights
uniform vec3 light_colors_reactor;
uniform vec3 light_positions_reactor[4];
uniform int active_lights_reactor[4];
uniform float d_light_max_reactor;

void main()
{
   // Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;

	// Renormalize normal
	vec3 N = normalize(fragment.normal);

	// Inverse the normal if it is viewed from its back (two-sided surface)
	//  (note: gl_FrontFacing doesn't work on Mac)
	if (material.texture_settings.two_sided && gl_FrontFacing == false) {
		N = -N;
	}

    vec3 end_color = vec3(0, 0, 0);
    for (int i = 0; i < N_lights_reactor; i++) {
        if (active_lights_reactor[i] == 0) continue;

        // Light decrease with distance
        float light_intensity = 1.0f;
        float d_lmax = d_light_max_reactor;
        if (d_lmax > 0) {
            float d_light = length(light_positions_reactor[i] - fragment.position);
            float decrease_scale = min(d_light / d_lmax, 1.0f);
            light_intensity = (1.0f - decrease_scale);
        }
        light_intensity = 1;

        // Phong coefficient (diffuse, specular)
        // *************************************** //

        // Unit direction toward the light
        vec3 L = normalize(light_positions_reactor[i]-fragment.position);

        // Diffuse coefficient
        float diffuse_component = max(dot(N,L),0.0);

        // Texture
        // *************************************** //

        // Current uv coordinates
        vec2 uv_image = vec2(fragment.uv.x, fragment.uv.y);
        if(material.texture_settings.texture_inverse_v) {
            uv_image.y = 1.0-uv_image.y;
        }

        // Get the current texture color
        vec4 color_image_texture = texture(image_texture, uv_image);
        if(material.texture_settings.use_texture == false) {
            color_image_texture=vec4(1.0,1.0,1.0,1.0);
        }
        
        // Compute Shading
        // *************************************** //

        // Compute the base color of the object based on: vertex color, uniform color, texture and light color
        vec3 color_object = fragment.color * material.color * color_image_texture.rgb * light_colors_reactor;

        // Compute the final shaded color using Phong model
        float Ka = ambiant;
        float Kd = diffus;
        vec3 color_shading = (Ka + Kd * diffuse_component) * color_object;
        
        // Output color, with the alpha component
        end_color += color_shading * light_intensity;
    }
    FragColor = vec4(end_color, 1.0);
}