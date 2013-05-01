#version 330 core

layout(location = 0) in vec3 verPosition;
layout(location = 1) in vec2 verUV;

uniform mat4 PVW;
out vec2 uv;

void main(){
	gl_Position = PVW*vec4(verPosition, 1.0f);
	uv = verUV;
}
