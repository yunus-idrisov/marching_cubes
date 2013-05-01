#include <iostream>
#include <string>
#include <fstream>
#include <GL/glfw.h>
#include <stdio.h>
#include <stdlib.h>
#include "HelperFuns.h"
using namespace std;

SceneParameters gSceneParams;

int InitGL(int winWidth, int winHeight, int glver_major, int glver_minor){
	if( !glfwInit() ){
		cerr << "Failed to initialize GLFW." << endl;
		return -1;
	}
	// Параметры OpenGL.
	// Antialiasing.
	//glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);// ??
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, glver_major);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, glver_minor);
	glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// w, h, rbits, gbits, bbits, abits, depth bits, stencil bits, fullscreen/windowed.
	// Если 0, то GLFW выберет значение по умолчанию или запретит.
	if( !glfwOpenWindow(winWidth, winHeight, 0,0,0,0, 32, 0, GLFW_WINDOW) ){
		cerr << "Failed to open GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glViewport(0, 0, winWidth, winHeight);
	glClearColor(0.1, 0.0, 0.35, 0);

	gSceneParams.winWidth = winWidth;
	gSceneParams.winHeight = winHeight;
	return 1;
}

int InitScene(){
	glfwSetWindowTitle("Marching Cubes");
	glfwEnable(GLFW_STICKY_KEYS);
	gSceneParams.ratio = gSceneParams.winWidth/(float)gSceneParams.winHeight;

	// Создаём VertexArray.
	glGenVertexArrays(1, &gSceneParams.vertexArrayID);
	glBindVertexArray(gSceneParams.vertexArrayID);

	// Создаём шейдер.
	//gSceneParams.shader = CreateProgram("vertex_shader.vs", "fragment_shader.fs");
	//if( gSceneParams.shader == 0 ){
		//cerr <<  "Failed to create shader." << endl;
		//return -1;
	//}

	// Загружаем текстуру.
	//gSceneParams.texture = LoadBMP("diffuse.bmp");
	//if( gSceneParams.texture == 0 ){
		//cerr << "Failed to load texture" << endl;
		//return -1;
	//}

	// Camera setup.
	Mat4x4 W, V, P;
	Mat4x4Identity(&W);
	Vector3f eye = {3, 3, 3}, target = {0,0,0}, up = {0,1,0};
	gSceneParams.Eye = eye;
	gSceneParams.Target = target;
	gSceneParams.Up = up;
	Mat4x4View(&V, &eye, &target, &up);
	GLfloat r = gSceneParams.ratio;
	Mat4x4Pers(&P, 3.14159f/4, r, 0.1f, 10.0f);
	Mat4x4Mult(&gSceneParams.PVW, &V, &W);
	Mat4x4Mult(&gSceneParams.PVW, &P, &gSceneParams.PVW);

	return 1;
}

GLuint CreateProgram(const char *vertex_shader_path, const char *fragment_shader_path){
	// Сначала создаём вершинный шейдер.

	// Считываем код вер. шейдера.
	string verShaderCode("");
	ifstream verShaderStream(vertex_shader_path);
	if( verShaderStream.is_open() ){
		string line("");
		while( getline(verShaderStream, line) )
			verShaderCode += line + "\n";
		verShaderStream.close();
	} else{
		cerr << "Error: Cann't open \"" << vertex_shader_path << "\" file." << endl;
		return 0;
	}

	// Компилируем вер. шейдер.
	GLuint verShader = glCreateShader(GL_VERTEX_SHADER);
	const char *verShaderSource = verShaderCode.c_str();
	glShaderSource(verShader, 1, &verShaderSource, NULL);
	glCompileShader(verShader);

	// Проверём не возникли ли ошибки во время компиляции.
	GLint result;
	int infoLogLength;
	glGetShaderiv(verShader, GL_COMPILE_STATUS, &result);
	if( result == GL_FALSE ){
		glGetShaderiv(verShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *errMessage = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(verShader, infoLogLength, NULL, errMessage);
		errMessage[infoLogLength] = '\0';
		cerr << "Error in \"" << vertex_shader_path << "\":" << endl;
		cerr << errMessage << endl;
		delete [] errMessage;
		return 0;
	}

	// Теперь создаём фрагментный шейдер.
	// Считываем код фраг. шейдера.
	string fragShaderCode("");
	ifstream fragShaderStream(fragment_shader_path);
	if( fragShaderStream.is_open() ){
		string line("");
		while( getline(fragShaderStream, line) )
			fragShaderCode += line + "\n";
		fragShaderStream.close();
	} else{
		cerr << "Error: Cann't open \"" << fragment_shader_path << "\" file." << endl;
		return 0;
	}

	// Компилируем фраг. шейдер.
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char *fragShaderSource = fragShaderCode.c_str();
	glShaderSource(fragShader, 1, &fragShaderSource, NULL);
	glCompileShader(fragShader);

	// Проверём не возникли ли ошибки во время компиляции.
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
	if( result == GL_FALSE ){
		glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *errMessage = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(fragShader, infoLogLength, NULL, errMessage);
		errMessage[infoLogLength] = '\0';
		cerr << "Error in \"" << fragment_shader_path << "\":" << endl;
		cerr << errMessage << endl;
		delete [] errMessage;
		return 0;
	}

	// Создаём шейдерную программу.
	GLuint program = glCreateProgram();
	glAttachShader(program, verShader);
	glAttachShader(program, fragShader);
	glLinkProgram(program);

	// Проверём не возникли ли ошибки во время компоновки.
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	if( result == GL_FALSE ){
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *errMessage = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(fragShader, infoLogLength, NULL, errMessage);
		errMessage[infoLogLength] = '\0';
		cerr << "Error in \"" << fragment_shader_path << "\":" << endl;
		cerr << errMessage << endl;
		delete [] errMessage;
		return 0;
	}

	glDeleteShader(verShader);
	glDeleteShader(fragShader);
	return program;
}

GLuint LoadBMP(const char* filePath){
	FILE* file = fopen(filePath, "rb");
	if( file == NULL ){
		printf("Image could not be opened\n");
		return 0;
	}

	unsigned char  bmpHeader[54];
	unsigned int   dataPos;
	unsigned int   imgWidth, imgHeight;
	unsigned int   imgSize;// imgWidth * imgHeight * 3
	unsigned char* data;// массив пикселов рисунка.

	if( fread(bmpHeader, 1, 54, file) != 54 ){
		printf("Not a correct bmp file.\n");
		return 0;
	}

	// Первыми двумя символа bmp файла должны быть "BM".
	if( bmpHeader[0] != 'B' || bmpHeader[1] != 'M' ){
		printf("Not a correct bmp file.\n");
		return 0;
	}
	dataPos   =	*(int*)&(bmpHeader[0x0A]);
	imgSize = *(int*)&(bmpHeader[0x22]);
	imgWidth  = *(int*)&(bmpHeader[0x12]);
	imgHeight = *(int*)&(bmpHeader[0x16]);

	// Страховка.
	if( imgSize == 0 )
		imgSize = imgWidth*imgHeight*4;
	if( dataPos == 0 )
		dataPos = 54;

	// Считываем данные.
	data = new unsigned char[imgSize];
	fread(data, 1, imgSize, file);

	// Создаём и заполняем текстуру данными.
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// Сгенерировать mipmap для текущей текстуры(т.е. для которой вызвана glBindTexuter(...)).
	glGenerateMipmap(GL_TEXTURE_2D);

	delete [] data;
	fclose(file);
	return texture;
}

void UpdateScene(){
	static double lastTime = glfwGetTime();
	GLfloat deltaTime = glfwGetTime() - lastTime;

	// Update code here.

	lastTime = glfwGetTime();

	// Вычисление FPS.
	static double startTime = glfwGetTime();
	static int FPS = 0;
	if( glfwGetTime() - startTime > 1.0f ){
		cout << FPS << endl;
		startTime = glfwGetTime();
		FPS = 0;
	}
	FPS++;
}

void RenderScene(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUseProgram(gSceneParams.shader);
	
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void ReleaseSceneResources(){
	glDeleteVertexArrays(1, &gSceneParams.vertexArrayID);
	//glDeleteProgram(gSceneParams.shader);
}
