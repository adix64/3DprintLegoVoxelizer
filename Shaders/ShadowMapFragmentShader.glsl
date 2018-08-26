#version 330
in vec2 texcoord;
in vec3 world_pos;
in vec3 wordl_normal;

uniform vec3 eye_position;
uniform sampler2D diffuseMap;
uniform int useDiffuseMap;
out float FragColor;

void main()
{	if (useDiffuseMap != 0)
		if(texture2D(diffuseMap, texcoord).a < 0.5)
			discard;
	FragColor = distance(world_pos.xyz, eye_position);
}

