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

int main() {
	constexpr float heightStep = atmThick / dtransmitT_hDim, cosStep = 2.0 / dtransmitT_cosDim;
	cout << earthRadius << endl;
	cout << 1000000.0 << endl;
	for (size_t i = transmitT_hDim - 1; i < transmitT_hDim; ++i) {
		double curRadius = earthRadius + static_cast<double>(i) * heightStep;
		double curRadius2 = pow(curRadius, 2);
		cout << "Height = " << curRadius << endl;
		for (size_t j = 0; j < transmitT_cosDim; ++j) {
			double curCos = 1.0 - cosStep * static_cast<double>(j);
			cout << "curCos: " << curCos << ". Value: " << getHerizonAtmDepth(curRadius, curRadius2, curCos) << endl;
		}
	}

}