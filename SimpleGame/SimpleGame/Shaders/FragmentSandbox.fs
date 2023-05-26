#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_texCoord;

uniform vec2 u_point;
uniform vec2 u_points[3];
uniform float u_time;
uniform sampler2D u_texture;

uniform int u_step;

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
	float radius = 0.3f;
	float ratio = 1.0 / radius;
	
	vec4 color = vec4(0);
	for(int i = 0; i < 3; ++i){
		float d = length(v_texCoord - u_points[i]);

		if(d < radius){
			// d * 2 * ratio * PI 는 반지름에 따라 0~1 에 맞추는 것
			// 한 주기를 타게끔 한 것
			float temp = sin(10 * d * 2 * ratio * PI - u_time * 5);
			color = vec4(temp);
		}
	}
	FragColor = color;
}

void Circles()
{
	vec2 center = vec2(0.5f);
	
	vec2 temp = v_texCoord - center;
	float d = length(temp);
	float value = sin(d * 4 * PI * 10);

	FragColor = vec4(value);
}

void Radar()
{
	float radius = 0.1f;

	vec2 center = vec2(0.5f);
	float d = length(v_texCoord - center);
	float value = pow(sin(d * 2 * PI * 4 - u_time * 4), 12);
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

void Radar2()
{
	float radius = 0.05f;

	vec2 center = vec2(0.5f, 0.f);
	float d = length(v_texCoord - center);
	float value = pow(sin(d * 2 * PI * 4 - u_time * 10), 4);
	value = clamp(value, 0 ,1);

	vec4 result = vec4(value);

	for(int i = 0; i < 3; ++i){
		
		float dist = length(v_texCoord - u_points[i]);

		// sin이 양수일때의 절반만을 사용
		// dist에 값을 더해 sin 함수 자체를 움직임
		// clamp 로 0~1 사이 값만을 사용하도록 함
		float temp = sin((dist + PI / 2) * 4 * PI);
		temp = clamp(temp, 0, 1);
		if(dist < radius)
			result += 10 * value * vec4(0, temp, 0, 0);
		// 0~1 사이인 value 값을 곱해서 sin주기가 음수일 때는
		// 레이더에서 나오지 않도록 함
	}
	
	//FragColor = vec4(t);
	FragColor = vec4(result);
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
	float value = 0.5 * sin(v_texCoord.x * PI * 2 - u_time) * v_texCoord.x;
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

void TimeFrag()
{
	float color = 0;
	for(int i = 0; i < 3; ++i){
		float newTime = u_time + i * 0.2;
		float value = 0.5 * sin(v_texCoord.x * PI * 2 - newTime * 2) * v_texCoord.x;
		float sinValue = sin(v_texCoord.x * PI * 2 * 10 - newTime * 10);
		float width = 0.01 * v_texCoord.x * 5 + 0.001;
		
		float newY = 2.0 * (v_texCoord.y - 0.5);

		if( newY < value + width && 
			newY > value - width)
		{
			color += 1 * sinValue * (1.0 - v_texCoord.x);
		}
	}
	FragColor = vec4(color);
}

void OtherTimeFrag()
{
	float color = 0;

	for(int i = 0; i < 10; ++i){
		float time = u_time + i * 0.3;
		
		float value = sin(v_texCoord.x * 2 * PI - time * 4) / 4 * v_texCoord.x + 0.5;
		float sinValue = sin(v_texCoord.x * 2 * PI * 20 - time * 5);
		float width = 0.001 + 0.001 * v_texCoord.x * 5;
		
		if(value - width < v_texCoord.y &&
			value + width > v_texCoord.y)
		{
			color += 1 * sinValue * (1.0 - v_texCoord.x);
		}
	}
	FragColor = vec4(color);
}

void SinGraph()
{
	float value = sin(v_texCoord.x * 2 * PI - u_time * 2) * v_texCoord.x / 2 
			+ 0.5f;
	float width = 0.01f;

	vec4 color = vec4(0);

		
	if(value - width < v_texCoord.y &&
		value + width > v_texCoord.y)
	{
		color = vec4(1);
	}

	FragColor = color;
}

void RealFlag()
{
	float period = (v_texCoord.x + 1.0f) * 1.0f;
	float amp = 0.75f;

	float xValue = v_texCoord.x * 2.0f * PI * period;
	float yValue = ((1.0f - v_texCoord.y) - 0.5f) * 2.0f;
	float sinValue = sin(xValue - 3.0f * u_time) * 0.25f * v_texCoord.x;

	if(sinValue + amp > yValue &&
		sinValue - amp < yValue)
	{
		float vX = v_texCoord.x;
		float yWidth = 2 * amp;
		float yDist = yValue - (sinValue - amp);
		float vY = 1 - (yDist / yWidth);

		FragColor = texture(u_texture, vec2(vX, vY));
		//FragColor = vec4(vX, vY, 0, 1);
	}
	else {
		FragColor = vec4(0);
	}

}

void SpriteAnimation()
{
	int xStep = u_step % 8;
	int yStep = u_step / 8;

	float x = v_texCoord.x / 8.0f + xStep / 8.0f;	// 8*6 sheet
	float y = v_texCoord.y / 6.0f + yStep / 6.0f;
	
	vec2 newPos = vec2(x, y);
	FragColor = texture(u_texture, newPos);
}

void main()
{
	//Default();
	//Circle();
	//Circles();
	//CircleArray();
	//Radar();
	//Radar2();
	//Lines();
	//Flag();
	//TimeFrag();
	//OtherTimeFrag();
	//SinGraph();
	RealFlag();
	//SpriteAnimation();
}
