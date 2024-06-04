#pragma once

#include "glm/vec3.hpp"

constexpr float mapWidth = 200.f;
constexpr float mapWidthHalf = mapWidth / 2.f;
extern float curTime;
extern float timeFlow;
extern glm::vec3 sunLightDir;

void moveTime(float timePassed);
void globalParamsIdle();