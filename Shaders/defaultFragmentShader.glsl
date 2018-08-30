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
	vec3 V = normalize(world_pos - eye_pos);
	vec3 L1 = normalize(vec3(0.5,1,0.5));
	vec3 L2 = normalize(vec3(-0.45,-0.6,-0.3));
	vec3 H1 = normalize(L1 - V);
	vec3 H2 = normalize(L2 - V);
	float lambert1 = max(0, dot(N, L1));
	float specular1 =  lambert1 > 0 ? max(0, dot(N, H1)) : 0;
	specular1 = pow(specular1, 50);
	float lambert2 = max(0, dot(N, L2));
	float specular2 =  lambert2 > 0 ? max(0, dot(N, H2)) : 0;
	specular2 = pow(specular2, 50);
	
	float fresnel = dot(normalize(eye_pos - world_pos), N);
	fresnel = 2 - fresnel;
	vec4 reflectionColor = texture(cubeMap, reflect(V, N));

	out_color = vec4(((lambert1 + specular1) * vec3(1,0.6,0.5) * 0.75 +
					  (lambert2 + specular2) * vec3(0.7,1,1)* 0.75)
			   		 * vcolor * fresnel, 1);	
	// out_color = 0.7 * out_color + 0.3 * reflectionColor;
	float x = gl_FragCoord.z;
	x *= x; x *= x; x *= x; x *= x;
	x *= x; x *= x; x *= x;

	out_color.rg *= 1 - x;
	out_color = mix(out_color, reflectionColor, 0.1);
	// out_color += vec4(x);
}