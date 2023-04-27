#version 330

in vec3 a_position;
in vec2 a_texPos;

out vec2 v_texPos;

void main()
{
	vec4 newPosition = vec4(a_position, 1.0f);
	gl_Position = newPosition;
	v_texPos = a_texPos;
}