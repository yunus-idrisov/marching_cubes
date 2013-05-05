#version 330

out vec3 Color;
in vec3 NorW;
in vec3 PosW;
uniform vec3 Eye;

void main(){
	vec3 LightDir = vec3(-0.57, -0.57, -0.57);
	vec3 n = normalize( NorW );

	vec3 difColor = vec3(1,1,1);
	float difComp = max( dot(-LightDir, n), 0 );
	Color = difColor*difComp;

	vec3 fogColor = vec3(0.1, 0.0, 0.35);
	float fogStart = 0.5f;
	float fogEnd = 30.0f;
	float dis = distance(Eye, PosW);
	float s = clamp((dis - fogStart)/fogEnd, 0, 1);
	Color = Color + s*(fogColor - Color);
}
