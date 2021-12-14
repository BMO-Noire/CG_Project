#pragma warning(disable : 4996)

#include "pch.h"
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include <corecrt_wtime.h>
#include "robot.h"

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes into the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}

GLuint complie_shaders()
{
	GLchar* vertexsource, *fragmentsource;

	//--- 버텍스 세이더 읽어 저장하고 컴파일 하기
	vertexsource = filetobuf("vertex.glsl");
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexsource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return false;
	}

	//--- 프래그먼트 세이더 읽어 저장하고 컴파일하기
	fragmentsource = filetobuf("fregment.glsl");
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentsource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
		return false;
	}

	GLuint ShaderProgramID = glCreateProgram(); //--- 세이더 프로그램 만들기
	glAttachShader(ShaderProgramID, vertexShader); // 세이더 프로그램에 버텍스 세이더 붙이기
	glAttachShader(ShaderProgramID, fragmentShader); // 세이더 프로그램에 프래그먼트 세이더 붙이기
	glLinkProgram(ShaderProgramID); // 세이더 프로그램 링크하기
	glDeleteShader(vertexShader); // 세이더 프로그램에 링크하여 세이더 객체 자체는 삭제 가능
	glDeleteShader(fragmentShader);
	glGetProgramiv(ShaderProgramID, GL_LINK_STATUS, &result); // 세이더가 잘 연결되었는지 체크하기
	if (!result) {
		glGetProgramInfoLog(ShaderProgramID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program 연결 실패\n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(ShaderProgramID); //--- 만들어진 세이더 프로그램 사용하기
	// 여러 개의 프로그램 만들 수 있고, 특정 프로그램을 사용하려면
	// glUseProgram 함수를 호출하여 사용 할 특정 프로그램을 지정한다.
	// 사용하기 직전에 호출할 수 있다.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return ShaderProgramID;
}

std::random_device rd;
std::uniform_real_distribution<float> urd(0.0f, 1.0f);

GLvoid drawScene(GLvoid);
GLvoid Reshape(int w, int h);
void draw();

GLuint shaderID = 0;

std::vector<StageBox*> vecBoxes;

Robot* robot;
Robot2* robot2;
Stage* stage;
//StageBox* stagebox;

int howmany[9]{};
int mouse_prev_x = 0, mouse_prev_y = 0;
int mouse_dx = 0, mouse_dy = 0;

BOOL up = false;
BOOL down = false;
BOOL bleft = false;
BOOL bright = false;


bool d_mode = false;

void m_click(int button, int state, int x, int y) {

	mouse_dx += x - mouse_prev_x;
	mouse_dy += y - mouse_prev_y;

	mouse_prev_x = x;
	mouse_prev_y = y;

	glutPostRedisplay();

}

bool timer_stop = false;
int draw_mode = 0;
int draw_quar = 0;

float spin_self = 0.01;
float whole_spin_rad = 0.f;
float camera_spin = 0.f;
glm::vec3 camera_pos = glm::vec3(1.f);
float axis_x = 0.f, axis_y = 5.0f, axis_z = 15.0f;

bool r_onoff = false;

void key_input(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
		glutLeaveMainLoop();
		break;
	case 'a':
		camera_pos.x += 0.5f;
		break;
	case 'd':
		camera_pos.x -= 0.5f;
		break;
	case 's':
		camera_pos.z += 0.5f;
		break;
	case 'w':
		camera_pos.z -= 0.5f;
		break;
	case 'y':
		whole_spin_rad += -.1f;
		break;
	case 'Y':
		whole_spin_rad -= -.1f;
		break;
	case 'u':
		camera_spin += -.1f;
		break;
	case 'U':
		camera_spin -= -.1f;
		break;
	case 'i':
		robot->forward();
		robot2->forward();
		break;
	case 'k':
		robot->backward();
		robot2->backward();
		break;
	case 'j':
		robot->left();
		robot2->left();
		break;
	case 'l':
		robot->right();
		robot2->right();
		break;
	case 'o':
		robot->jump();
		robot2->jump();
		break;
	case 'c':
		camera_pos.x = axis_x;
		camera_pos.y = axis_y;
		camera_pos.z = axis_z;
		whole_spin_rad = 0.f;
		camera_spin = 0.f;
		robot->reset();
		robot2->reset();
		break;
	case 't':
		for (auto& box : vecBoxes)
		{
			if(!box->GetBottomCheck())
				box->left();
		}
		break;
	case 'g':
		for (auto& box : vecBoxes)
		{
			if (!box->GetBottomCheck())
				box->right();
		}
		break;
	case 'f':
		for (auto& box : vecBoxes)
		{
			if (!box->GetBottomCheck())
				box->backward();
		}
		break;
	case 'h':
		for (auto& box : vecBoxes)
		{
			if (!box->GetBottomCheck())
				box->forward();
		}
		break;
	}

	

	
	
}


void menu_func(int value) {
	switch (value) {
	case 0:
	{
		static bool wire_mode = false;
		wire_mode = !wire_mode;
		if (wire_mode) {
			draw_mode = GLU_LINE;
		}
		else {
			draw_mode = GLU_FILL;
		}
	}
	}
}

int pullout(int num, int th) {
	return (int(num / pow(10, th - 1)) % 10);
}

void timer(int value) {
	if (!timer_stop) {
		value++;
	}

	bool checkAllBox = false;

	for (auto& box : vecBoxes)
	{
		checkAllBox = box->GetBottomCheck();
	}
	if (vecBoxes.size() == 0)
		checkAllBox = true;

	if (checkAllBox)
	{
		StageBox* stagebox = new StageBox;
		vecBoxes.emplace_back(stagebox);
	}
	

	stage->update();
	robot->update();
	robot2->update();
	for (auto& box : vecBoxes)
	{
		box->update();
	}
	
	draw();
	glutTimerFunc(1, timer, value);
}

extern int loadObj(const char*);

void main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example1");
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cerr << "Unable to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
		std::cout << "GLEW Initialized\n";

	std::cout<<"명령어" << std::endl;
	std::cout << "i/j/k/l 캐릭터 움직임" << std::endl;
	std::cout << "o 캐릭터 점프" << std::endl;
	std::cout << "w/a/s/d 카메라이동" << std::endl;
	std::cout << "y/Y 화면 중점 회전" << std::endl;
	std::cout << "(추가)u/U 카메라 중점 회전" << std::endl;
	std::cout << "c 초기화" << std::endl;
	std::cout << "q 프로그램종료" << std::endl;

	shaderID = complie_shaders(); // 세이더 프로그램

	
	stage = new Stage;
	//stagebox = new StageBox;
	robot = new Robot;
	robot2 = new Robot2;

	//camera_pos.x = axis_x;
	//camera_pos.y = axis_y;
	//camera_pos.z = axis_z;
	camera_pos.x = axis_x;
	camera_pos.y = 15.f;
	camera_pos.z = 45.f;
	whole_spin_rad = 0.f;
		
	glutMouseFunc(m_click);
	glutKeyboardFunc(key_input);
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutTimerFunc(1, timer, 0);
	int menu = glutCreateMenu(menu_func);
	glutAddMenuEntry("Draw Mode", 0);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();
}

void set_color(float r, float g, float b) {
	unsigned int color_location = glGetUniformLocation(shaderID, "uniform_color");
	glUniform3f(color_location, r, g, b);
}

void Draw_Axis()
{
	float length = 900.f;
	set_color(1, 0, 0);


	//X-axis (red)
	glBegin(GL_LINES);
	glVertex3f(-length, 0, 0);
	glVertex3f(length, 0, 0);
	glEnd();

	set_color(0, 1, 0);
	//Y-axis (green)
	glBegin(GL_LINES);
	glVertex3f(0, -length, 0);
	glVertex3f(0, length, 0);
	glEnd();

	//Z-axis (blue)
	set_color(0, 0, 1);
	glBegin(GL_LINES);
	glVertex3f(0, 0, -length);
	glVertex3f(0, 0, length);
	glEnd();
}

GLvoid drawScene() {
	draw();
}

GLvoid Reshape(int w, int h)
{
	glViewport(0, 0, w, h);
}

void draw() {
	glUseProgram(shaderID);
	glPolygonMode(GL_FRONT_AND_BACK, GL_DEPTH_TEST);
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	

	glm::vec3 cameraPos = glm::rotate(glm::mat4(1.0f), whole_spin_rad, glm::vec3(0, 1, 0)) * glm::vec4(camera_pos, 1.f);
	glm::vec3 cameraDirection = glm::vec3(-cameraPos.x, -cameraPos.y, -cameraPos.z);
	glm::vec4 temp = glm::rotate(glm::mat4(1.0f), camera_spin, glm::vec3(0, 1, 0)) * glm::vec4(cameraDirection, 1.f);
	cameraDirection = glm::vec3(temp.x / temp.w + cameraPos.x, temp.y / temp.w + cameraPos.y, temp.z / temp.w + cameraPos.z);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	unsigned int viewLocation = glGetUniformLocation(shaderID, "viewTransform");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));


	glm::mat4 projection = glm::mat4(1.0f);
	projection = glm::perspective(glm::radians(45.0f), (float)1.0f / (float)1.0f, 0.1f, 100.0f);
	unsigned int projectionLocation = glGetUniformLocation(shaderID, "projectionTransform");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


	unsigned int axis_location = glGetUniformLocation(shaderID, "modelTransform");
	glm::mat4 model = glm::mat4(1.0f); // 초기화
	glUniformMatrix4fv(axis_location, 1, GL_FALSE, glm::value_ptr(model));
	Draw_Axis();
	
	unsigned int modelLocation = glGetUniformLocation(shaderID, "modelTransform");
	model = glm::translate(model, glm::vec3(0, 0, 0));
	model = glm::rotate(model, .0f, glm::vec3(0, 1, 0));
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model)); //--- modelTransform 변수에 변환 값 적용하기
	
	set_color(0.5, 0.4, 0.1);
	stage->draw(shaderID, model);
	robot->draw(shaderID, model);
	//tetris->draw();
	//robot2->draw(shaderID, model);
	for (auto& box : vecBoxes)
	{
		box->draw(shaderID, model);
	}
	

	unsigned int color_location = glGetUniformLocation(shaderID, "uniform_color");
	glUniform3f(color_location, 0.5f, 0.1f, 0.1f);


	glutSwapBuffers();
}
