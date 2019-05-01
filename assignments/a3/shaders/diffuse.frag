~include version;
~include material;
~include camera;
~include lights;
in vec3 vtx_normal;
in vec3 vtx_frg_pos;
in vec2 vtx_uv;
out vec4 frag_color;

uniform sampler2D albedo;
uniform sampler2D ao;
uniform sampler2D emissive;
uniform sampler2D metal_roughness;
uniform sampler2D normal;

void main()
{

	frag_color=texture(albedo, vtx_uv) * max(0, dot(vtx_normal, normalize(vec3(1, -1, 1))));
}