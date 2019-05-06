#version 330 core

// Camera properties
layout (std140) uniform camera
{
	// View -> Screen transformation matrix
	mat4 projection;
	// World -> View coordinate transformation
	mat4 view;
	// Premultiplied projection * view (for efficiency)
	mat4 pvm;
	// Orthographic projection (not used in this assignment)
	mat4 ortho;
	// Camera world space position
	vec4 position;
};

// Model -> World transformation matrix
uniform mat4 model=mat4(1.0f);

// Available input 
// Object space position
layout (location=0) in vec4 pos;
// Object space normal
layout (location=2) in vec4 normal;
// Vertex texture coordinate
layout (location=3) in vec4 uv;

// Values to pass to the fragment shader (interpolated)
// World space vertex position
out vec3 vtx_position;
// World space vertex normal
out vec3 vtx_normal;
// Vertex uv coordinate
out vec2 vtx_uv;

void main()												
{
	gl_Position=pvm*model*vec4(pos.xyz,1.f);
	vtx_position=vec3(model*vec4(pos.xyz,1.f));
	vtx_normal=vec3(transpose(inverse(model))*vec4(normal.xyz,0.f)).xyz;
	vtx_uv = uv.xy;
}