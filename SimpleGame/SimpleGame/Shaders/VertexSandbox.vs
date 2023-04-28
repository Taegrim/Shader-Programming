#version 330

in vec3 a_position;

out float v_alpha;

uniform float u_time;

const float PI = 3.141592;

void SinAnim()
{
	float value = a_position.x + 1.f;
	float newY = sin(PI * value - u_time) * (value / 2.0);
	
	vec4 newPosition = vec4(a_position, 1.0);
	newPosition.y = newY;

	gl_Position = newPosition;
	v_alpha = 1.0 - (value / 2.0);
}

void OtherSin()
{
	float value = (a_position.x + 1.0) / 2.0;

	float newY = sin(2 * PI * value * 2 - u_time) * value;

	vec4 newPosition = vec4(a_position, 1.0);
	newPosition.y = newY;

	gl_Position = newPosition;
	v_alpha = 1.0 - value;
}

void Sin()
{
	float value = (a_position.x + 1.0) / 2.0;

	float newY = sin(value * 2 * PI * 2 - u_time * 2);
	gl_Position = vec4(a_position.x, newY, a_position.z, 1.0);
	v_alpha = 1.0 - value;
}

void main()
{
	//OtherSin();
	Sin();
}