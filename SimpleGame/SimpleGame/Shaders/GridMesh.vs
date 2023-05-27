#version 330

layout(location=0) in vec3 a_position;

const float PI = 3.141592;

uniform float u_time;

out vec2 v_tex;
out float v_brightness;

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
	float xValue = a_position.x + 0.5f;
	float sinValue = sin(2 * PI * (xValue) + u_time);

	newPosition.y += xValue * sinValue * 0.5f;

	gl_Position = newPosition;


	float tx = xValue;
	float ty = -a_position.y + 0.5f;
	v_tex = vec2(tx, ty);
	v_brightness = (sinValue + 1.0f + 0.3f) * 0.5f;
}

void SinMix()
{
	vec4 newPosition = vec4(a_position, 1.0f);
	float xValue = a_position.x + 0.5f;
	float sinValue = sin(2 * PI * (xValue) + u_time);

	newPosition.y += xValue * sinValue * 0.5f;

	vec3 targetPosition = vec3(1.0f, 1.0f, 0.0f);
	vec3 morphPosition = mix(newPosition.xyz, targetPosition, fract(u_time * 0.1f));

	gl_Position = vec4(morphPosition, 1.0f);


	float tx = xValue;
	float ty = -a_position.y + 0.5f;
	v_tex = vec2(tx, ty);
	v_brightness = (sinValue + 1.0f + 0.3f) * 0.5f;
}

void SinMorph()
{
	vec4 newPosition = vec4(a_position, 1.0f);
	float xValue = a_position.x + 0.5f;
	float sinValue = sin(2 * PI * (xValue) + u_time * 3.0f);

	newPosition.y += xValue * sinValue * 0.5f;

	vec3 targetPosition = vec3(1.0f, 1.0f, 0.0f);
	vec3 morphPosition = mix(newPosition.xyz, targetPosition, fract(u_time * 0.3f) * xValue * 2.0f);

	gl_Position = vec4(morphPosition, 1.0f);


	float tx = xValue;
	float ty = -a_position.y + 0.5f;
	v_tex = vec2(tx, ty);
	v_brightness = (sinValue + 1.0f + 0.3f) * 0.5f;
}

void main()
{
	//Translate();
	//SinVS();
	//SinMix();
	SinMorph();
}
