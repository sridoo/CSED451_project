#pragma once
#include <memory>

#include "glm/vec3.hpp"


class FtransmitTable {
	std::unique_ptr<glm::vec3[]> val_;
	FtransmitTable(std::unique_ptr<glm::vec3[]> val);
	friend class TransmitTable;
public:
	glm::vec3* get();
	const glm::vec3* get() const;
	FtransmitTable() = default;
};

class TransmitTable {
	std::unique_ptr<glm::dvec3[]> val;
public:
	glm::dvec3* get();
	const glm::dvec3* get() const;
	TransmitTable(const char* cachePath);
	const glm::dvec3& operator()(double radius, double consine) const;
	FtransmitTable toFloat() const;
};

static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;

extern TransmitTable transmitTable;
extern FtransmitTable fTransmitTable;