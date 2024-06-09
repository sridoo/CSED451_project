#pragma once
#include <memory>
#include <istream>
#include<ostream>

#include "glm/vec3.hpp"

#include "transmit.hpp"

constexpr size_t intensity_hDim = 32, intensity_viewDim = 256, intensity_sunDim = 64;
constexpr size_t nIntensityElems = intensity_hDim * intensity_viewDim * intensity_sunDim;
struct ScatterTable {
	std::unique_ptr<glm::dvec3[]> first, second;
	double maxVal;

	ScatterTable();
};
ScatterTable bakeScatterTable(size_t scatterLevel, const TransmitTable& transmitTable);
void writeScatterTableTex(std::ostream& amOut, std::ostream& pmOut, const ScatterTable& scatterTable);
void writeScatterTable(std::ostream& amOut, std::ostream& pmOut, const ScatterTable& scatterTable);
void writeScatterTReadable(std::ostream& out, size_t maxLines, const ScatterTable& scatterTable);
ScatterTable readScatterTable(std::istream& amIn, std::istream& pmIn);