#version 330

in vec3 a_position;

void main()
{
	vec4 newPosition = vec4(a_position, 1.0f);
	gl_Position = newPosition;
}