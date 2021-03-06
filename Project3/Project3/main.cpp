#pragma warning(disable : 4996)
#pragma comment(lib, "winmm.lib")

#define SOUND_BGM		L"bgm.wav"
#define SOUND_JUMP		"jump.wav"
#define SOUND_CUBE		L"cube.wav"

#include "pch.h"
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include <corecrt_wtime.h>
#include "robot.h"
#include <mmsystem.h>
#include <Digitalv.h>
MCI_OPEN_PARMS openBgm;
MCI_PLAY_PARMS playBgm;
MCI_OPEN_PARMS openShuffleSound;
MCI_PLAY_PARMS playShuffleSound;

void playingBgm(void);
void playingShuffleSound(void);
int dwID;
bool play= true;
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
	GLchar* vertexsource, * fragmentsource;

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
StageBox* shadowbox;


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

//조명
float light_spin = 0;
float light_power = 0.1f;
glm::vec3 light_pos = glm::vec3(0, 20, 0);
glm::vec3 light_color = glm::vec3(1, 1, 1);

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
		if (robot != nullptr)
		{
			robot->forward();
		}
		break;
	case 'k':
		if (robot != nullptr)
		{
			robot->backward();
		}
		break;
	case 'j':
		if (robot != nullptr)
		{
			robot->left();
		}
		break;
	case 'l':
		if (robot != nullptr)
		{
			robot->right();
		}
		break;
	case 'o':
		robot->jump();
		PlaySound(TEXT(SOUND_JUMP), NULL, SND_ASYNC | SND_ALIAS);
		break;
	case 'c':
		camera_pos.x = axis_x;
		camera_pos.y = axis_y;
		camera_pos.z = axis_z;
		whole_spin_rad = 0.f;
		camera_spin = 0.f;
		vecBoxes.clear();
		robot = new Robot;
		break;
	case 't':
		for (auto& box : vecBoxes)
		{
			if (!box->GetBottomCheck())
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

	int howmany[9]{};

	for (auto& box : vecBoxes)
	{
		checkAllBox = box->GetBottomCheck();
	}
	if (vecBoxes.size() == 0)
		checkAllBox = true;


	stage->update();

	robot2->update();


	for (auto& box : vecBoxes)
	{
		box->update(howmany);

		if (box->GetBottomCheck())
		{
			glm::vec3 vpos{};
			vpos = box->GetPos();

			if (vpos.x == 0.f)
			{
				if (vpos.z == 0.f)
				{
					howmany[4] += 1;
				}
				else if (vpos.z == -2.f)
				{
					howmany[7] += 1;
				}
				else if (vpos.z == 2.f)
				{
					howmany[1] += 1;
				}
			}
			else if (vpos.x == -2.f)
			{
				if (vpos.z == 0.f)
				{
					howmany[3] += 1;
				}
				else if (vpos.z == -2.f)
				{
					howmany[6] += 1;
				}
				else if (vpos.z == 2.f)
				{
					howmany[0] += 1;
				}
			}
			else if (vpos.x == 2.f)
			{
				if (vpos.z == 0.f)
				{
					howmany[5] += 1;
				}
				else if (vpos.z == -2.f)
				{
					howmany[8] += 1;
				}
				else if (vpos.z == 2.f)
				{
					howmany[2] += 1;
				}
			}
		}
		else
		{
			if (robot != nullptr)
				robot->update(box->GetPos(), howmany);
			shadowbox->SetPos(box->GetPos(), howmany);
		}
	}


	if (checkAllBox)
	{
		StageBox* stagebox = new StageBox;
		float min = -100;

		for (int i = 0; i < 9; ++i)
		{
			if (howmany[i] > min)
				min = howmany[i];
		}
		stagebox->SetPos(glm::vec3(0.f, 10.f + (min * 2.f), 0.f));

		vecBoxes.emplace_back(stagebox);
	}

	if (robot != nullptr)
	{
		if (robot->GetDeadCheck())
		{
			delete robot;
			robot = nullptr;
		}
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


	shaderID = complie_shaders(); // 세이더 프로그램


	playingBgm();
	//PlaySound(TEXT(SOUND_BGM), NULL, SND_ASYNC | SND_LOOP);

	stage = new Stage;
	//stagebox = new StageBox;
	robot = new Robot;
	robot2 = new Robot2;
	shadowbox = new StageBox;
	shadowbox->SetAlpha(0.5f);
	//camera_pos.x = axis_x;
	//camera_pos.y = axis_y;
	//camera_pos.z = axis_z;
	camera_pos.x = axis_x;
	camera_pos.y = 15.f;
	camera_pos.z = 15.f;
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

void set_color(float r, float g, float b, float a) {
	unsigned int color_location = glGetUniformLocation(shaderID, "uniform_color");
	glUniform4f(color_location, r, g, b, a);
	//glColor4f
}

void Draw_Axis()
{
	float length = 900.f;
	set_color(1, 0, 0, 1);

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
	glClearColor(0.5f, 0.5f, 0.5f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

	set_color(0.5, 0.4, 0.1, 1);
	stage->draw(shaderID, model);
	if (robot != nullptr)
		robot->draw(shaderID, model);
	//tetris->draw();
	//robot2->draw(shaderID, model);
	shadowbox->draw(shaderID, model);

	for (auto& box : vecBoxes)
	{
		box->draw(shaderID, model);
	}

	// 조명
	glUseProgram(shaderID);
	int lightPosLocation = glGetUniformLocation(shaderID, "lightPos");
	glm::vec3 temp_light_pos = glm::rotate(glm::mat4(1.f), light_spin, glm::vec3(0, 1, 0)) * glm::vec4(light_pos, 1.f);
	glUniform3fv(lightPosLocation, 1, glm::value_ptr(temp_light_pos));

	int lightColorLocation = glGetUniformLocation(shaderID, "lightColor");
	glUniform3fv(lightColorLocation, 1, glm::value_ptr(light_color));

	int lightPowerLocation = glGetUniformLocation(shaderID, "ambientLight");
	glUniform1f(lightPowerLocation, light_power);

	int camera_pos_Location = glGetUniformLocation(shaderID, "view_pos");
	glUniform3f(camera_pos_Location, cameraPos.x, cameraPos.y, cameraPos.z);


	unsigned int color_location = glGetUniformLocation(shaderID, "uniform_color");
	glUniform3f(color_location, 0.5f, 0.1f, 0.1f);


	glutSwapBuffers();
}


void playingBgm(void) {
	openBgm.lpstrElementName = SOUND_BGM; //파일 오픈
	openBgm.lpstrDeviceType = L"mpegvideo"; //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openBgm);
	dwID = openBgm.wDeviceID;
	mciSendCommand(dwID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&openBgm); //음악 반복 재생
}
void playingShuffleSound(void) {
	openShuffleSound.lpstrElementName = SOUND_CUBE; //파일 오픈
	openShuffleSound.lpstrDeviceType = L"mpegvideo"; //mp3 형식
	mciSendCommand(0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE, (DWORD)(LPVOID)&openShuffleSound);
	dwID = openShuffleSound.wDeviceID;

	if(play)
	{
		mciSendCommand(dwID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&openShuffleSound); //음악을 한 번 재생
		play = false;
	}
	else
	{
		mciSendCommand(dwID, MCI_SEEK, MCI_SEEK_TO_START, (DWORD)(LPVOID)NULL); //음원 재생 위치를 처음으로 초기화
		play = true;
	}

}
