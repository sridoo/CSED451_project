#pragma once
#include "glm/vec3.hpp"

static constexpr double atmThick = 8000;
static constexpr double earthRadius = 6371000;
static constexpr double earthRadius2 = earthRadius * earthRadius;
static constexpr double atmTopRadius = earthRadius + atmThick;
static constexpr double atmTopRaidus2 = atmTopRadius * atmTopRadius;


double getHerizonAtmDepth(double curRadius, double curRaidus2, double dirCos);
double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x);
double displacedCos(double initRadius, double initRadius2, double dirCos, double x);
double displacedDensity(double dirCos, double initRadius, double initRadius2, double x);

extern const glm::dvec3 rayScatCoef;
