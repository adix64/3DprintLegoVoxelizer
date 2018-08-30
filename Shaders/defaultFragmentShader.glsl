#version 330
 
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 color;
uniform int mode;
uniform int invertColor;
uniform vec3 eye_pos;
uniform samplerCube cubeMap;

in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;
in vec3 vcolor;

layout(location = 0) out vec4 out_color;

void main()
{	
	vec3 N = normalize(world_normal);
	float lambert1 = 0.25 + 0.75 * max(0, dot(N, normalize(vec3(0.5,1,0.5))));
	float lambert2 = 0.25 + 0.75 * max(0, dot(N, normalize(vec3(-0.45,-0.6,-0.3))));
	float fresnel = dot(normalize(eye_pos - world_pos), N);
	fresnel = 2 - fresnel;
	vec3 V = normalize(world_pos - eye_pos);
	vec4 reflectionColor = texture(cubeMap, reflect(V, N));

	out_color = vec4((lambert1 * vec3(1,0.6,0.5) * 0.75 + lambert2 * vec3(0.7,1,1)* 0.75)*vcolor *fresnel, 1);	
	// out_color = 0.7 * out_color + 0.3 * reflectionColor;
	float x = gl_FragCoord.z;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x;

	out_color.rg *= 1 - x;
	out_color *= vec4(.5) + reflectionColor;
	// out_color += vec4(x);
}