#version 330

// TODO

uniform mat4 combined_xform;
uniform mat4 model_xform;

layout (location=0) in vec3 vertex_position;
layout (location=1) in vec3 vertex_normal;
layout (location=2) in vec2 texture_coords;
layout (location=3) in vec3 position_coords;

out vec4 varying_normal;
out vec2 varying_texcoord;
//out vec3 varying_pos;

void main(void)
{
	gl_Position = combined_xform * model_xform * vec4(vertex_position, 1.0f);
	varying_texcoord = texture_coords;
	varying_normal =  vec4(vertex_normal, 0);
	//varying_pos = position_coords;
	
	vec3 light_direction = vec3(-0.5f, -1, -0.3f);
	vec3 L = normalize(light_direction);

	//varying_normal =  mat3(model_xform) * vec4(vertex_normal, 0);
}