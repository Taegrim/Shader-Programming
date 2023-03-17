#version 330

layout(location=0) in vec3 a_Position;

void main()
{
	vec4 newPosition;
	newPosition.xyz = a_Position;
	newPosition.w = 1;
	gl_Position = newPosition;
}
