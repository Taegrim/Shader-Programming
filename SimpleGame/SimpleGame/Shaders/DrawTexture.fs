#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_texPos;

uniform sampler2D u_texSampler;

void main()
{
	FragColor = texture(u_texSampler, v_texPos);
}