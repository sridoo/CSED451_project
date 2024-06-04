#include <fstream>
#include <iostream>

#include "transmit.hpp"
#include "scattering.hpp"


using namespace std;

int main() {
	ifstream scatterCache(".\\resource\\singleScatter.cache", ios::binary);
	auto scatterT = readScatterTable(scatterCache);
	ofstream singleScatterTex(".\\resource\\singleScatterTex.tex");
	writeScatterTableTex(singleScatterTex, scatterT);
	writeScatterTReadable(std::cout, 10000, scatterT);
}