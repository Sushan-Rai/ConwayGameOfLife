#include <stdio.h>
#include <GL\glew.h>
#include <GL\freeglut.h>
#include "game.h"

#define MAP_ROWS 144 // 72
#define MAP_COLS 256 // 128
#define NUM_SQUARES (MAP_ROWS * MAP_COLS)
#define SQUARE_WIDTH 10
#define SQUARE_HEIGHT 10
#define SQUARE_PADDING 2

enum GameState {
    G_SPLASH,
    G_DRAW,
    G_SIMULATE,
    G_APPLY
};
enum GameState gameState = G_SPLASH;

struct Square {
    int r, c;
    float x, y;
    float w, h;
    int state, nextState;
};

struct Square squares[NUM_SQUARES];

float wld2scrX(float x) {
    return x - gGetWinW() / 2;
}
float wld2scrY(float y) {
    return y - gGetWinH() / 2;
}
float scr2wldX(float x) {
    return x;
}
float scr2wldY(float y) {
    return gGetWinH() - y; // screen pixel top left to world coord bottom left
}

void drawSplash() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1, 1, 1); // White text
    glRasterPos2f(-0.2f, 0.4f); // Adjust as needed
    const char* lines[] = {
        "Mini Project",
        "Game of Life",
        "",
        "           - by",
        "Sushan Rai             1RVU22CSE173",
        "Akarsh Kumar Gowda     1RVU22CSE016",
        "",
        "Press ENTER to start..."
    };

    for (int i = 0; i < sizeof(lines) / sizeof(lines[0]); i++) {
        glRasterPos2f(-0.5f, 0.4f - i * 0.1f);
        for (const char* c = lines[i]; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    glFlush();
}

void drawSquare(struct Square* sq) {
    glBegin(GL_POLYGON);

    float glX = gGetGLX(wld2scrX(sq->x));
    float glY = gGetGLY(wld2scrY(sq->y));
    float glW = gGetGLX(sq->w);
    float glH = gGetGLY(sq->h);

    // cyan magenta
    if (sq->state != sq->nextState)
        glColor3f(0, 1, 1);
    else if (sq->state == 1)
        glColor3f(1, 0, 1);
    else
        glColor3f(.1, .1, .1);

    // red yellow
    // if (sq->state != sq->nextState)
    //     glColor3f(1, .6, 0);
    // else if (sq->state == 1)
    //     glColor3f(1, 0, 0);
    // else
    //     glColor3f(.1, .1, .1);

    // blue white
    // if (sq->state != sq->nextState)
    //     glColor3f(0, 1, 0);
    // else if (sq->state == 1)
    //     glColor3f(1, 1, 0);
    // else
    //     glColor3f(.1, .1, .1);
    float fr = (float)sq->r / (float)MAP_ROWS;
    float fc = (float)sq->c / (float)MAP_COLS;
    // if (sq->state != sq->nextState)
    //     glColor3f(fr, 0, fr);
    // else if (sq->state == 1)
    //     glColor3f(0, fc, fc);
    // else
    //     glColor3f(.1, .1, .1);

    glVertex2f(glX, glY); // left bottom
    glVertex2f(glX + glW, glY); // right bottom
    glVertex2f(glX + glW, glY + glH); // right top
    glVertex2f(glX, glY + glH); // left top

    glEnd();
}

void paintSquare(int mx, int my, bool toggle) {
    // convert from click coords with mouse to the game coords
    int indexCol = scr2wldX(mx) / SQUARE_WIDTH;
    int indexRow = scr2wldY(my) / SQUARE_HEIGHT;
    // bounds check and change the square's state
    if (indexCol >= 0 && indexRow >= 0 && indexCol < MAP_COLS && indexRow < MAP_ROWS) {
        int index = indexRow * MAP_COLS + indexCol;
        struct Square* sq = &squares[index];
        if (toggle) {
            sq->state = !sq->state;
        }
        else {
            sq->state = 1;
        }
    }
}

int countAliveNeighbors(struct Square* sq) {
    int count = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0)
                continue; // skip self
            // we want to wrap our edges around so its infinite
            int nbC = i + sq->c;
            int nbR = j + sq->r;
            // wrapping logic
            if (nbC < 0)
                nbC = MAP_COLS + nbC; // wrap
            if (nbC >= MAP_COLS)
                nbC = MAP_COLS - nbC;
            if (nbR < 0)
                nbR = MAP_ROWS + nbR; // wrap
            if (nbR >= MAP_ROWS)
                nbR = MAP_ROWS - nbR;
            int nbIndex = nbR * MAP_COLS + nbC; // neighbors index
            if (squares[nbIndex].state == 1)
                count++; // increment neighbors
        }
    }
    return count;
}

// Rules
// 1. Any live cell with less than 2 live neighbors dies (underpopulation)
// 2. Any live cell with 2 or 3 neighbors lives
// 3. Any live cell with more than 3 live neighbors dies (overpopulation)
// 4. Any dead cell with exactly 3 neighbors becomes alive (reproduction)
void gameOfLife(struct Square* sq) {
    int nbCount = countAliveNeighbors(sq);
    if (sq->state == 1) {
        // alive
        if (nbCount < 2 || nbCount > 3) {
            sq->nextState = 0; // should die
        }
        else {
            sq->nextState = 1; // should live
        }
    }
    else {
        // dead
        if (nbCount == 3) {
            sq->nextState = 1; // should come alive
        }
        else {
            sq->nextState = 0; // should remain dead
        }
    }
}

void setup() {
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            int i = r * MAP_COLS + c;
            squares[i].r = r;
            squares[i].c = c;
            squares[i].x = c * SQUARE_WIDTH + SQUARE_PADDING / 2;
            squares[i].y = r * SQUARE_HEIGHT + SQUARE_PADDING / 2;
            squares[i].w = SQUARE_WIDTH - SQUARE_PADDING;
            squares[i].h = SQUARE_HEIGHT - SQUARE_PADDING;
            squares[i].state = 0; // off
            squares[i].nextState = 0; // off

             int cenR = MAP_ROWS / 2;
             int cenC = MAP_COLS / 2;
             if (r > cenR - 30 && r < cenR + 30)
                 if (c > cenC - 30 && c < cenC + 30)
                     squares[i].state = 1;
             if (r % 4 == 0 || c % 4 == 0)
             if (r - MAP_ROWS / 2 < 10)
                 if (c - MAP_COLS / 2 < 10)
                     squares[i].state = 1;

        }
    }
}

void update() {
    // if we press 'x' exit the game
    if (gIsKeyReleased('x')) {
        gExit();
    }

    if (gameState == G_SPLASH) {
        if (gIsKeyReleased(13)) { // Enter key
            gameState = G_DRAW;
        }
        return;
    }

    switch (gameState) {
    case G_DRAW:
        if (gIsKeyDown('s')) {
            // paint mode
            if (gIsMouseButtonDown(0)) {
                paintSquare(gGetMouseX(), gGetMouseY(), false);
            }
        }
        else {
            // click mode
            if (gIsMouseButtonReleased(0)) {
                paintSquare(gGetMouseX(), gGetMouseY(), true);
            }
        }
        // go from drawing to simulation
        if (gIsKeyReleased(32)) { // spacebar
            gameState = G_SIMULATE;
        }
        break;
    case G_SIMULATE:
        // simulate conways game of life across the entire board
        for (int i = 0; i < NUM_SQUARES; i++) {
            gameOfLife(&squares[i]);
        }
        gameState = G_APPLY;

        // go from simulation to draw
        if (gIsKeyReleased(32)) { // spacebar
            gameState = G_DRAW;
        }
        break;
    case G_APPLY:
        // take the simulation and apply its state change to the tiles
        for (int i = 0; i < NUM_SQUARES; i++) {
            squares[i].state = squares[i].nextState; // apply state change
        }
        gameState = G_SIMULATE;

        // go from simulation to draw
        if (gIsKeyReleased(32)) { // spacebar
            gameState = G_DRAW;
        }
        break;
    }
}

void draw() {
    if (gameState == G_SPLASH) {
        drawSplash();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < NUM_SQUARES; i++) {
        drawSquare(&squares[i]);
    }

    glFlush();
}

int main(int argc, char** argv) {
    setup();
    gSetup(argc, argv, 2560, 1440, true, update, draw); // at FS its 1440p
    return 0;
}
