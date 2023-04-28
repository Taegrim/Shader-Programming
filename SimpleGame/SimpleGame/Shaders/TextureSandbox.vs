#version 330

in vec3 a_position;
in vec2 a_texPos;

out vec2 v_texPos;

void main()
{
	gl_Position = vec4(a_position, 1.0);
	v_texPos = a_texPos;
}