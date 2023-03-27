#version 330

layout(location=0) in vec3 a_Position;
layout(location=1) in vec3 a_Velocity;
layout(location=2) in float a_emitTime;
layout(location=3) in float a_lifeTime;

const vec3 c_Gravity = vec3(0.0, -0.8, 0.0);
const vec3 c_Vel = vec3(0.1, 0.0, 0.0);
const float PI = 3.141592;
const float MAX_TIME = 10.0;
uniform float u_Time;

void main()
{
	vec4 newPosition = vec4(0,0,0,1);
	float t = MAX_TIME * fract(u_Time / MAX_TIME);
	float yTime = (t/MAX_TIME) * 2.0 * PI;

	newPosition.x = a_Position.x + t * c_Vel.x;
	newPosition.y = a_Position.y + sin(yTime);
	newPosition.z = a_Position.z;
	newPosition.w = 1;
	gl_Position = newPosition;

	/*float time;
	time = u_Time - a_emitTime;

	if(time < 0.f){
		gl_Position = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else{
		time = a_lifeTime * fract(time / a_lifeTime);

		vec4 newPosition;
		newPosition.xyz = a_Position 
						+ a_Velocity * time
						+ 0.5 * c_Gravity * time * time;
		newPosition.w = 1;
		gl_Position = newPosition;
	}*/
}
