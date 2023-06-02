#version 330

in vec3 a_position;
in vec2 a_texPos;

out vec2 v_texPos;

uniform vec4 u_posScale;	// (x, y : translate, z, w : scale)

void main()
{
	vec4 newPosition = vec4(a_position.xy * u_posScale.zw, 0.0, 1.0);
	newPosition.xy = newPosition.xy + u_posScale.xy;
	gl_Position = newPosition;
	v_texPos = vec2(a_texPos.x, 1.0 - a_texPos.y);
}