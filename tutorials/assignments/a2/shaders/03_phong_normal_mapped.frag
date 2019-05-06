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

// Light properties
struct light
{
	ivec4 att;
	vec4 pos;
	vec4 dir;
	vec4 amb;
	vec4 dif;
	vec4 spec;
	vec4 atten;
	vec4 r;
};

layout (std140) uniform lights
{
	vec4 amb;		// ambient color
	ivec4 lt_att;	// lt_att[0]: number of lights
	light lt[2];	// Values for each light
};


// Texture samplers (PART 3/4)
uniform sampler2D albedo;
uniform sampler2D emissive;
uniform sampler2D metal_roughness;
uniform sampler2D normal;

// Input attributes, written in the vertex shader and interpolated
// Part 2
in vec3 vtx_normal;
in vec3 vtx_position;

// Part 3
in vec2 vtx_uv;

// Part 4
in vec4 vtx_tangent;

// Final color of the fragment
out vec4 frag_color;


// Forward declare the phong lighting function
vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength);

vec3 lighting(vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength) {
	vec3 color = vec3(0);
	
	// Iterate through all lights and compute lighting
	for(int i=0;i<lt_att[0];i++) {
		color+=phong(i,baseColor,n,v,shininess,specularStrength);
	}
	
	return color;
}


////////////////////////////////////////////////
vec3 phong(int i, vec4 baseColor, vec3 n, vec3 v, float shininess, float specularStrength) {

	vec3 lightDir =  normalize(-lt[i].dir.xyz);
	vec3 lightCol = lt[i].dif.rgb;
	vec3 ambient_color = amb.rgb;
	
	vec3 phong_lighting = vec3(0);
	
	////////////////////////////////////////
	// Compute Phong Lighting 
	// Write result into "phong_lighting"
	////////////////////////////////////////
	
	// Your implementation

	////////////////////////////////////////
	
	return phong_lighting;
	
}

vec3 compute_normal() { 
	vec3 norm = vec3(0);
	
	//////////////////////////////////////////////////////////////////////
	/// PART 2
	/// Read normal from interpolated value
	//////////////////////////////////////////////////////////////////////
	
	// Your implementation

	//////////////////////////////////////////////////////////////////////
	/// PART 4
	/// Construct the tangent frame from normal, tangent and bitangent
	/// Sample the tangent space normal from the texture
	/// Transform the tangent space normal to world space
	//////////////////////////////////////////////////////////////////////
	
	// Your implementation
	
	//////////////////////////////////////////////////////////////////////
	
	return norm;
}

vec3 compute_vertex_to_eye() {
	vec3 vertex_to_eye = vec3(0);
	
	////////////////////////////////////////////
	/// Compute direction from fragment to camera
	////////////////////////////////////////////
	
	// Your implementation
	
	////////////////////////////////////////////
	
	return vertex_to_eye;
}

void main()
{
	// Set parameters for this pixel
    float shininess = 90;
    float specularStrength = 0.5;
    vec4 baseColor = vec4(0.1, 0.1, 0.8, 1); 
	vec3 emissiveColor = vec3(0);
	
	////////////////////////////////////////////
	/// PART 3 
	/// Read material properties from textures
	////////////////////////////////////////////
	
	
	////////////////////////////////////////////
	
	vec3 norm = compute_normal();
	vec3 vertex_to_eye = compute_vertex_to_eye();
	
	vec3 color = emissiveColor + lighting(baseColor, norm, vertex_to_eye, shininess, specularStrength);
	frag_color=vec4(color,1.f);
}