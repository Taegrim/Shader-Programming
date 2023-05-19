#version 330

layout(location=0) out vec4 FragColor;

in vec4 v_color;
//varying vec4 v_color;
in vec2 v_uv;

uniform sampler2D u_texture;

const float PI = 3.141592;

void Default()
{
	FragColor = vec4(v_uv, 0.0f, 1.0f);
}

void SliceHalf()
{
	vec2 newCoord = vec2(0, 0);
	if(v_uv.y > 0.5f){
		newCoord = v_uv;
	}

	FragColor = vec4(newCoord, 0.0f, 1.0f);
}

void Circle()
{
	float radius = 0.5f;
	vec2 center = vec2(0.5f);

	if(length(v_uv - center) < radius){
		FragColor = v_color;
	}
	else{
		FragColor = vec4(0);
	}
}

void Circles()
{
	vec2 center = vec2(0.5f);
	
	vec2 temp = v_uv - center;
	float d = length(temp);
	float value = sin(d * 2 * PI * 4);

	FragColor = vec4(value);
}

void Textured()
{
	vec4 value = texture(u_texture, v_uv) * v_color;
	FragColor = value;
}

void main()
{
	//FragColor = v_color;
	//FragColor = vec4(v_uv, 0, 1);
	//Circle();
	Textured();
}
