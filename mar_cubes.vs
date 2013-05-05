#version 330

layout(location = 0) in vec3 PosL;
layout(location = 1) in vec3 NorL;

uniform mat4 mPVW;
uniform mat4 mW;
uniform vec3 Eye;

out vec3 NorW;
out vec3 PosW;

void main(){
	gl_Position = mPVW*vec4(PosL, 1.0f);
	PosW = (mW*vec4(PosL,1.0f)).xyz;
	NorW = (mW*vec4(NorL, 0.0f)).xyz;
}
