#include "inputRecorder.hpp"

static bool w, a, s, d, t;

bool wPressed() {
	return w;
}
bool aPressed() {
	return a;
}
bool sPressed() {
	return s;
}
bool dPressed() {
	return d;
}
bool tPressed() {
	return t;
}

void inRecKeyboardFunc(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'w': case 'W':
		w = true;
		break;
	case 'a': case 'A':
		a = true;
		break;
	case 's': case 'S':
		s = true;
		break;
	case 'd': case 'D':
		d = true;
		break;
	case 't': case 'T':
		t = true;
		break;
	}
}

void initInRec() {
	w = a = s = d = t = false;
}