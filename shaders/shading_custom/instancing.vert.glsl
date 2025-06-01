#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)
layout (location = 4) in vec3 instance_position;  // instance position  (x,y,z)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
} fragment;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera

// float theta(vec2 a, vec2 b) {
// 	float d = dot(a, b);
// 	if (d >= 0) return -acos(d / (length(a) * length(b)));
// 	else return acos(d / (length(a) * length(b)));
// }	

// mat3 rotation_z(in float angle) {
// 	return mat3(vec3(cos(angle), -sin(angle), 0.0),
// 		vec3(sin(angle), cos(angle), 0.0),
// 		vec3(0.0, 0.0, 1.0));
// }


void main()
{
	mat3 O = transpose(mat3(view));                   
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); 
	vec3 camera_position = -O*last_col;

	vec3 CameraRight_worldspace = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 CameraUp_worldspace = vec3(view[0][1], view[1][1], view[2][1]);
	vec4 position = vec4(instance_position + CameraRight_worldspace * vertex_position.x + CameraUp_worldspace * vertex_position.y, 1.0);

	// The normal of the vertex in the world space
	// mat4 modelNormal = transpose(inverse(model));
	// vec4 normal = modelNormal * vec4(vertex_normal, 0.0);
	vec4 normal = vec4(normalize(camera_position - instance_position), 0.0);

	vec4 position_projected = projection * view * position;

	fragment.position = position.xyz;//new_position.xyz;
	fragment.normal   = normal.xyz;
	fragment.color = vertex_color;
	fragment.uv = vertex_uv;
	
	gl_Position = position_projected;

}
