#version 330

layout(location = 0) in vec3 in_position;		

uniform mat4 Projection, View, Model;

void main()
{
	gl_Position = Projection * View * Model*vec4(in_position, 1); 
}
