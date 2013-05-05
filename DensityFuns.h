#ifndef DENSITYFUNS_H
#define DENSITYFUNS_H
#include <GL/gl.h>
#include "Math.h"

// Город.
// Если последний параметр функции DensityFun() true, то
// происходит генерация случайных значений, на основе которых
// в дальнейшем и происходит генерация плотности.
GLfloat Density_Fun(const Vector3f& p,
					GLuint xnum, GLuint ynum, GLuint znum, 
					GLfloat cubeSize,
					bool toRand);

#endif // DENSITYFUNS_H
