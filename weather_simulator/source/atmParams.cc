#include "atmParams.hpp"

double getHerizonAtmDepth(double curRadius, double curRaidus2, double dirCos) {
	double b = curRadius * dirCos;
	double curHeightSine2 = curRaidus2 - pow(b, 2);

	if (dirCos > 0) if (double tmp = earthRadius2 - curHeightSine2; tmp > 0)
		return b - sqrt(tmp);
	return b + sqrt(atmTopRaidus2 - curHeightSine2);
}

double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x) {
	return sqrt(initRadius2 + pow(x, 2) - 2 * x * initRadius * dirCos);
}
double displacedCos(double initRadius, double initRadius2, double dirCos, double x) {
	return (initRadius - dirCos * x) / displacedRaidus(initRadius, initRadius2, dirCos, x);
}
double displacedDensity(double dirCos, double initRadius, double initRadius2, double x) {
	return exp(-(displacedRaidus(initRadius, initRadius2, dirCos, x) - earthRadius) / atmThick);
}

const glm::dvec3 rayScatCoef = { 5.6 * pow(10, -6.0), 1.35 * pow(10, -5.0), 3.31 * pow(10, -5.0) };