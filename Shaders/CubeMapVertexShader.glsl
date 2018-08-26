#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec2 texcoord;
out vec3 world_pos;
out vec3 world_normal;
void main()
{
	// TODO : pass v_texture_coord as output to Fragment Shader
	texcoord = v_texture_coord;
	world_pos =  vec3(Model * vec4(v_position, 1.0));
	world_normal =  vec3(Model * vec4(v_normal, 0.0));
	gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
