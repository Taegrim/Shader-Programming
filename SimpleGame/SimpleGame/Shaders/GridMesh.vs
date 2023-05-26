#version 330

layout(location=0) in vec3 a_position;

const float PI = 3.141592;

void Translate()
{
	float scale = 0.2f;
	vec3 trans = vec3(0.5f);

	vec4 newPosition = vec4(a_position, 1.0f);

	newPosition.y += sin(2 * PI * (newPosition.x + 0.5f)) * 0.5f;

	newPosition.xyz = newPosition.xyz * scale + trans;

	gl_Position = newPosition;
}

void SinVS()
{
	vec4 newPosition = vec4(a_position, 1.0f);

	newPosition.y += sin(2 * PI * (newPosition.x + 0.5f)) * 0.5f;

	gl_Position = newPosition;
}

void main()
{
	//Translate();
	SinVS();
}
