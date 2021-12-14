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
	float alpha;

	bool bottomCheck;

public:
	StageBox();
	~StageBox();


	void draw(GLuint shader, glm::mat4 out);
	void update();
	void update(int* many);
	void check_bottom();
	void right();
	void left();
	void forward();
	void backward();

	void SetPos(glm::vec3 _pos) { pos = _pos; };
	void SetPos(glm::vec3 _pos, int* many);
	void SetAlpha(float _alpha) { alpha = _alpha; };

	glm::vec3 GetPos() { return pos; };
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

	bool deadcheck;
	bool collisioncheck;

	void draw_head(GLuint shaderID, glm::mat4 out);
	void draw_arm(GLuint shaderID, glm::mat4 out);
	void draw_leg(GLuint shaderID, glm::mat4 out);

	void CheckCollision(glm::vec3 _pos, int* many);

public:
	Robot();
	~Robot();

	void update(glm::vec3 _pos, int* many);
	void draw(GLuint shader, glm::mat4 out);

	void jump();
	void right();
	void left();
	void forward();
	void backward();
	glm::vec3 pos2();
	void reset();

	bool GetDeadCheck() { return deadcheck; };
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
