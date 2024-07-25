#version 330

uniform sampler2D sampler_tex;



in vec4 varying_normal;
in vec2 varying_texcoord;
in vec3 varying_pos;

//int vec3 varying_gouraund;

out vec4 fragment_colour;

void main(void)
{
	vec3 tex_colour = texture(sampler_tex, varying_texcoord).rgb;

    vec4 normals = normalize(varying_normal);

	fragment_colour = vec4(tex_colour,1);

	

	//vec3 N = normalize(varying_normal);
	//fragment_colour = vec4(tex_colour * N.y,1);
}