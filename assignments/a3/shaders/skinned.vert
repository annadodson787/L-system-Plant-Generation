~include version;
~include camera;

uniform mat4 model=mat4(1.0f);

layout (location=0) in vec4 pos;
layout (location=1) in vec4 color;
layout (location=2) in vec4 normal;
layout (location=3) in vec4 uv;
layout (location=4) in vec4 tangent;
layout (location=5) in vec4 weights;
layout (location=6) in ivec4 joints;


out vec3 vtx_normal;
out vec4 vtx_tangent;
out vec3 vtx_position;
out vec2 vtx_uv;
out vec3 vtx_frg_pos;

void main()												
{
	vtx_position=vec3(model*vec4(pos.xyz,1.f));
	gl_Position=pvm*vec4(vtx_position,1.f);
	vtx_normal=vec3(transpose(inverse(model))*vec4(normal.xyz,0.f)).xyz;

	
	vtx_tangent=vec4(vec3(transpose(inverse(model))*vec4(tangent.xyz,0.f)), tangent.w);
	vtx_uv = uv.xy;
	vtx_frg_pos=vec3(model*vec4(pos.xyz,1.f));
}