#include <fstream>
#include <iostream>

#include "transmit.hpp"

using namespace std;

int main() {
	ifstream transmitCache(".\\resource\\transmitTable.cache", ios::binary);
	TransmitTable transmitTable(transmitCache);

}