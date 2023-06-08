#version 330

layout(location=0) out vec4 FragColor;
layout(location=1) out vec4 FragColor1;
layout(location=2) out vec4 FragColor2;
layout(location=3) out vec4 FragColor3;
layout(location=4) out vec4 FragColor4;

in vec4 v_color;
//varying vec4 v_color;
in vec2 v_uv;

uniform sampler2D u_texture;

uniform int u_step;

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

	FragColor2 = vec4(value);
}

void Textured()
{
	vec4 value = texture(u_texture, v_uv) * v_color;
	FragColor = value;
}

void SpriteAnimation()
{
	int xStep = u_step % 8;
	int yStep = u_step / 8;

	float x = v_uv.x / 8.0f + xStep / 8.0f + 0.007f;		// 이미지가 치우쳐 있으므로 0.007를 더해 중앙에 맞춤
	float y = v_uv.y / 6.0f + yStep / 6.0f;
	
	vec2 newPos = vec2(x, y);
	FragColor = texture(u_texture, newPos);
}

void main()
{
	//FragColor = v_color;
	//FragColor = vec4(v_uv, 0, 1);
	//Circle();
	//Circles();
	//Textured();
	SpriteAnimation();

	FragColor1 = v_color;
	FragColor2 = vec4(1, 0, 0, 1);
	FragColor3 = vec4(0, 1, 0, 1);
	FragColor4 = vec4(0, 0, 1, 1);
}
