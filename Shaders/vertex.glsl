#version 330

layout(location = 0) in vec3 in_position;		

uniform mat4 model_matrix;
out int InstanceID;

void main()
{
	InstanceID = gl_InstanceID;
	gl_Position = model_matrix*vec4(in_position,1); 
}
