#ifndef GAME_H
#define GAME_H


void gSetup(int argc, char** argv, int w, int h, bool fs, void(*u)(), void(*d)());
void gExit();
float gGetGLX(float x);
float gGetGLY(float y);
float gGetWinW();
float gGetWinH();
bool gIsKeyDown(unsigned char key);
bool gIsKeyReleased(unsigned char key);
bool gIsMouseButtonDown(int button);
bool gIsMouseButtonReleased(int button);
int gGetMouseX();
int gGetMouseY();

#endif