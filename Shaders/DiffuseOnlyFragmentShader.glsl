#version 330
 
uniform sampler2D diffuseMap;
uniform float alphaOverrider;
in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;


layout(location = 0) out vec4 out_color;



void main()
{
	out_color = texture2D(diffuseMap, texcoord);

	out_color.a *= alphaOverrider;
}