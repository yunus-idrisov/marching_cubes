#include <iostream>
#include <string>
#include <fstream>
#include <GL/glfw.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HelperFuns.h"
#include "MarchingCubes_LookupTables.h"
using namespace std;

const GLfloat SceneParameters::PI;
SceneParameters gSceneParams;
static void AddGridToScene(GLfloat gridCellSize, GLint CellCount);
static void AddCube();

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

	// Сетка.
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	// Обычный режим.
	//glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0f, 1.0f);

	glClearColor(0.1, 0.0, 0.35, 1.0f);
	glClearDepth(1.0f);

	gSceneParams.winWidth = winWidth;
	gSceneParams.winHeight = winHeight;
	return 1;
}

int InitScene(){
	glfwSetWindowTitle("Marching Cubes");
	glfwEnable(GLFW_STICKY_KEYS);
	gSceneParams.ratio = gSceneParams.winWidth/(float)gSceneParams.winHeight;
	gSceneParams.cam = Camera();
	// Создаём VertexArray.
	glGenVertexArrays(1, &gSceneParams.vertexArrayID);
	glBindVertexArray(gSceneParams.vertexArrayID);

	// Создаём шейдер.
	gSceneParams.shader[0] = CreateProgram("grid.vs", "grid.fs");
	if( gSceneParams.shader[0] == 0 ){
		cerr <<  "Failed to create shader." << endl;
		return -1;
	}

	gSceneParams.gridShaderPVWRef = glGetUniformLocation(gSceneParams.shader[0], "mPVW");

	AddGridToScene(1.0f, 10);
	AddCube();
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

	if( glfwGetKey(GLFW_KEY_LEFT) == GLFW_PRESS )
		gSceneParams.cam.SetAlpha( gSceneParams.cam.GetAlpha() + 3*deltaTime );

	if( glfwGetKey(GLFW_KEY_RIGHT) == GLFW_PRESS )
		gSceneParams.cam.SetAlpha( gSceneParams.cam.GetAlpha() - 3*deltaTime );

	if( glfwGetKey(GLFW_KEY_UP) == GLFW_PRESS )
		if( (gSceneParams.cam.GetBeta() + 3.0f*deltaTime) < gSceneParams.PI/2.0f )
			gSceneParams.cam.SetBeta( gSceneParams.cam.GetBeta() + 3*deltaTime );

	if( glfwGetKey(GLFW_KEY_DOWN) == GLFW_PRESS )
		if( (gSceneParams.cam.GetBeta() - 3.0f*deltaTime) > -gSceneParams.PI/2.0f )
			gSceneParams.cam.SetBeta( gSceneParams.cam.GetBeta() - 3*deltaTime );

	gSceneParams.cam.CalculatePV();

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
	
	// Сначала рендерим сетку.
	glUseProgram( gSceneParams.shader[0] );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, gSceneParams.VerBuffer[0]);
	glUniformMatrix4fv( gSceneParams.gridShaderPVWRef, 1, GL_TRUE, gSceneParams.cam.GetPV().m );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Pos_Col), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Pos_Col), (void*)(sizeof(GLfloat)*3));
	glDrawArrays(GL_LINES, 0, gSceneParams.BufferVerCount[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram( 0 );

	// Теперь кубы.
	glUseProgram( gSceneParams.shader[0] );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, gSceneParams.VerBuffer[1]);
	glUniformMatrix4fv( gSceneParams.gridShaderPVWRef, 1, GL_TRUE, gSceneParams.cam.GetPV().m );
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Pos_Col), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_Pos_Col), (void*)(sizeof(GLfloat)*3));
	glDrawArrays(GL_TRIANGLES, 0, gSceneParams.BufferVerCount[1]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram( 0 );
}

static void AddGridToScene(GLfloat gridCellSize, GLint CellCount){
	if( CellCount < 2 )
		CellCount = 2;
	if( (CellCount % 2) != 0 )
		CellCount++;
	GLint cc = CellCount;
	GLuint verCount = 2*2*(cc+1);

	Vertex_Pos_Col gridVers[verCount];
	for(int i = 0; i < verCount; i++){
		gridVers[i].pos.y = 0.0f;

		gridVers[i].color.x = 0.3f;
		gridVers[i].color.y = 0.3f;
		gridVers[i].color.z = 0.3f;
	}

	GLfloat t = -(cc/2)*gridCellSize;
	GLuint v = 0;
	for(int i = 0; i <= cc; i++){
		gridVers[v].pos.x = t + i*gridCellSize;
		gridVers[v].pos.z = t;
		v++;
		gridVers[v].pos.x = t + i*gridCellSize;
		gridVers[v].pos.z = -t;
		v++;
	}

	for(int i = 0; i <= cc; i++){
		gridVers[v].pos.x = t; 
		gridVers[v].pos.z = t + i*gridCellSize;
		v++;             
		gridVers[v].pos.x = -t;
		gridVers[v].pos.z =  t + i*gridCellSize;
		v++;
	}

	gridVers[cc].color.x = 0.0f;
	gridVers[cc].color.y = 0.0f;
	gridVers[cc].color.z = 1.0f;
	gridVers[cc+1].color.x = 0.0f;
	gridVers[cc+1].color.y = 0.0f;
	gridVers[cc+1].color.z = 1.0f;

	gridVers[cc + verCount/2].color.x = 1.0f;
	gridVers[cc + verCount/2].color.y = 0.0f;
	gridVers[cc + verCount/2].color.z = 0.0f;
	gridVers[cc + verCount/2+1].color.x = 1.0f;
	gridVers[cc + verCount/2+1].color.y = 0.0f;
	gridVers[cc + verCount/2+1].color.z = 0.0f;

	glGenBuffers(1, &gSceneParams.VerBuffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, gSceneParams.VerBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Pos_Col)*verCount, gridVers, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	gSceneParams.BufferVerCount[0] = verCount;
}

static void AddCube(){
	GLuint edge_to_points[12][2] = { {0,1}, {1,2}, {2,3}, {3,0},
									 {4,5}, {5,6}, {6,7}, {7,4},
									 {0,4}, {1,5}, {2,6}, {3,7} };

	GLfloat d = 1.0f;
	Vertex_Pos_Col vertices[8] = {
		{ -d,-d, d, 1,0,1 },
		{ -d, d, d, 0,1,0 },
		{  d, d, d, 0,1,0 },
		{  d,-d, d, 1,0,1 },
		{ -d,-d,-d, 1,0,1 },
		{ -d, d,-d, 0,1,0 },
		{  d, d,-d, 0,1,0 },
		{  d,-d,-d, 1,0,1 },
	};

	unsigned char cube_case = 193;
	
	GLuint triNum = case_to_polygon_num[cube_case];
	GLuint verNum = triNum*3;
	Vertex_Pos_Col triangles[verNum];
	GLuint v = 0;
	for(int i = 0; i < triNum; i++){
		GLuint e[3] = { case_edges[cube_case][i][0],
						case_edges[cube_case][i][1],
						case_edges[cube_case][i][2] };

		for(int j = 0; j < 3; j++){
			GLuint v1 = edge_to_points[e[j]][0];
			GLuint v2 = edge_to_points[e[j]][1];

			triangles[v].pos.x = (vertices[v1].pos.x + vertices[v2].pos.x)/2.0f;
			triangles[v].pos.y = (vertices[v1].pos.y + vertices[v2].pos.y)/2.0f;
			triangles[v].pos.z = (vertices[v1].pos.z + vertices[v2].pos.z)/2.0f;
			triangles[v].color.x = (vertices[v1].color.x + vertices[v2].color.x)/2.0f;
			triangles[v].color.y = (vertices[v1].color.y + vertices[v2].color.y)/2.0f;
			triangles[v].color.z = (vertices[v1].color.z + vertices[v2].color.z)/2.0f;
			v++;
		}
	}

	glGenBuffers(1, &gSceneParams.VerBuffer[1]);
	glBindBuffer(GL_ARRAY_BUFFER, gSceneParams.VerBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex_Pos_Col)*verNum, triangles, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	gSceneParams.BufferVerCount[1] = verNum;
}

void ReleaseSceneResources(){
	glDeleteVertexArrays(1, &gSceneParams.vertexArrayID);

	glDeleteBuffers(1, &gSceneParams.VerBuffer[0]);
	glDeleteBuffers(1, &gSceneParams.VerBuffer[1]);
	glDeleteProgram(gSceneParams.shader[0]);
}
