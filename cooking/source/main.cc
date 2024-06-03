#include <fstream>
#include <iostream>

#include "transmit.hpp"

using namespace std;

int main() {
	ifstream fileRead(".\\resource\\transmitTable.cache", std::ios::binary);
	TransmitTable transmitTable(fileRead);
	cout << "readDone" << endl;
	writeTransmitTableReadable(cout, 100000, transmitTable);
}