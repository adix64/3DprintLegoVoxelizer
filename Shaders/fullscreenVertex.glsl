#version 330                                                                        

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoordinates;

out vec2 texCoords;
void main()
{          
	texCoords = TexCoordinates;
    gl_Position = vec4(Position,1);
}
