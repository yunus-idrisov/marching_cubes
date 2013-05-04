#ifndef HELPERFUNS_H
#define HELPERFUNS_H
#include <GL/gl.h>
#include "Camera.h"
#include "Math.h"

struct SceneParameters{
	static const GLfloat PI = 3.14159f;
	static const GLfloat PI_OVER_TWO = 1.57080f;
	// Window
	GLuint		winWidth;
	GLuint		winHeight;
	GLfloat		ratio;		  // winWidth/winHeight

	GLuint		vertexArrayID;
	GLuint		shader[2];
	GLuint		gridShaderPVWRef;

	GLuint		mcShaderPVW_Ref;
	GLuint		mcShaderW_Ref;
	Mat4x4		mcW;

	GLuint		VerBuffer[2];
	GLuint		BufferVerCount[2];

	Camera cam;
};

extern SceneParameters gSceneParams;

// Функция для инициализации OpenGL и создания окна.
// При ошибке возвращается -1.
int InitGL(int winWidth, int winHeight, int glver_major, int glver_minor);

// Функция для начальной установки параметров приложения.
// При ошибке возвращается -1.
int InitScene();

// Функция CreateProgram создаёт шейдерную программу из 
// вершинного и фрагментного шейдеров.
// При ошибке возвращается 0.
GLuint CreateProgram(const char *vertex_shader_path, const char *fragment_shader_path);

// Функция для обновления сцены.
void UpdateScene();

// Функция для рендеринга всей сцены.
void RenderScene();

// Очистка.
void ReleaseSceneResources();

struct Vertex_Pos_Col{
	Vector3f pos;
	Vector3f color;
};

struct Vertex_Pos_Nor{
	Vector3f pos;
	Vector3f nor;
};

#endif // HELPERFUNS_H
