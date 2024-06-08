#include <fstream>
#include <iostream>

#include "transmit.hpp"
#include "scattering.hpp"


using namespace std;

int main() {
	ofstream transmitCache(".\\resource\\transmitTable.cache", ios::binary),
		transmitTex(".\\resource\\transmitTableTex.tex");
	TransmitTable transmitTable = bakeTransmitTable();
	writeTransmitTable(transmitCache, transmitTable);
	writeTransmitTableTex(transmitTex, transmitTable);
	//ofstream singleScatterTex(".\\resource\\singleScatterTex.tex");
	//writeScatterTableTex(singleScatterTex, scatterT);
}