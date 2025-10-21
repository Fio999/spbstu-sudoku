#include <glad/gl.h>
#include <GL/freeglut.h>
#include <EGL/egl.h>
#include <stdio.h>
#include <string.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 680
#define CELL_SIZE (WINDOW_WIDTH / 9)
#define GRID_SIZE 9

int sudokuGrid[9][9];
int selectedRow = -1, selectedCol = -1;

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text);
}

void drawGrid() {
    glColor3f(0.5f, 0.5f, 0.5f);
    
    for(int i = 0; i <= 9; i++) {
        if (i % 3 == 0) {
            glLineWidth(2.0f);
        } else {
            glLineWidth(1.0f);
        }

        glBegin(GL_LINES);
        glVertex2f(i * CELL_SIZE, 0);
        glVertex2f(i * CELL_SIZE, WINDOW_WIDTH);
        glEnd();
        
        glBegin(GL_LINES);
        glVertex2f(0, i * CELL_SIZE);
        glVertex2f(WINDOW_WIDTH, i * CELL_SIZE);
        glEnd();
    }
}

void drawNumbers() {
    char str[2];
    str[1] = '\0';
    
    glColor3f(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < 9; i++) {
        for(int j = 0; j < 9; j++) {
            if(sudokuGrid[i][j] != -1) {
                str[0] = sudokuGrid[i][j] + '0';
                drawText(j * CELL_SIZE + CELL_SIZE/2 - 4, 
                        (9 - i) * CELL_SIZE - CELL_SIZE/2 - 8, str);
            }
        }
    }
}

void drawButton() {
    glColor3f(0.8f, 0.8f, 0.8f);
    glRectf(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT - 60, 
           WINDOW_WIDTH/2 + 50, WINDOW_HEIGHT - 20);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(WINDOW_WIDTH/2 - 30, WINDOW_HEIGHT - 55, "Solve");
}

void display() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_WIDTH, 0, WINDOW_HEIGHT);

    drawGrid();

    if(selectedRow != -1 && selectedCol != -1) {
        glColor4f(0.3f, 0.3f, 1.0f, 0.3f);
        glRectf(selectedCol * CELL_SIZE, (9 - selectedRow - 1) * CELL_SIZE,
                (selectedCol + 1) * CELL_SIZE, (9 - selectedRow) * CELL_SIZE);
    }

    drawNumbers();
    drawButton();

    glutSwapBuffers();
}

int isValid(int box[GRID_SIZE][GRID_SIZE], int row, int col, int num) {
    for (int x = 0; x < GRID_SIZE; x++) {
        if (box[row][x] == num || box[x][col] == num)
            return 0;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (box[startRow + i][startCol + j] == num)
                return 0;

    return 1;
}

// solver function
int sudokuSolver(int grid[9][9]) {
    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {
            if (grid[row][col] == -1) {
                for (int num = 1; num <= 9; num++) {
                    if (isValid(grid, row, col, num)) {
                        grid[row][col] = num;
                        if (sudokuSolver(grid))
                            return 1;
                        grid[row][col] = -1;
                    }
                }
                return 0;
            }
        }
    }
    return 1;
}

void mouseClick(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        y = WINDOW_HEIGHT - y;
        
        if(y > WINDOW_HEIGHT - 60 && y < WINDOW_HEIGHT - 20 &&
           x > WINDOW_WIDTH/2 - 50 && x < WINDOW_WIDTH/2 + 50) {
            // Solve button clicked
            sudokuSolver(sudokuGrid);
            glutPostRedisplay();
            return;
        }

        //if(x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_WIDTH) {
        //    selectedRow = selectedCol = -1;
        //    return;
        //}

        selectedCol = x / CELL_SIZE;
        selectedRow = 8 - (y / CELL_SIZE);
        glutPostRedisplay();
    }
}

void keyboard(unsigned char key, int x, int y) {
    if (selectedRow != -1 && selectedCol != -1) {
        if (key >= '1' && key <= '9') {
            sudokuGrid[selectedRow][selectedCol] = key - '0';
        } else if (key == 8) { // Backspace
            sudokuGrid[selectedRow][selectedCol] = -1;
        }
    }

    if (key == 'w') {
        selectedRow--;
    } else if (key == 's') {
        selectedRow++;
    } else if (key == 'a') {
        selectedCol--;
    } else if (key == 'd') {
        selectedCol++;
    }

    if (key == 27) { //Escape
        selectedCol = selectedRow = -1;
    } else {
        if (selectedRow < 0) {
            selectedRow = 8;
        } else if (selectedRow > 8) {
            selectedRow = 0;
        }
        if (selectedCol < 0) {
            selectedCol = 8;
        } else if (selectedCol > 8) {
            selectedCol = 0;
        }
    }

    if (key == 127) { //Delete
        memset(sudokuGrid, -1, sizeof(sudokuGrid));
    } else if (key == 13) { //Enter
        sudokuSolver(sudokuGrid);
    }

    glutPostRedisplay();
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);

    //glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    memset(sudokuGrid, -1, sizeof(sudokuGrid));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Sudoku Solver");

    if(!gladLoadGL(eglGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouseClick);
    glutKeyboardFunc(keyboard);

    glutMainLoop();
    return 0;
}
