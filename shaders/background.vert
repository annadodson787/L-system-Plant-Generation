#version 330 core
layout (location=0) in vec4 pos;
layout (location=1) in vec4 v_color;

out vec3 vtx_frg_pos;

void main()
{
	gl_Position=vec4(pos.xyz,1.f);
	vtx_frg_pos=pos.xyz;
}