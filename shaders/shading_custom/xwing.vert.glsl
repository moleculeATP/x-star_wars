#version 330 core

// Inputs venant des VBOs
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_color;
layout (location = 3) in vec2 vertex_uv;

// Variables de sortie vers le fragment shader
out struct fragment_data
{
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} fragment;

// Uniforms reçus du programme C++
uniform mat4 projection;
uniform mat4 view;
uniform vec3 camera_pos;
uniform float distance_xwing;

void main()
{
    // Extraction des axes de la caméra depuis la matrice view
    vec3 forward = normalize(-view[2].xyz); // Axe -Z (direction de la caméra)
    vec3 right   = normalize(view[0].xyz);  // Axe X (gauche/droite)
    vec3 up      = normalize(view[1].xyz);  // Axe Y (haut/bas)

    // Appliquer la rotation de la caméra au modèle
    vec3 rotated_position = vertex_position.x * right 
                          + vertex_position.y * up 
                          + vertex_position.z * forward;

    // Nouvelle position du modèle devant la caméra
    vec3 world_position = camera_pos + forward * distance_xwing + rotated_position;

    // Transformation de la normale pour suivre la rotation de la caméra
    vec3 rotated_normal = vertex_normal.x * right 
                         + vertex_normal.y * up 
                         + vertex_normal.z * forward;

    // Projection dans l'espace écran
    gl_Position = projection * view * vec4(world_position, 1.0);

    // Remplissage des variables pour le fragment shader
    fragment.position = world_position;
    fragment.normal   = rotated_normal;
    fragment.color    = vertex_color;
    fragment.uv       = vertex_uv;
}