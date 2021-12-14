#ifndef PCH_H
#define PCH_H

#include <iostream>
#include <string>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm/ext.hpp>					//10.0.18362.0
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>
#include <gl/glm/glm/gtc/type_ptr.hpp>

//#include <glm/ext.hpp>
//#include <glm/glm/glm.hpp>
//#include <glm/glm/glm/gtc/matrix_transform.hpp>
//#include <glm/glm/gtc/type_ptr.hpp>

#include <vector>

float clamp(float min, float mid, float max);
float radian(float angle);

#endif