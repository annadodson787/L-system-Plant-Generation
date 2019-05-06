////////////////////////////////////////////////
// You don't need to change this part
////////////////////////////////////////////////
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
// Object space tangent (xyz) and bitangent sign (w)
layout (location=4) in vec4 tangent;


// Values to pass to the fragment shader (interpolated)
// Part 2
// World space vertex position
out vec3 vtx_position;
// World space vertex normal
out vec3 vtx_normal;

// Part 3
// Vertex uv coordinate
out vec2 vtx_uv;

// Part 4
// World space vertex tangent (xyz) and bitangent sign (w)
out vec4 vtx_tangent;

////////////////////////////////////////////////

void main()												
{
	gl_Position = vec4(0, 0, 0, 1);
	////////////////////////////////////////////////////////
	/// Fill in for parts 1, 2, 3 and 4
	/// Process vertex attributes and pass them to the
	/// fragment shader by assigning to the "out" variables
	////////////////////////////////////////////////////////
	
	/// PART 1
	// Fill gl_Position
	
	// PART 2
	// Fill vtx_position and vtx_normal

	// PART 3
	// Fill vtx_uv

	// PART 4
	// Fill vtx_tangent
	
	///////////////////////////////////////////////
}