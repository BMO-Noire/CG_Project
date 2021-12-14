#pragma once

class Box {
	GLuint vao;
	GLuint points;
	GLuint points_index[2];

public:
	Box();
	~Box();

	void draw();
	void draw_frame();
	void set_size(float x, float y, float z);
	void set_size(glm::vec3 size);
};

class Stage {
	Box* boxes;

	glm::vec3 pos;

	float len;

	float curtain_up;
	float stage_curtain;
public:
	Stage();
	~Stage();

	void stage_up();
	void stage_down();
	
	void draw(GLuint shader, glm::mat4 out);
	void update();
};

class StageBox {
	Box* boxes;

	glm::vec3 pos;
	glm::vec3 velocity;

	float rot;
	float len;

	bool bottomCheck;

public:
	StageBox();
	~StageBox();

	
	void draw(GLuint shader, glm::mat4 out);
	void update();
	void check_bottom();
	void right();
	void left();
	void forward();
	void backward();

	bool GetBottomCheck() { return bottomCheck; };

};


class Robot
{
	Box* face;
	Box* body;
	Box* arm;
	Box* leg;

	float animation;
	float frame;

	glm::vec3 pos;
	glm::vec3 velocity;
	int sight;

	glm::vec3 head_size;
	glm::vec3 body_size;
	glm::vec3 leg_size;
	glm::vec3 arm_size;

	
	void draw_head(GLuint shaderID, glm::mat4 out);
	void draw_arm(GLuint shaderID, glm::mat4 out);
	void draw_leg(GLuint shaderID, glm::mat4 out);

public:
	Robot();
	~Robot();

	void update();
	void draw(GLuint shader, glm::mat4 out);

	void jump();
	void right();
	void left();
	void forward();
	void backward();
	glm::vec3 pos2();
	void reset();
};

class Robot2
{
	Box* face;
	Box* body;
	Box* arm;
	Box* leg;

	float animation;
	float frame;

	glm::vec3 pos;
	glm::vec3 velocity;
	int sight;

	glm::vec3 head_size;
	glm::vec3 body_size;
	glm::vec3 leg_size;
	glm::vec3 arm_size;


	void draw_head(GLuint shaderID, glm::mat4 out);
	void draw_arm(GLuint shaderID, glm::mat4 out);
	void draw_leg(GLuint shaderID, glm::mat4 out);

public:
	Robot2();
	~Robot2();

	void update();
	void draw(GLuint shader, glm::mat4 out);

	void jump();
	void right();
	void left();
	void forward();
	void backward();

	glm::vec3 pos2();

	void reset();
};
