#include <stdlib.h>
#include <iostream>
#include "DensityFuns.h"
using namespace std;

GLfloat Density_Fun1(const Vector3f& p,
					 GLuint xnum, GLuint ynum, GLuint znum,
					 GLfloat cubeSize)
{
	GLfloat r = xnum*cubeSize/2;
	if( (p.x*p.x + p.y*p.y + p.z*p.z) <= r*r )
		return  1;
	else
		return -1;
}

const GLuint sphNum = 20;
static Vector3f spheres[sphNum];
static void initSpheres(GLuint xnum, GLuint ynum, GLuint znum, GLfloat cubeSize);

GLfloat Density_Fun2(const Vector3f& p,
					 GLuint xnum, GLuint ynum, GLuint znum, 
					 GLfloat cubeSize)
{
	if( (xnum*cubeSize/2.0f - cubeSize) < p.x )
		return -1;
	if( (xnum*cubeSize/-2.0f + cubeSize) > p.x )
		return -1;
	if( (ynum*cubeSize/2.0f - cubeSize) < p.y )
		return -1;
	if( (ynum*cubeSize/-2.0f + cubeSize) > p.y )
		return -1;
	if( (znum*cubeSize/2.0f - cubeSize) < p.z )
		return -1;
	if( (znum*cubeSize/-2.0f + cubeSize) > p.z )
		return -1;

	static bool isInit = false;
	if( !isInit ){
		srand(time(0));
		initSpheres(xnum, ynum, znum, cubeSize);
	}
	isInit = true;

	GLfloat r = xnum*cubeSize/5.0f;
	Vector3f c;
	for(int i = 0; i < sphNum; i++){
		c = spheres[i];
		Vec3Sub(c, c, p);
		if( Vec3Length(c) <= r )
			return -1;
	}
	return 1;
}

static void initSpheres(GLuint xnum, GLuint ynum, GLuint znum, GLfloat cubeSize){
	for(int i = 0; i < sphNum; i++){
		spheres[i].x = xnum*cubeSize/2*(rand()/float(RAND_MAX)*2.0f - 1.0f);
		spheres[i].y = ynum*cubeSize/2*(rand()/float(RAND_MAX)*2.0f - 1.0f);
		spheres[i].z = znum*cubeSize/2*(rand()/float(RAND_MAX)*2.0f - 1.0f);
	}
}
