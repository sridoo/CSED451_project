#include <cmath>
#include "glm/vec3.hpp"

static constexpr double atmThick = 8000;
static constexpr double earthRadius = 6371000;
static constexpr double earthRadius2 = earthRadius * earthRadius;
static constexpr double atmTopRadius = earthRadius + atmThick;
static constexpr double atmTopRaidus2 = atmTopRadius * atmTopRadius;


inline double getHerizonAtmDepth(double curRadius, double curRaidus2, double dirCos) {
	double b = curRadius * dirCos;
	double curHeightSine2 = curRaidus2 - pow(b, 2);

	if (dirCos >= 0) if (double tmp = earthRadius2 - curHeightSine2; tmp >= 0)
		return b - sqrt(tmp);
	return b + sqrt(atmTopRaidus2 - curHeightSine2);
}

inline double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x) {
	return sqrt(initRadius2 + pow(x, 2) - 2 * x * initRadius * dirCos);
}
inline double displacedDensity(double dirCos, double initRadius, double initRadius2, double x) {
	return exp(-(displacedRaidus(initRadius, initRadius2, dirCos, x) - earthRadius) / atmThick);
}

const glm::dvec3 rayScatCoef = { 5.6E-6, 1.35E-5, 3.31E-5 };