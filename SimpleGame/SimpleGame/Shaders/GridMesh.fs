#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_tex;
in float v_brightness;

uniform sampler2D u_texSampler;

void Tiling()
{
	int repeatCount = 2;
	vec2 newPosition = fract(v_tex * float(repeatCount));

	FragColor = texture(u_texSampler, newPosition) * v_brightness;
}

void Radar()
{
	vec2 newPosition = vec2(v_tex);

	FragColor = texture(u_texSampler, newPosition) * v_brightness;
}

void main()
{
	Tiling();
}
