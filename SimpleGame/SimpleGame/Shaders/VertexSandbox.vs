#version 330

in vec3 a_position;

uniform float u_time;

const float PI = 3.141592;

void main()
{
	float value = a_position.x + 1.f;
	float newY = sin(PI * value - u_time) * (value / 2.0);
	
	vec4 newPosition = vec4(a_position, 1.0);
	newPosition.y = newY;

	gl_Position = newPosition;
}