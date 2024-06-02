#include<iostream>
#include "glm/vec3.hpp"

using namespace std;
using namespace glm;

static constexpr double atmThick = 8000;
static constexpr double earthRadius = 6371000;
static constexpr double earthRadius2 = earthRadius * earthRadius;
static constexpr double atmTopRadius = earthRadius + atmThick;
static constexpr double atmTopRaidus2 = atmTopRadius * atmTopRadius;

static constexpr size_t transmitT_hDim = 512, transmitT_cosDim = 512;
static constexpr double dtransmitT_hDim = 512.0, dtransmitT_cosDim = 512.0;
static constexpr size_t intensity_hDim = 32, intnesity_viewDim = 256, intensity_sunDim = 64;


double getHerizonAtmDepth(double curRadius, double curRaidus2, double dirCos) {
	double b = curRadius * dirCos;
	double curHeightSine2 = curRaidus2 - pow(b, 2);

	if(dirCos > 0) if (double tmp = earthRadius2 - curHeightSine2; tmp > 0)
		return b - sqrt(tmp);
	return b + sqrt(atmTopRaidus2 - curHeightSine2);
}

static inline double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x) {
	return sqrt(initRadius2 + pow(x, 2) - 2 * x * initRadius * dirCos);
}
static inline double density(double dirCos, double initRadius, double initRadius2, double x) {
	return exp(-(displacedRaidus(initRadius, initRadius2, dirCos, x) - earthRadius) / atmThick);
}

const static dvec3 rayScatCoef = { 5.6 * pow(10, -6.0), 1.35 * pow(10, -5.0), 3.31 * pow(10, -5.0) };

template<typename T>
static double integrate(T fun, double from, double to, double step) {
	double val = 0, last = fun(from);
	for (double i = from + step; i < to; i += step) {
		double cur = fun(i);
		val += step * (last + cur) / 2.f;
		last = cur;
	}
	return val;
}

int main() {

	class Density {
		const double dirCos_, initRadius_, initRadius2_;
	public:
		Density(double dirCos, double initRadius, double initRadius2)
			: dirCos_(dirCos), initRadius_(initRadius), initRadius2_(initRadius2) {}

		double operator()(double x) {
			return density(dirCos_, initRadius_, initRadius2_, x);
		}
	};

	constexpr float heightStep = atmThick / dtransmitT_hDim, cosStep = 2.0 / dtransmitT_cosDim;
	cout << earthRadius << endl;
	cout << 1000000.0 << endl;
	for (size_t i = 0; i < 1; ++i) {
		double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		double curRadius2 = pow(curRadius, 2);
		cout << "Height = " << curRadius << endl;
		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			double curCos = 1.0 - cosStep * static_cast<double>(j);
			double xEnd = getHerizonAtmDepth(curRadius, curRadius2, curCos);
			vec3 val = -rayScatCoef * integrate(Density(curCos, curRadius, curRadius2), 0.0, xEnd, 7.0);
			cout << "curCos: " << curCos << ". Value: " << val.x << ", " << val.y << ", " << val.z << endl;
		}
	}

}