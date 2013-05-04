#ifndef DENSITYFUNS_H
#define DENSITYFUNS_H
#include <GL/gl.h>
#include "Math.h"

// Сфера.
GLfloat Density_Fun1(const Vector3f& p,
					 GLuint xnum, GLuint ynum, GLuint znum, 
					 GLfloat cubeSize);

// Отдаленно напоминает сыр.
GLfloat Density_Fun2(const Vector3f& p,
					 GLuint xnum, GLuint ynum, GLuint znum, 
					 GLfloat cubeSize);

#endif // DENSITYFUNS_H
