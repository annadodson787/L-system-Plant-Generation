#version 330 core
uniform vec4 color=vec4(0.f,1.f,0.f,1.f);

in vec3 vtx_frg_pos;

out vec4 frag_color;

void main()								
{ 
	vec3 color_0=vec3(0.f,0.f,1.f);
	vec3 color_1=vec3(0.f,1.f,0.f);
	float m=abs(vtx_frg_pos.x*vtx_frg_pos.x);
	vec3 c=mix(color_0,color_1,m);
	frag_color=vec4(c,1.f);
}	