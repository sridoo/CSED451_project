#pragma once
#include <memory>
#include <ostream>
#include <istream>

#include "GL/glew.h"

#include "glm/vec3.hpp"


class TransmitTable {
	std::unique_ptr<glm::dvec3[]> val;
	TransmitTable(std::unique_ptr<glm::dvec3[]> arr);
	friend TransmitTable bakeTransmitTable();
public:
	glm::dvec3* get();
	const glm::dvec3* get() const;
	TransmitTable(std::istream& in);
	const glm::dvec3& operator()(double radius, double consine) const;
	std::unique_ptr<GLubyte[]> toRGB() const;
};


TransmitTable bakeTransmitTable();
constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;
constexpr size_t nTransmitElem = transmitT_hDim * transmitT_cosDim;
void writeTransmitTableReadable(std::ostream& out, size_t lineLimit, const TransmitTable& table);
void writeTransmitTable(std::ostream& out, const TransmitTable& table);
void writeTransmitTableTex(std::ostream& out, const TransmitTable& table);