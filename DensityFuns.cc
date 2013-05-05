#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include "DensityFuns.h"
using namespace std;

static const GLuint planeSize = 32;

static GLfloat up[planeSize][planeSize];
static GLfloat bottom[planeSize][planeSize];

GLfloat Density_Fun(const Vector3f& p,
					GLuint xnum, GLuint ynum, GLuint znum, 
					GLfloat cubeSize,
					bool toRand)
{
	if( toRand ){
		srand(time(0));
		for(int i = 0; i < planeSize; i++){
			for(int j = 0; j < planeSize; j++){
				up[i][j] = rand()/float(RAND_MAX) - 1.1f;
				bottom[i][j] = rand()/float(RAND_MAX) + 0.1f;
			}
		}
		return 0.0f;
	}

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

	if( p.y > -0.2f && p.y < 0.0f )
		return 1.0f;
	if( p.y < -0.2f )
		return -1.0f;

	GLuint xc = (p.x + xnum*cubeSize/2.0f)/(xnum*cubeSize)*planeSize;
	GLuint yc = (p.z + znum*cubeSize/2.0f)/(znum*cubeSize)*planeSize;
	GLfloat l = up[xc][yc];
	GLfloat r = bottom[xc][yc];
	GLfloat h = ynum*cubeSize/2.0f;
	if( p.y > (h + 2*h*r/(l - r)) )
		return -1.0f;
	else
		return 1.0f;
} 
