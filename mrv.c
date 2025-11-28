#define _GNU_SOURCE
#include <glad/gl.h>
#include <GL/freeglut.h>
#include <EGL/egl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define EXTRA_RENDER

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 680
#define CELL_SIZE (WINDOW_WIDTH / 9)
#define GRID_SIZE 9
#define CELL_AMT (GRID_SIZE * GRID_SIZE)
#define TEST_AMT 100

char** tests;
clock_t* btimes;

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

void drawButtons() {
    glColor3f(0.8f, 0.8f, 0.8f);
    glRectf(WINDOW_WIDTH/2 - 50, WINDOW_HEIGHT - 60, 
           WINDOW_WIDTH/2 + 50, WINDOW_HEIGHT - 20);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(WINDOW_WIDTH/2 - 25, WINDOW_HEIGHT - 45, "Solve");

    glColor3f(0.8f, 0.8f, 0.8f);
    glRectf(20, WINDOW_HEIGHT - 20, 120, WINDOW_HEIGHT - 60);
    
    glColor3f(0.0f, 0.0f, 0.0f);
    drawText(35, WINDOW_HEIGHT - 45, "Run test");
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
    drawButtons();

    glutSwapBuffers();
}

/*int isValid(int box[GRID_SIZE][GRID_SIZE], int row, int col, int num) {
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
}*/

int isValid(int grid[GRID_SIZE][GRID_SIZE], int row, int col, int num) {
    for (int x = 0; x < GRID_SIZE; x++) {
        if (grid[row][x] == num || grid[x][col] == num)
            return 0;
    }

    int startRow = row - row % 3;
    int startCol = col - col % 3;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (grid[startRow + i][startCol + j] == num)
                return 0;

    return 1;
}

// ----------- MRV: ищем клетку с минимальным количеством допустимых значений ----------
int findMRVCell(int grid[GRID_SIZE][GRID_SIZE], int* bestRow, int* bestCol) {
    int minOptions = 10; // максимум - 9

    for (int row = 0; row < GRID_SIZE; row++) {
        for (int col = 0; col < GRID_SIZE; col++) {

            if (grid[row][col] != -1) continue;

            int count = 0;
            for (int num = 1; num <= 9; num++)
                if (isValid(grid, row, col, num))
                    count++;

            //if (count == 0) return 0;  // нет возможных значений ? тупик

            if (count < minOptions) {
                minOptions = count;
                *bestRow = row;
                *bestCol = col;
            }
        }
    }
    return (minOptions != 10);
}

// -------------------- Sudoku solver с MRV -----------------------
int sudokuSolver(int grid[GRID_SIZE][GRID_SIZE]) {
    int row, col;

    // выбрать клетку с минимальным количеством допустимых значений
    if (!findMRVCell(grid, &row, &col))
        return 1; // нет пустых — решение найдено

        // пробуем допустимые значения
        for (int num = 1; num <= 9; num++) {
            if (isValid(grid, row, col, num)) {
                grid[row][col] = num;

                if (sudokuSolver(grid))
                    return 1;

                grid[row][col] = -1; // откат
            }
        }

        return 0; // тупик
}


/*// solver function
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
}*/

void loadTests(char* filename) {
    FILE* fptr;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fptr = fopen(filename, "r");
    if (!fptr) {
        printf("ERROR: couldn't read test file");
        return;
    }

    int cnt = 0;
    tests = (char**)calloc(TEST_AMT, sizeof(char*));
    if (!tests) exit(-1);
    while ((read = getline(&line, &len, fptr)) != -1) {
        tests[cnt] = (char*)calloc(CELL_AMT, sizeof(char));
        if (!tests[cnt]) exit(-1);
        for (ssize_t i = 0; i < CELL_AMT; i++) {
            tests[cnt][i] = (line[i] == '_') ? -1 : line[i] - '0';
        }
        if (++cnt > TEST_AMT) break;
    }

    fclose(fptr);
    if (line) free(line);
}

void benchmark() {
    loadTests("tests");
    btimes = (clock_t*)calloc(TEST_AMT, sizeof(clock_t));
    clock_t stime = 0;

    for (int i = 0; i < TEST_AMT; i++) {
        for (int j = 0; j < CELL_AMT; j++) {
            sudokuGrid[j / GRID_SIZE][j % GRID_SIZE] = (int)tests[i][j];
        }
        btimes[i] = clock();
        sudokuSolver(sudokuGrid);
        btimes[i] = clock() - btimes[i];
        stime += btimes[i];
        glutPostRedisplay();
    }

    clock_t atime = stime / TEST_AMT;
    FILE* rptr = fopen("benchmark-report", "w");
    fprintf(rptr, "TOTAL: %Lf (%llu).\n", (long double)stime / (long double)CLOCKS_PER_SEC, (unsigned long long int)(stime));
    fprintf(rptr, "AVERAGE: %Lf (%llu).\n\n", (long double)atime / (long double)CLOCKS_PER_SEC, (unsigned long long int)(atime));
    fprintf(rptr, "CLOCKS PER SEC: %llu.\n\n", (unsigned long long int)(CLOCKS_PER_SEC));

    for (int i = 0; i < TEST_AMT; i++) {
        fprintf(rptr, "%03d %llu\n", i, (unsigned long long int)btimes[i]);
    }
    fclose(rptr);

    for (int i = 0; i < TEST_AMT; i++) free(tests[i]);
    free(tests);
    free(btimes);
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

        if(y > WINDOW_HEIGHT - 20 && y < WINDOW_HEIGHT - 60 &&
           x > 20 && x < 120) {
            // Benchmark button clicked
            benchmark();
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
    } else if (key == 32) { //Space
        benchmark();
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
