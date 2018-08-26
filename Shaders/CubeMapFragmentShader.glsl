#version 330
 
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform int use_tex2;
in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;

uniform vec3 eye_position;
uniform vec3 lightPos1, lightPos2;

layout(location = 0) out vec4 out_color;



void main()
{
	vec3 N = normalize(world_normal);
	vec3 V = normalize(eye_position - world_pos);

	vec3 L1 = normalize(lightPos1 - world_pos);
	float dotProd1 = dot(N, L1);
	float light1 = max(dotProd1, 0.2);

	vec3 H1 = normalize(L1 + V);
	float specularFactor1 = max(dot(N, H1),0);
	if(specularFactor1 > 0 && dotProd1 > 0)
		light1 += pow(specularFactor1, 20);

	vec3 L2 = normalize(lightPos2 - world_pos);
	float dotProd2 = dot(N, L2);
	float light2 = max(dotProd2, 0.2);

	vec3 H2 = normalize(L2 + V);
	float specularFactor2 = max(dot(N, H2),0);
	if(specularFactor2 > 0 && dotProd2 > 0)
		light2 += pow(specularFactor2, 20);

	out_color = texture2D(diffuseMap, texcoord) * (light1 + light2);
	out_color.a = texture2D(diffuseMap, texcoord).a;
	if(texture2D(diffuseMap, texcoord).a < 0.5)
		discard;
}