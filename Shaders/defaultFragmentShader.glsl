#version 330
 
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 color;
uniform int mode;
uniform int invertColor;
uniform vec3 eye_pos;

in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;
in vec3 vcolor;

layout(location = 0) out vec4 out_color;

void main()
{	
	float lambert1 = 0.25 + 0.75 * max(0, dot(normalize(world_normal), normalize(vec3(0.5,1,0.5))));
	float lambert2 = 0.25 + 0.75 * max(0, dot(normalize(world_normal), normalize(vec3(-0.45,-0.6,-0.3))));
	float fresnel = dot(normalize(eye_pos - world_pos), normalize(world_normal));
	fresnel = 2 - fresnel;
//fresnel = 1;
	out_color = vec4((lambert1 * vec3(1,0.6,0.5) * 0.75 + lambert2 * vec3(0.7,1,1)* 0.75)*vcolor *fresnel, 1);	
}