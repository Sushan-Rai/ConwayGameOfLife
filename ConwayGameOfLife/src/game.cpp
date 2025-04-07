#include <GL\glew.h>
#include <GL\freeglut.h>

#define NUM_KEYS 256
bool keysDown[NUM_KEYS];
bool lastKeysDown[NUM_KEYS];
#define NUM_MOUSE_BUTTONS 10
bool mouseButtonsDown[NUM_MOUSE_BUTTONS];
bool lastMouseButtonsDown[NUM_MOUSE_BUTTONS];
int mouseX = 0, mouseY = 0;

int window = 0;
int windowWidth, windowHeight;
void(*clientUpdate)();
void(*clientDraw)();

void onKeyboardDown(unsigned char key, int x, int y) {
    lastKeysDown[key] = keysDown[key];
    keysDown[key] = true;
}
void onKeyboardUp(unsigned char key, int x, int y) {
    lastKeysDown[key] = keysDown[key];
    keysDown[key] = false;
}
void onMouseButton(int button, int state, int x, int y) {
    lastMouseButtonsDown[button] = mouseButtonsDown[button];
    mouseButtonsDown[button] = (state == 0); // down
}
void onMouseMove(int x, int y) {
    mouseX = x;
    mouseY = y;
}

void onDisplay() {
}

void updateLoop(int val) {
    clientUpdate();

    // update input because events are not immediate enough
    for (int i = 0; i < NUM_KEYS; i++) {
        lastKeysDown[i] = keysDown[i];
    }
    for (int i = 0; i < NUM_MOUSE_BUTTONS; i++) {
        lastMouseButtonsDown[i] = mouseButtonsDown[i];
    }

    glutTimerFunc(16, updateLoop, 0); // 16 ms = 60 fps
}

void drawLoop(int val) {
    clientDraw();
    glutTimerFunc(8, drawLoop, 0); // 8 ms = 120 fps
}

void gSetup(int argc, char** argv, int w, int h, bool fs, void(*u)(), void(*d)()) {

    windowWidth = w;
    windowHeight = h;
    clientUpdate = u;
    clientDraw = d;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE);
    glutInitWindowSize(w, h);
    glutInitWindowPosition(0, 0);
    window = glutCreateWindow("Hi");
    if (fs)
        glutFullScreen();

    glutDisplayFunc(onDisplay);

    glutKeyboardUpFunc(onKeyboardUp);
    glutKeyboardFunc(onKeyboardDown);
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMouseMove);
    glutPassiveMotionFunc(onMouseMove);

    glutTimerFunc(0, updateLoop, 0);
    glutTimerFunc(0, drawLoop, 0);

    glutMainLoop();
}

void gExit() {
    glutLeaveMainLoop();
    glutDestroyWindow(window);
    window = 0;
}

// OpenGL has -.5 to .5 coords and we want to scale
// ours to match the mixel dimensions of the window
float gGetGLX(float x) {
    return x / windowWidth * 2;
}
float gGetGLY(float y) {
    return y / windowHeight * 2;
}
float gGetWinW() {
    return windowWidth;
}
float gGetWinH() {
    return windowHeight;
}

// input functions
bool gIsKeyDown(unsigned char key) {
    return keysDown[key];
}
bool gIsKeyReleased(unsigned char key) {
    return lastKeysDown[key] && !keysDown[key];
}
bool gIsMouseButtonDown(int button) {
    return mouseButtonsDown[button];
}
bool gIsMouseButtonReleased(int button) {
    return lastMouseButtonsDown[button] && !mouseButtonsDown[button];
}
int gGetMouseX() {
    return mouseX;
}
int gGetMouseY() {
    return mouseY;
}