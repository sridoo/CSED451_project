#pragma once
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

glm::mat4 getCamMat();
void camIdle();

extern const glm::vec3 camPos;