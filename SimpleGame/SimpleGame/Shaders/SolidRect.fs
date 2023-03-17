#version 330

layout(location=0) out vec4 FragColor;

uniform vec4 u_Color;

in vec4 v_Color;

void main()
{
	FragColor = vec4(v_Color.r, v_Color.g, v_Color.b, v_Color.a);
}
