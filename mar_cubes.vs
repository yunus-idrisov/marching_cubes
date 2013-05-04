#version 330

layout(location = 0) in vec3 PosL;
layout(location = 1) in vec3 NorL;

uniform mat4 mPVW;
uniform mat4 mW;

out vec3 NorW;

void main(){
	gl_Position = mPVW*vec4(PosL, 1.0f);
	NorW = (mW*vec4(NorL, 0.0f)).xyz;
}
