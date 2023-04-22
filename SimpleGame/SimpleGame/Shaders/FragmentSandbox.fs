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

void Lines()
{
	int lineCount = 4;
	float newValueX = v_texCoord.x * 2 * PI * lineCount;
	float newValueY = v_texCoord.y * 2 * PI * lineCount;
	float greyHorizontal = sin(newValueX);
	float greyVertical = sin(newValueY);
	float newValue = max(greyHorizontal, greyVertical);
	FragColor = vec4(newValue);
}

void Flag()
{
	float value = 0.5 * (sin(v_texCoord.x * PI * 2 - u_time)) * v_texCoord.x;
	float sinValue = sin(v_texCoord.x * PI * 2 * 100);
	float width = 0.005;

	float flagWidth = 0.2;
	float newY = 2.0 * (v_texCoord.y - 0.5);
	float firstLine = newY - flagWidth;
	float secondLine = newY + flagWidth;

	float startY = -0.5;
	float line = newY;
	
	FragColor = vec4(0);
	for(int i = 0; i < 5; ++i){
		if( line + startY < value + width && 
			line + startY > value - width)
		{
			FragColor = vec4(1 * sinValue);
		}
		startY = startY + flagWidth;
	}

}

void TimeFlag()
{
	// 수정 필요
	FragColor = vec4(0);
	for(int i = 0; i < 5; ++i){
		float newTime = u_time + i * 0.2;
		float value = 0.5 * (sin(v_texCoord.x * PI * 2 - u_time)) * v_texCoord.x;
		float sinValue = sin(v_texCoord.x * PI * 2 * 100);
		float width = 0.005;


		if( newY < value + width && 
			newY > value - width)
		{
			FragColor = vec4(1 * sinValue);
		}
	}
}

void main()
{
	//Default();
	//Circle();
	//Circles();
	//Radar();
	//Lines();
	//Flag();
	TimeFlag();
}
