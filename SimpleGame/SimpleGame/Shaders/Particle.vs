#version 330

in vec3 a_position;
in vec3 a_velocity;
in vec4 a_color;
in float a_emitTime;
in float a_lifeTime;
in float a_period;
in float a_amp;
in float a_value;

const float c_radius = 0.5;
const vec3 c_gravity = vec3(0.0, -0.8, 0.0);
const vec3 c_vel = vec3(0.7, 0.7, 0.0);
const float PI = 3.141592;

uniform float u_time;

out vec4 v_color;
//varying vec4 v_color;

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

		newPosition.x = a_position.x + nX + time * a_velocity.x;
		newPosition.y = a_position.y + nY + time * a_velocity.y;

		vec2 dir = vec2(-a_velocity.y, a_velocity.x);
		dir = normalize(dir);
		newPosition.xy += dir * a_amp * sin(a_period * time * 2.0 * PI) * time;
	}

	return newPosition;
}

void main()
{
	gl_Position = GraphSin();
	v_color = a_color;
}
