#include "pch.h"
#include "robot.h"


extern void set_color(float, float, float, float);



Box::Box()
{
	vao = 0;

	float x = 0.5, y = .1f, z = .3f;
	float vPositionList[] = {
		x, y, z, // 1
		x, y, -z, // 2
		-x, y, -z, // 3
		-x, y, z, // 4

		x, -y, z, // 5
		x, -y, -z, // 
		-x, -y, -z,
		-x, -y, z,
	};

	unsigned int point_indexes[] = {
		0, 1, 2, 3, 7, 4, 5, 1,
	};

	unsigned int point_indexes_[] = {
		6, 7, 4, 5, 1, 2, 3, 7
	};

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &points);
	glBindBuffer(GL_ARRAY_BUFFER, points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(2, points_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(point_indexes), point_indexes, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(point_indexes_), point_indexes_, GL_STATIC_DRAW);

}

Box::~Box()
{
}

void Box::draw()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[0]);
	glDrawElements(GL_TRIANGLE_FAN, 8, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[1]);
	glDrawElements(GL_TRIANGLE_FAN, 8, GL_UNSIGNED_INT, 0);
}

void Box::draw_frame()
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[0]);
	glDrawElements(GL_LINE_LOOP, 8, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, points_index[1]);
	glDrawElements(GL_LINE_LOOP, 8, GL_UNSIGNED_INT, 0);
}

void Box::set_size(float x, float y, float z)
{
	float vPositionList[] = {
	x, y, z, // 1
	x, y, -z, // 2
	-x, y, -z, // 3
	-x, y, z, // 4

	x, -y, z, // 5
	x, -y, -z, // 
	-x, -y, -z,
	-x, -y, z,
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_DYNAMIC_DRAW);
}

void Box::set_size(glm::vec3 size)
{
	float x, y, z;
	x = size.x, y = size.y, z = size.z;
	float vPositionList[] = {
		x, y, z, // 1
		x, y, -z, // 2
		-x, y, -z, // 3
		-x, y, z, // 4

		x, -y, z, // 5
		x, -y, -z, // 
		-x, -y, -z,
		-x, -y, z,
	};

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vPositionList), vPositionList, GL_DYNAMIC_DRAW);
}


//


void Robot::draw_head(GLuint location, glm::mat4 out)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(out));
	set_color(1, 1, 1, 1);
	face[0].draw();

	set_color(0.f, 0.f, 0.f, 1.f);
	glm::mat4 fm = glm::translate(out, glm::vec3(0, 0, head_size.z));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(fm));
	face[1].draw();
}

void Robot::draw_arm(GLuint location, glm::mat4 out)
{
	set_color(1, 1, 0, 1);
	float z_add = velocity.y > 0 ? -velocity.y : 0;
	glm::mat4 la = glm::translate(out, glm::vec3(-body_size.x, 0, 0));
	la = glm::translate(la, glm::vec3(0, arm_size.y, 0));
	la = glm::rotate(la, radian(-15) - z_add, glm::vec3(0, 0, 1));
	la = glm::rotate(la, -animation, glm::vec3(1, 0, 0));
	la = glm::translate(la, glm::vec3(0, -arm_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(la));
	arm[0].draw();


	set_color(1, 1, 0, 1);
	glm::mat4 ra = glm::translate(out, glm::vec3(body_size.x, 0, 0));
	ra = glm::translate(ra, glm::vec3(0, arm_size.y, 0));
	ra = glm::rotate(ra, radian(15) + z_add, glm::vec3(0, 0, 1));
	ra = glm::rotate(ra, animation, glm::vec3(1, 0, 0));
	ra = glm::translate(ra, glm::vec3(0, -arm_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(ra));
	arm[1].draw();
}

void Robot::draw_leg(GLuint location, glm::mat4 out)
{
	set_color(0, 0.5, 0.5, 1);
	glm::mat4 ll = glm::translate(out, glm::vec3(-0.2f, 0, 0));
	ll = glm::translate(ll, glm::vec3(0, leg_size.y, 0));
	ll = glm::rotate(ll, animation, glm::vec3(1, 0, 0));
	ll = glm::translate(ll, glm::vec3(0, -leg_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(ll));
	leg[0].draw();

	set_color(0, 0.5, 0.5, 1);
	glm::mat4 rl = glm::translate(out, glm::vec3(0.2f, 0, 0));
	rl = glm::translate(rl, glm::vec3(0, leg_size.y, 0));
	rl = glm::rotate(rl, -animation, glm::vec3(1, 0, 0));
	rl = glm::translate(rl, glm::vec3(0, -leg_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rl));
	leg[1].draw();
}

Robot::Robot()
{
	head_size = glm::vec3(0.4f, 0.4f, 0.4f);
	body_size = glm::vec3(0.5f, 0.8f, 0.5f);
	arm_size = glm::vec3(0.1f, 0.6f, 0.2f);
	leg_size = glm::vec3(0.1f, 0.6f, 0.2f);


	face = new Box[2]; // head, nose
	face[0].set_size(head_size);
	face[1].set_size(0.1f, 0.1f, 0.1f);

	body = new Box;
	body->set_size(body_size);

	pos.z = 2.f;

	leg = new Box[2];
	arm = new Box[2];

	for (int i = 0; i < 2; ++i) {
		leg[i].set_size(leg_size);
		arm[i].set_size(arm_size);
	}
	sight = 0;
}

Robot::~Robot()
{

}

void Robot::update(glm::vec3 _pos, int* many)
{
	if (pos.y + (body_size.y + leg_size.y + head_size.y) * 2 > _pos.y)
	{
		if (pos.x < _pos.x + body_size.x && pos.x > _pos.x - body_size.x)
		{
			if (pos.z < _pos.z + body_size.z && pos.z > _pos.z - body_size.z)
				deadcheck = true;
		}
	}


	pos += velocity;

	pos.x = clamp(-10.f + body_size.x, pos.x, 10.f - body_size.x);
	pos.y = clamp(0, pos.y, 50.f);
	pos.z = clamp(body_size.z - 10.f, pos.z, 10.f - body_size.z);
	velocity.y -= 0.000098;
	velocity.x = velocity.x;
	velocity.z = velocity.z;

	CheckCollision(_pos, many);
	if (collisioncheck)
	{
		pos -= velocity;
		collisioncheck = false;
	}
	frame += (velocity.z + velocity.x);
	animation = sin(frame) * radian(45);
}

void Robot::CheckCollision(glm::vec3 _pos, int* many)
{
	for (int i = 0; i < 9; ++i)
	{
		if (pos.x >= -3.f && pos.x < -1.f)
		{
			if (pos.z >= -1.f && pos.z < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= -3.f && pos.z < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= 1.f && pos.z < 3.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
		}
		if (pos.x >= -1.f && pos.x < 1.f)
		{
			if (pos.z + (2 * body_size.z) >= -1.f && pos.z - (2 * body_size.z) < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= -3.f && pos.z < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= 1.f && pos.z < 3.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
		}
		if (pos.x >= 1.f && pos.x < 3.f)
		{
			if (pos.z >= -1.f && pos.z < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= -3.f && pos.z < 1.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
			if (pos.z >= 1.f && pos.z < 3.f)
			{
				if (many[i] * 2 > pos.y)
					collisioncheck = true;
			}
		}

	}

}

void Robot::draw(GLuint shaderID, glm::mat4 out)
{
	unsigned int location = glGetUniformLocation(shaderID, "modelTransform");
	glm::mat4 model = glm::translate(out, pos);
	model = glm::rotate(model, radian(sight), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));

	set_color(1, 1, 1, 1);

	model = glm::translate(model, glm::vec3(0, leg_size.y - 0.1f, 0));
	draw_leg(location, model);

	model = glm::translate(model, glm::vec3(0, leg_size.y + body_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	set_color(0, 1, 0.1, 1);
	body->draw();

	glm::mat4 arm_model = glm::translate(model, glm::vec3(0, body_size.y / 4, 0));
	draw_arm(location, arm_model);

	model = glm::translate(model, glm::vec3(0, body_size.y + head_size.y, 0));
	draw_head(location, model);
}

void Robot::jump()
{
	if (pos.y <= 0.001f) {
		velocity.y = .03f;
	}
}

void Robot::right()
{
	velocity.x = 0.01f;
	velocity.z = 0;
	sight = 90;
}

void Robot::left()
{
	velocity.x = -0.01f;
	velocity.z = 0;
	sight = 270;
}

void Robot::forward()
{
	velocity.x = 0;
	velocity.z = -0.01f;
	sight = 180;
}

void Robot::backward()
{
	velocity.x = 0;
	velocity.z = 0.01f;
	sight = 0;
}

void Robot::reset()
{
	velocity = glm::vec3(0, 0, 0);
	pos = glm::vec3(0, 0, 0);
	sight = 0;
	frame = 0;
	animation = 0;
}

glm::vec3 Robot::pos2()
{
	return pos;
}

//



Stage::Stage()
{
	pos.x = -2.f;
	pos.z = -2.f;
	boxes = new Box[9];
	len = 1.f;
	boxes[0].set_size(len, 0.01f, len);
	boxes[1].set_size(len, 0.01f, len);
	boxes[2].set_size(len, 0.01f, len);
	boxes[3].set_size(len, 0.01f, len);
	boxes[4].set_size(len, 0.01f, len);
	boxes[5].set_size(len, 0.01f, len);
	boxes[6].set_size(len, 0.01f, len);
	boxes[7].set_size(len, 0.01f, len);
	boxes[8].set_size(len, 0.01f, len);
}

Stage::~Stage()
{
	delete[] boxes;
}

void Stage::stage_up()
{
	curtain_up += 0.01;
}

void Stage::stage_down()
{
	curtain_up -= 0.01;
}

void Stage::draw(GLuint shaderID, glm::mat4 out)
{
	int i = 0;
	unsigned int location = glGetUniformLocation(shaderID, "modelTransform");
	//glm::mat4 model = out * glm::mat4(1.0f); 
	glm::mat4 model = glm::translate(out, pos);

	set_color(1, 0, 0, 1);
	glm::mat4 m0 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m0));
	boxes[i].draw();
	i++;

	set_color(0, 1, 0, 1);
	glm::mat4 m1 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m1));
	boxes[i].draw();
	i++;

	set_color(1, 1, 0, 1);
	glm::mat4 m2 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m2));
	boxes[i].draw();
	i++;

	set_color(0, 0, 1, 1);
	glm::mat4 m3 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m3));
	boxes[i].draw();
	i++;

	set_color(1, 0, 1, 1);
	glm::mat4 m4 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m4));
	boxes[i].draw();
	i++;

	set_color(0, 1, 1, 1);
	glm::mat4 m5 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	m5 = glm::translate(m5, glm::vec3(0, stage_curtain, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m5));
	boxes[i].draw();
	i++;

	set_color(0, 1, 0, 1);
	glm::mat4 m6 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m6));
	boxes[i].draw();
	i++;

	set_color(0, 0, 1, 1);
	glm::mat4 m7 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m7));
	boxes[i].draw();
	i++;

	set_color(0, 1, 0, 1);
	glm::mat4 m8 = glm::translate(model, glm::vec3((i % 3) * len * 2, 0, (i / 3) * len * 2));
	m5 = glm::translate(m5, glm::vec3(0, stage_curtain, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m8));
	boxes[i].draw();
	i++;
}

void Stage::update()
{
	stage_curtain = clamp(0, stage_curtain + curtain_up, len * 2);
}


StageBox::StageBox()
{
	boxes = new Box[6];
	len = 1.f;
	alpha = 1.f;
	pos.y = 10.f;
	velocity.y = 0.01f;

	boxes[0].set_size(len, 0.01f, len);
	boxes[1].set_size(len, 0.01f, len);
	boxes[2].set_size(0.01f, len, len);
	boxes[3].set_size(0.01f, len, len);
	boxes[4].set_size(len, len, 0.01f);
	boxes[5].set_size(len, len, 0.01f);
}


StageBox::~StageBox()
{
	delete[] boxes;
}

void StageBox::draw(GLuint shaderID, glm::mat4 out)
{
	int i = 0;
	unsigned int location = glGetUniformLocation(shaderID, "modelTransform");

	//glm::mat4 model = out * glm::mat4(1.0f);
	glm::mat4 model = glm::translate(out, pos);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	//set_color(1, 0, 0);
	//boxes[i].draw();
	//i++;
	//model = glm::translate(model, glm::vec3(3.0f, len +1.0f, 0));							// y รเ
	//model = glm::rotate(model, 1.0f, glm::vec3(0, 0, 1));
	//model = glm::rotate(model, rot, glm::vec3(1, 1, 0));

	set_color(1, 0, 0, alpha);
	glm::mat4 m0 = glm::translate(model, glm::vec3(0, 0, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m0));
	boxes[i].draw();
	i++;

	set_color(0, 1, 0, alpha);
	glm::mat4 m1 = glm::translate(model, glm::vec3(0, 2 * len, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m1));
	boxes[i].draw();
	i++;

	set_color(1, 1, 0, alpha);
	glm::mat4 m2 = glm::translate(model, glm::vec3(len, len, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m2));
	boxes[i].draw();
	i++;

	set_color(0, 0, 1, alpha);
	glm::mat4 m3 = glm::translate(model, glm::vec3(-len, len, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m3));
	boxes[i].draw();
	i++;

	set_color(1, 0, 1, alpha);
	glm::mat4 m4 = glm::translate(model, glm::vec3(0, len, -len));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m4));
	boxes[i].draw();
	i++;


	set_color(0, 1, 1, alpha);
	glm::mat4 m5 = glm::translate(model, glm::vec3(0, len, len));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m5));
	boxes[i].draw();
	i++;


}

void StageBox::update()
{

}

void StageBox::update(int* many)
{
	if (!bottomCheck)
	{
		pos.y -= velocity.y;

		if (pos.x == 0.f)
		{
			if (pos.z == 0.f)
			{
				pos.y = clamp(many[4] * 2, pos.y, 100.f);
				if (many[4] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == -2.f)
			{
				pos.y = clamp(many[7] * 2, pos.y, 100.f);
				if (many[7] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == 2.f)
			{
				pos.y = clamp(many[1] * 2, pos.y, 100.f);
				if (many[1] * 2 == pos.y)
					bottomCheck = true;
			}
		}
		else if (pos.x == -2.f)
		{
			if (pos.z == 0.f)
			{
				pos.y = clamp(many[3] * 2, pos.y, 100.f);
				if (many[3] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == -2.f)
			{
				pos.y = clamp(many[6] * 2, pos.y, 100.f);
				if (many[6] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == 2.f)
			{
				pos.y = clamp(many[0] * 2, pos.y, 100.f);
				if (many[0] * 2 == pos.y)
					bottomCheck = true;
			}
		}
		else if (pos.x == 2.f)
		{
			if (pos.z == 0.f)
			{
				pos.y = clamp(many[5] * 2, pos.y, 100.f);
				if (many[5] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == -2.f)
			{
				pos.y = clamp(many[8] * 2, pos.y, 100.f);
				if (many[8] * 2 == pos.y)
					bottomCheck = true;
			}
			else if (pos.z == 2.f)
			{
				pos.y = clamp(many[2] * 2, pos.y, 100.f);
				if (many[2] * 2 == pos.y)
					bottomCheck = true;
			}
		}

	}
}

void StageBox::check_bottom()
{

}

void StageBox::right()
{
	pos.z += len * 2;
}

void StageBox::left()
{
	pos.z -= len * 2;
}

void StageBox::forward()
{
	pos.x += len * 2;
}

void StageBox::backward()
{
	pos.x -= len * 2;
}

void StageBox::SetPos(glm::vec3 _pos, int* many)
{
	pos.x = _pos.x;
	pos.z = _pos.z;

	if (pos.x == 0.f)
	{
		if (pos.z == 0.f)
		{
			pos.y = many[4] * 2;
		}
		else if (pos.z == -2.f)
		{
			pos.y = many[7] * 2;
		}
		else if (pos.z == 2.f)
		{
			pos.y = many[1] * 2;
		}
	}
	else if (pos.x == -2.f)
	{
		if (pos.z == 0.f)
		{
			pos.y = many[3] * 2;
		}
		else if (pos.z == -2.f)
		{
			pos.y = many[6] * 2;
		}
		else if (pos.z == 2.f)
		{
			pos.y = many[0] * 2;
		}
	}
	else if (pos.x == 2.f)
	{
		if (pos.z == 0.f)
		{
			pos.y = many[5] * 2;
		}
		else if (pos.z == -2.f)
		{
			pos.y = many[8] * 2;
		}
		else if (pos.z == 2.f)
		{
			pos.y = many[2] * 2;
		}
	}
}



void Robot2::draw_head(GLuint location, glm::mat4 out)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(out));
	set_color(1, 1, 1, 1);
	face[0].draw();

	set_color(0.f, 0.f, 0.f, 1);
	glm::mat4 fm = glm::translate(out, glm::vec3(0, 0, head_size.z));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(fm));
	face[1].draw();
}

void Robot2::draw_arm(GLuint location, glm::mat4 out)
{
	set_color(1, 1, 0, 1);
	float z_add = velocity.y > 0 ? -velocity.y : 0;
	glm::mat4 la = glm::translate(out, glm::vec3(-body_size.x, 0, 0));
	la = glm::translate(la, glm::vec3(0, arm_size.y, 0));
	la = glm::rotate(la, radian(-15) - z_add, glm::vec3(0, 0, 1));
	la = glm::rotate(la, -animation, glm::vec3(1, 0, 0));
	la = glm::translate(la, glm::vec3(0, -arm_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(la));
	arm[0].draw();


	set_color(1, 1, 0, 1);
	glm::mat4 ra = glm::translate(out, glm::vec3(body_size.x, 0, 0));
	ra = glm::translate(ra, glm::vec3(0, arm_size.y, 0));
	ra = glm::rotate(ra, radian(15) + z_add, glm::vec3(0, 0, 1));
	ra = glm::rotate(ra, animation, glm::vec3(1, 0, 0));
	ra = glm::translate(ra, glm::vec3(0, -arm_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(ra));
	arm[1].draw();
}

void Robot2::draw_leg(GLuint location, glm::mat4 out)
{
	set_color(0, 0.5, 0.5, 1);
	glm::mat4 ll = glm::translate(out, glm::vec3(-0.2f, 0, 0));
	ll = glm::translate(ll, glm::vec3(0, leg_size.y, 0));
	ll = glm::rotate(ll, animation, glm::vec3(1, 0, 0));
	ll = glm::translate(ll, glm::vec3(0, -leg_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(ll));
	leg[0].draw();

	set_color(0, 0.5, 0.5, 1);
	glm::mat4 rl = glm::translate(out, glm::vec3(0.2f, 0, 0));
	rl = glm::translate(rl, glm::vec3(0, leg_size.y, 0));
	rl = glm::rotate(rl, -animation, glm::vec3(1, 0, 0));
	rl = glm::translate(rl, glm::vec3(0, -leg_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(rl));
	leg[1].draw();
}

Robot2::Robot2()
{
	head_size = glm::vec3(0.4f, 0.4f, 0.4f);
	body_size = glm::vec3(0.5f, .8f, .6f);
	arm_size = glm::vec3(0.1f, 0.6f, 0.2f);
	leg_size = glm::vec3(0.1f, 0.6f, 0.2f);


	face = new Box[2]; // head, nose
	face[0].set_size(head_size);
	face[1].set_size(0.1f, 0.1f, 0.1f);

	body = new Box;
	body->set_size(body_size);


	leg = new Box[2];
	arm = new Box[2];

	for (int i = 0; i < 2; ++i) {
		leg[i].set_size(leg_size);
		arm[i].set_size(arm_size);
	}
	sight = 0;
}

Robot2::~Robot2()
{

}

void Robot2::update()
{
	pos += velocity;
	pos.x = clamp(-10.f + body_size.x, pos.x + 3.f, 10.f - body_size.x);
	pos.y = clamp(0, pos.y, 50.f);
	pos.z = clamp(body_size.z - 10.f, pos.z, 10.f - body_size.z);
	velocity.y -= 0.000098;
	velocity.x = velocity.x;
	velocity.z = velocity.z;


	frame += (velocity.z + velocity.x);
	animation = sin(frame) * radian(45);
}

void Robot2::draw(GLuint shaderID, glm::mat4 out)
{
	unsigned int location = glGetUniformLocation(shaderID, "modelTransform");
	glm::mat4 model = glm::translate(out, pos);
	model = glm::rotate(model, radian(sight), glm::vec3(0, 1, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));

	set_color(1, 1, 1, 1);

	model = glm::translate(model, glm::vec3(0, leg_size.y - 0.1f, 0));
	draw_leg(location, model);

	model = glm::translate(model, glm::vec3(0, leg_size.y + body_size.y, 0));
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(model));
	set_color(0, 1, 0.1, 1);
	body->draw();

	glm::mat4 arm_model = glm::translate(model, glm::vec3(0, body_size.y / 4, 0));
	draw_arm(location, arm_model);

	model = glm::translate(model, glm::vec3(0, body_size.y + head_size.y, 0));
	draw_head(location, model);
}

void Robot2::jump()
{
	if (pos.y <= 0.001f) {
		velocity.y = .03f;
	}
}

void Robot2::right()
{
	velocity.x = 0.01f;
	velocity.z = 0;
	sight = 90;
}

void Robot2::left()
{
	velocity.x = -0.01f;
	velocity.z = 0;
	sight = 270;
}

void Robot2::forward()
{
	velocity.x = 0;
	velocity.z = -0.01f;
	sight = 180;
}

void Robot2::backward()
{
	velocity.x = 0;
	velocity.z = 0.01f;
	sight = 0;
}

void Robot2::reset()
{
	velocity = glm::vec3(0, 0, 0);
	pos = glm::vec3(0, 0, 0);
	sight = 0;
	frame = 0;
	animation = 0;
}

glm::vec3 Robot2::pos2()
{
	return pos;
}