#pragma once
#include <memory>
#include "GL/glew.h"

#include "glm/vec3.hpp"


class TransmitTable {
	std::unique_ptr<glm::dvec3[]> val;
public:
	glm::dvec3* get();
	const glm::dvec3* get() const;
	TransmitTable(const char* cachePath);
	const glm::dvec3& operator()(double radius, double consine) const;
	std::unique_ptr<GLubyte[]> toRGB() const;
};

static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;

extern TransmitTable transmitTable;
extern GLuint transmitTableTexID;

void initTransmitTableTexID();