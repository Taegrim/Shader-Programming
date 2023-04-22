#version 330

layout(location=0) out vec4 FragColor;

in float v_alpha;

void main()
{
	float newLines = sin((100.0 * (1.0 - v_alpha)));

	FragColor = vec4(1, 1, 1, newLines);
}