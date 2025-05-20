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

// View matrix
uniform mat4 view;

uniform vec3 center;
uniform vec4 source_color = vec4(1.0);
uniform vec4 glow_color = vec4(0.0, 1.0, 0.08, 0.84);
uniform float laser_width = 0.03f;
uniform float laser_length = 0.2f;
uniform float source_intensity = 5.0f;
uniform float glow_fade = 0.1;
uniform float min_alpha = 0.5;

void main()
{
    vec2 uv = fragment.uv;
    // float dist_from_center = length(uv - vec2(0.5, 0.5));
    // float length_factor = smoothstep(laser_length, laser_length - 0.1, abs(fragment.position.z / 5.0));
    // float length_factor = 1;
    // float core = smoothstep(laser_width, 0.0, dist_from_center) * length_factor;
    //vec3 core_result = source_color.rgb * core * source_intensity;
    // // float glow = smoothstep(glow_fade, 0.0, dist_from_center) * (1.0 - core);
    // // vec3 glow_result = glow_color.rgb * glow;

    // vec3 final_color = core_result; // t + glow_result * glow_color.a;
    // final_color = clamp(final_color, 0.0, 1.0);
    float alpha = min_alpha; //max(glow + core, min_alpha);
    FragColor = vec4(vec3(1,1,1), alpha);
}