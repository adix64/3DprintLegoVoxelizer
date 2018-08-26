#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform int invertColor;

out vec2 texcoord;
out vec3 vcolor;
out vec3 world_normal;
out vec3 world_pos;
void main()
{
	world_normal = vec3(Model * vec4(v_normal, 0.0));
	texcoord = v_texture_coord;	
	vcolor = v_color;

	world_pos = vec3(Model * vec4(v_position, 1));		
	gl_Position = Projection * View * vec4(world_pos, 1);
}
