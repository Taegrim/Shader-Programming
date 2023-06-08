#version 330

in vec3 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

void main()
{
	gl_Position = vec4(a_position, 1.0);
	v_texCoord = vec2(a_texCoord.x, 1 - a_texCoord.y);
}
