#version 330

out vec3 Color;
in vec3 NorW;

void main(){
	vec3 LightDir = vec3( 4.0f, -1.0f, -4.0f);
	LightDir = normalize( LightDir );
	vec3 n = normalize( NorW );

	vec3 difColor = vec3(0,1,0);
	float difComp = max( dot(-LightDir, n), 0 );
	Color = difColor*difComp;
}
