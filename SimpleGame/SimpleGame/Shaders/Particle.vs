#version 330

layout(location=0) in vec3 a_position;
layout(location=1) in vec3 a_velocity;
layout(location=2) in float a_emitTime;
layout(location=3) in float a_lifeTime;
layout(location=4) in float a_period;
layout(location=5) in float a_amp;
layout(location=5) in float a_value;

const float c_radius = 0.5;
const vec3 c_gravity = vec3(0.0, -0.8, 0.0);
const vec3 c_vel = vec3(0.7, 0.7, 0.0);
const float PI = 3.141592;

uniform float u_time;

vec4 P1()
{
	vec4 newPosition = vec4(0,0,0,1);

	float time;
	time = u_time - a_emitTime;

	if(time < 0.f){
		
	}
	else{
		time = a_lifeTime * fract(time / a_lifeTime);

		newPosition.xyz = a_position 
						+ a_velocity * time
						+ 0.5 * c_gravity * time * time;
		newPosition.w = 1;
	}

	return newPosition;
}

vec4 GraphSin()
{
	vec4 newPosition = vec4(0, 0, 0, 1);

	float time;
	time = u_time - a_emitTime;

	if(time < 0.f){
	
	}
	else{
		time = a_lifeTime * fract(time / a_lifeTime);
		
		float nX = sin(a_value * 2.0 * PI);
		float nY = cos(a_value * 2.0 * PI);

		newPosition.x = a_position.x + nX + time * c_vel.x;
		newPosition.y = a_position.y + nY + time * c_vel.y;

		vec2 dir = vec2(-c_vel.y, c_vel.x);
		dir = normalize(dir);
		newPosition.xy += dir * a_amp * sin(a_period * time * 2.0 * PI) * time;
	}

	return newPosition;
}

void main()
{
	gl_Position = GraphSin();
}
