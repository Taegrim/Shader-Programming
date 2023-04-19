#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_texCoord;

uniform vec2 u_point;
uniform vec2 u_points[3];
uniform float u_time;

const float PI = 3.141592;

void Default()
{
	FragColor = vec4(v_texCoord, 0.0f, 1.0f);
}

void SliceHalf()
{
	vec2 newCoord = vec2(0, 0);
	if(v_texCoord.y > 0.5f){
		newCoord = v_texCoord;
	}

	FragColor = vec4(newCoord, 0.0f, 1.0f);
}

void Circle()
{
	float radius = 0.1f;

	if(length(v_texCoord - u_point) < radius){
		FragColor = vec4(v_texCoord, 0.0f, 1.0f);
	}
	else{
		FragColor = vec4(0);
	}
}

void CircleArray()
{
	float radius = 0.1f;
	
	FragColor = vec4(0);
	for(int i = 0; i < 3; ++i){
		if(length(v_texCoord - u_points[i]) < radius){
			FragColor = vec4(v_texCoord, 0.0f, 1.0f);
		}
	}
}

void Circles()
{
	vec2 center = vec2(0.5f);
	
	vec2 temp = v_texCoord - center;
	float d = length(temp);
	float value = sin(d * 2 * PI * 4);

	FragColor = vec4(value);
}

void Radar()
{
	float radius = 0.1f;

	vec2 center = vec2(0.5f);
	vec2 temp = v_texCoord - center;
	float d = length(temp);
	float value = pow(sin(d * 2 * PI * 4 - u_time * 4), 12) - 0.5;
	float t = ceil(value);

	vec4 result = vec4(0);
	for(int i = 0; i < 3; ++i){
			
		if(length(v_texCoord - u_points[i]) < radius){
			result += 1.0 * t;
		}
	}
	
	//FragColor = vec4(t);
	FragColor = vec4(result + 10 * value);
}

void main()
{
	//Default();
	//Circle();
	//Circles();
	Radar();
}
