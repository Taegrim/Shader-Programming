#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_texPos;
uniform sampler2D u_texSampler;

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

	float ratio = 1.0 / 3.0;
	newPos.x = mod(v_texPos.x, ratio) / ratio;

	float t = floor(v_texPos.x / ratio);
	newPos.y = v_texPos.y * ratio + ratio * t;

	FragColor = texture(u_texSampler, newPos);
}

void ReverseHorzArrange()
{
	vec2 newPos = v_texPos;

	float ratio = 1.0 / 3.0;
	newPos.x = mod(v_texPos.x, ratio) / ratio;

	float t = floor(v_texPos.x / ratio);
	newPos.y = v_texPos.y * ratio + (1 - ratio * (t + 1));

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

void Mix1()
{
	vec2 newPos = v_texPos;	
	
	if(v_texPos.y < 0.5f){
		newPos.y *= 2;
		newPos.x = newPos.x * 2 + 0.5f;
	}
	else{
		newPos.y *= 2;
		newPos.x *= 2;
	}
	
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

void main()
{
	//VertMirror();
	//UVSwap();
	HorzArrange();
	//ReverseHorzArrange();
	//Reverse();
	//Mix1();
	//Mix2();
}