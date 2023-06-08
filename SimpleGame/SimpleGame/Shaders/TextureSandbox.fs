#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_texPos;
uniform sampler2D u_texSampler;
uniform vec2 u_xyRepeat;
uniform int u_offset;

void Default()
{
	FragColor = texture(u_texSampler, v_texPos);
}

void VertMirror()
{
	vec2 newPos = v_texPos;
	newPos.y = 1.0 - abs(newPos.y * 2.0 - 1.0);
	
	//if(newPos.y < 0.5){
	//	newPos.y *= 2;
	//}
	//else{
	//	newPos.y = 1 - newPos.y;
	//	newPos.y *= 2;
	//}

	FragColor = texture(u_texSampler, newPos);
}

void UVSwap()
{
	vec2 newPos = v_texPos;
	vec2 temp = v_texPos;

	temp.x = newPos.x;
	newPos.x = 1 - v_texPos.y;
	newPos.y = temp.x;

	FragColor = texture(u_texSampler, newPos);
}

void HorzArrange()
{
	vec2 newPos = v_texPos;

	newPos.x = fract(v_texPos.x * 3.0f);
	newPos.y = floor(3.0f * (1.0f - v_texPos.x)) / 3.0f + (v_texPos.y / 3.0f);

	FragColor = texture(u_texSampler, newPos);
}

void OtherHorzArrange()
{
	vec2 newPos = v_texPos;
	
	float ratio = 1.0 / 3.0;
	newPos.x = mod(v_texPos.x, ratio) / ratio;

	float t = floor(v_texPos.x / ratio);
	newPos.y = v_texPos.y * ratio + ratio * t;

	FragColor = texture(u_texSampler, newPos);
}

void ReverseHorzArrange()
{
	vec2 newPos = v_texPos;

	//float ratio = 1.0 / 3.0;
	//newPos.x = mod(v_texPos.x, ratio) / ratio;

	//float t = floor(v_texPos.x / ratio);
	//newPos.y = v_texPos.y * ratio + (1 - ratio * (t + 1));

	newPos.x = fract(newPos.x * 3.0f);
	newPos.y = floor(v_texPos.x * 3.0f) / 3.0f + (v_texPos.y / 3.0f);

	FragColor = texture(u_texSampler, newPos);
}

void Reverse()
{
	vec2 newPos = v_texPos;	

	float ratio = 1.0 / 3.0;
	float t = floor(v_texPos.y / ratio);
	newPos.y = mod(v_texPos.y, ratio) + (1.0 - ratio * (t + 1));

	FragColor = texture(u_texSampler, newPos);
}

void OtherReverse()
{
	vec2 newPos = v_texPos;
	
	float dy = fract(v_texPos.y * 3.0f) / 3.0f;
	newPos.y = floor(3.0f * (1.0f - v_texPos.y)) / 3.0f + dy;
	
	FragColor = texture(u_texSampler, newPos);
}

void Mix1()
{
	vec2 newPos = v_texPos;	

	float dx = v_texPos.x * 2.0f;

	newPos.x = dx + floor((1.0f - v_texPos.y) * 2.0f) * 0.5f;
	newPos.y = fract(v_texPos.y * 2.0f);
	
	FragColor = texture(u_texSampler, newPos);
}

void Mix2()
{
	vec2 newPos = v_texPos;	
	
	if(v_texPos.x < 0.5f){
		newPos.x *= 2;
		newPos.y *= 2;
	}
	else{
		newPos.x *= 2;
		newPos.y = newPos.y * 2 + 0.5f;
	}
	
	FragColor = texture(u_texSampler, newPos);
}

void OtherMix()
{
	vec2 newPos = v_texPos;	

	// repeat 값을 uniform 으로 받아서 처리할 수도 있음
	float xRepeat = u_xyRepeat.x;
	float yRepeat = u_xyRepeat.y;

	float dx = v_texPos.x * xRepeat;

	newPos.x = fract(dx + floor((1.0f - v_texPos.y) * yRepeat) * 0.5f);
	newPos.y = fract(v_texPos.y * yRepeat);
	
	FragColor = texture(u_texSampler, newPos);
}

void OtherMix2()
{
	vec2 newPos = v_texPos;	

	float xRepeat = u_xyRepeat.x;
	float yRepeat = u_xyRepeat.y;

	float dy = v_texPos.y * yRepeat;

	newPos.x = fract(v_texPos.x * xRepeat);
	newPos.y = fract(dy + floor(v_texPos.x * xRepeat) * 0.5f);
	
	FragColor = texture(u_texSampler, newPos);
}

void AxisChange()
{
	vec2 newPos = v_texPos;	

	float y = fract(v_texPos.x + v_texPos.y);
	float x = fract(v_texPos.x + (1.0f - v_texPos.y));

	float dx = x * 1.0f;

	newPos.x = fract(dx + floor(y * 2.0f) * 0.5f);
	newPos.y = fract(y * 2.0f);

	FragColor = texture(u_texSampler, newPos);

	//FragColor = vec4(x);
}

void MultiTexture()
{
	FragColor = texture(u_texSampler, v_texPos);
}

void SingleSmileTexture()
{
	vec2 newPos = v_texPos;

	newPos.y = v_texPos.y / 6.0f + u_offset / 6.0f;

	FragColor = texture(u_texSampler, newPos);
}

void main()
{
	//Default();
	//VertMirror();
	UVSwap();
	//HorzArrange();
	//OtherHorzArrange();
	//ReverseHorzArrange();
	//Reverse();
	//OtherReverse();
	//Mix1();
	//Mix2();
	//OtherMix();
	//OtherMix2();
	//AxisChange();
	//MultiTexture();
	//SingleSmileTexture();
}