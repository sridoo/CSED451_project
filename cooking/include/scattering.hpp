#pragma once
#include <glm/glm.hpp>


template<typename Fun>
static glm::vec3 integrate3(Fun fun, float from, float to, float step);
static inline double cosineToSine(double cosine);
static inline double cosSum(double cos1, double cos2);
static inline double displacedRaidus(double initRadius, double initRadius2, double dirCos, double x);
static inline double sunDisplacedCos(double viewCos, double initRadius, double initRadius2, double sunCos, double x);
static inline double phaseFunRay(double cosine);