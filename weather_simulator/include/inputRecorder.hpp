#pragma once
bool wPressed();
bool aPressed();
bool sPressed();
bool dPressed();
bool tPressed();
bool upPressed();
bool downPressed();

void initInRec();
void inRecKeyboardFunc(unsigned char key, int x, int y);
void inRecSpecialFunc(int key, int x, int y);