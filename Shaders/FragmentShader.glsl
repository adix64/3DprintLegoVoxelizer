#version 330

/////////////----MATERIAL ATTRIBUTES----/////////////
uniform int useDiffuseMap;
uniform sampler2D diffuseMap;
vec3 diffuseColor;

uniform int useSpecularMap;
uniform sampler2D specularMap;
uniform float specularPower;

uniform int useNormalMap;
uniform sampler2D normalMap;

uniform float fresnelCos;
uniform float ambientReflectivity;

uniform int useReflections;
uniform samplerCube reflectionCubeMap;
uniform samplerCube cubeShadowMap1, cubeShadowMap2;

uniform vec3 lightPos1, lightPos2;
///////////////////////////////////////////////////////
in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;

uniform vec3 eye_position;


layout(location = 0) out vec4 out_color;
#define EPSILON 0.001
float computePlight_shadowFactor1(vec3 light_direction, float distance)
{	
	int percent = 0;
	vec3 ld;
	float lightReach;

	vec3 sampleOffsetDirections[8] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1)//,
//	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
//	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
//	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	float shadow = 8;
	float bias = 0.15;
	int samples = 8;
//	float viewDistance = length(viewPos - fragPos);
	float diskRadius = 0.025 + 0.002* distance;// + 0.0001 * distance *distance;
	for(int k = 0; k < samples; ++k)
	{
	    float closestDepth = texture(cubeShadowMap1, 
	    					light_direction + sampleOffsetDirections[k] * diskRadius).r;
//	    closestDepth *= 1000;   // Undo mapping [0;1]
	    if(distance - EPSILON > closestDepth)
	        shadow -= 1.0;
	}
	return (shadow/samples) * 0.83333;
}

float computePlight_shadowFactor2(vec3 light_direction, float distance)
{	
	int percent = 0;
	vec3 ld;
	float lightReach;

	vec3 sampleOffsetDirections[8] = vec3[]
	(
	   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
	   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1)//,
//	   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
//	   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
//	   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
	);
	float shadow = 8;
	float bias = 0.15;
	int samples = 8;
//	float viewDistance = length(viewPos - fragPos);
	float diskRadius = 0.025 + 0.002* distance;// + 0.0001 * distance *distance;
	for(int k = 0; k < samples; ++k)
	{
	    float closestDepth = texture(cubeShadowMap2, 
	    					light_direction + sampleOffsetDirections[k] * diskRadius).r;
//	    closestDepth *= 1000;   // Undo mapping [0;1]
	    if(distance - EPSILON > closestDepth)
	        shadow -= 1.0;
	}
	return (shadow/samples) * 0.83333;
}


void main()
{
	float spow = specularPower;
	if(useSpecularMap != 0)
		spow = pow(texture2D(specularMap, texcoord).r,3);
	vec3 N = normalize(world_normal);
	vec3 V = normalize(eye_position - world_pos);
//LIGHT 1
	vec3 L1 = normalize(lightPos1 - world_pos);
	float dotProd1 = dot(N, L1);
	float light1 = max(dotProd1, 0.2);
	vec3 H1 = normalize(L1 + V);
	float specularFactor1 = max(dot(N, H1),0);

	if(specularFactor1 > 0 && dotProd1 > 0)
	{
		light1 += pow(specularFactor1, spow * 200); 
	}
//LIGHT 2
	vec3 L2 = normalize(lightPos2 - world_pos);
	float dotProd2 = dot(N, L2);
	float light2 = max(dotProd2, 0.2);
	
	vec3 H2 = normalize(L2 + V);
	float specularFactor2 = max(dot(N, H2),0);

	if(specularFactor2 > 0 && dotProd2 > 0)
	{
		light2 += pow(specularFactor2, spow * 200); 
	}

	vec3 light_direction1 = world_pos - lightPos1;
    float distance1 = length(light_direction1);
	vec3 light_direction2 = world_pos - lightPos2;
	float distance2 = length(light_direction2);
   	float shadow_factor1 = computePlight_shadowFactor1(light_direction1, distance1);
   	float shadow_factor2 = computePlight_shadowFactor2(light_direction2, distance2);

	out_color = texture2D(diffuseMap, texcoord) * (light1 * vec4(1,0.9,0.7,1) * shadow_factor1 + 
												   light2 * vec4(0.7,0.9,1,1) * shadow_factor2);
	if (useReflections != 0)
	{
		out_color += texture(reflectionCubeMap, reflect(normalize(world_pos - eye_position), normalize(world_normal))) * spow * 0.5;
		float absdotNV = abs(dot(N,V));
		if(absdotNV < 0.35f)
		{
			out_color = out_color +  (0.35 - absdotNV) * vec4(pow(out_color.r, 0.1), pow(out_color.g, 0.1), pow(out_color.b, 0.1), 1);
		}
	}

	out_color.a = texture2D(diffuseMap, texcoord).a;
	if(texture2D(diffuseMap, texcoord).a < 0.5)
		discard;

	float clarity = clamp(gl_FragCoord.z * gl_FragCoord.w + 0.5, 0, 1);
	float fog = 1 - clarity;
	out_color = vec4(out_color.r * clarity + fog * 0.5,
					 out_color.g * clarity + fog * 0.5,
					 out_color.b * clarity + fog * 0.7,
					 out_color.a);
	// out_color = vec4(spow/200,spow/200,spow/200,1);
}