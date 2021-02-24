#include <iostream>
#include <windows.h>    // SetConsoleCursorPosition()
#include <conio.h>      // __kbhit()
#include <ctime>        // clock()
#include <string>
#include <algorithm>    // min_element()
using namespace std;

int g_Board_X = 13;      // MODIFIABLE
int g_Board_Y = 23;     // MODIFIABLE
string CODE[7][4];

void printBoard(void) {
    for (int i = 0; i < g_Board_Y + 2; ++i) {
        for (int j = 0; j < g_Board_X + 2; ++j) {
            if (i == 0 || i == g_Board_Y + 1 || j == 0 || j == g_Board_X + 1)
                cout << "□";
            else
                cout << "  ";
        }
        cout << endl;
    }
}

void cursorsetting() {
    HANDLE consolHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ConsoleCursor;
    ConsoleCursor.bVisible = false;
    ConsoleCursor.dwSize = 1;
    SetConsoleCursorInfo(consolHandle, &ConsoleCursor);
}

void gotoxy(int x, int y) {
    COORD Pos;
    Pos.X = x;
    Pos.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Pos);
}


class Stack_Block {
private:
    bool** m_stack = new bool* [g_Board_Y + 2];
    friend class Block;

public:
    Stack_Block() {
        // dynamic allocation of 2D stack array
        for (int i = 0; i < g_Board_Y + 2; i++)
            m_stack[i] = new bool[g_Board_X + 2]{ false };

        // Board Frame
        for (int i = 0; i < g_Board_X + 2; i++) {   // horizontal
            m_stack[0][i] = true;
            m_stack[g_Board_Y + 1][i] = true;
        }
        for (int i = 0; i < g_Board_Y + 2; i++) {   // vertical
            m_stack[i][0] = true;
            m_stack[i][g_Board_X + 1] = true;
        }

    }
    
    // Print Stack Method
    void Print_Stack(void) {
        gotoxy(0, 0);
        for (int i = 1; i < g_Board_Y + 1; i++) {
            cout << endl << "□";
            for (int j = 1; j < g_Board_X + 1; j++) {
                if (m_stack[i][j] == true)
                    cout << "▣";
                else
                    cout << "  ";
            }
        }
    }

    // Check Stack Method
    bool Check_Stack(int y) {
        for (int i = 1; i < g_Board_X + 1; i++)
            if (!m_stack[y][i])
                return false;
        return true;
    }

    // Delete Stack Method
    void Delete_Stack(int y, int score) {
        // replace i th row with i-1 th row
        for (int j = 0; j < y - 2; j++)
            for (int i = 1; i < g_Board_X + 1; i++)
                m_stack[y - j][i] = m_stack[y - j - 1][i];
        for (int i = 1; i < g_Board_X + 1; i++)
            m_stack[1][i] = false;

        gotoxy(g_Board_X * 2 + 5, 20);
        cout << "    Score : " << score;
    }

};

class Block {
private:
    int m_shape;       // Block's shape
    int m_direction;   // Block's direction
    int m_x = 4;       // Location
    int m_y = 4;
    int m_score = 0;
    Stack_Block* StackPtr;

public:
    // Consturctor : random shape assignment using system clock
    Block(int shape = clock() % 7, int direction = clock() % 4) : m_shape(shape), m_direction(direction) {
        StackPtr = new Stack_Block;
    }

    // Command Method
    void Command(char input) {
        // Rotation
        if (input == 72) {  // UP key
            Rotate();
            if (Detect_Block()) {
                Rotate_Inverse();
                return;
            }
            return;
        }
        else if (input == 80) {     // DOWN key
            Drop();
            return;
        }
        // Translation
        else if (input == 75) {
            --m_x;
            if (Detect_Block()) {
                m_x++;
                return;
            }
            return;
        }
        else if (input == 77) {
            ++m_x;
            if (Detect_Block()) {
                m_x--;
                return;
            }
            return;
        }
    }

    // Rotate Method
    void Rotate() {
        ++m_direction;
        m_direction %= 4;
    }

    // Rotate Inverse Method
    void Rotate_Inverse() {
        --m_direction;
        m_direction += 4;
        m_direction %= 4;
    }

    // Drop Method
    void Drop() {
        ++m_y;
        if (Detect_Block()) {
            m_y--;
            Stack();
            Nextdrop();
        }
    }

    // Print Method
    void Print() {
        int p_x(0), p_y(0);

        for (int i = 0; i < (((m_shape == 2) || (m_shape == 3)) ? 5 : 4); i++) {
            if (CODE[m_shape][m_direction][i] == 'O') {
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "■";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'L') {
                p_x--;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "■";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'R') {
                p_x++;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "■";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'U') {
                p_y--;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "■";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'B') {
                if (CODE[m_shape][m_direction][i - 1] == 'L') {
                    p_x++;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'R') {
                    p_x--;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'U') {
                    p_y++;
                    continue;
                }
            }
        }
    }

    // Erase Method
    void Erase() {
        int p_x(0), p_y(0);

        for (int i = 0; i < (((m_shape == 2) || (m_shape == 3)) ? 5 : 4); i++) {
            if (CODE[m_shape][m_direction][i] == 'O') {
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "  ";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'L') {
                p_x--;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "  ";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'R') {
                p_x++;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "  ";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'U') {
                p_y--;
                gotoxy((m_x + p_x) * 2, m_y + p_y);
                cout << "  ";
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'B') {
                if (CODE[m_shape][m_direction][i - 1] == 'L') {
                    p_x++;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'R') {
                    p_x--;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'U') {
                    p_y++;
                    continue;
                }
            }
        }
    }

    // Detect_Block Method
    bool Detect_Block() {
        int p_x(0), p_y(0);

        for (int i = 0; i < (((m_shape == 2) || (m_shape == 3)) ? 5 : 4); i++) {
            if (CODE[m_shape][m_direction][i] == 'O') {
                if (StackPtr->m_stack[m_y + p_y][m_x + p_x])
                    return true;
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'L') {
                p_x--;
                if (StackPtr->m_stack[m_y + p_y][m_x + p_x])
                    return true;
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'R') {
                p_x++;
                if (StackPtr->m_stack[m_y + p_y][m_x + p_x])
                    return true;
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'U') {
                p_y--;
                if (StackPtr->m_stack[m_y + p_y][m_x + p_x])
                    return true;
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'B') {
                if (CODE[m_shape][m_direction][i - 1] == 'L') {
                    p_x++;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'R') {
                    p_x--;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'U') {
                    p_y++;
                    continue;
                }
            }
        }
        return false;
    }

    // Stack Method ~ Block_Stack Class
    void Stack() {
        int p_x(0), p_y(0);

        for (int i = 0; i < (((m_shape == 2) || (m_shape == 3)) ? 5 : 4); i++) {
            if (CODE[m_shape][m_direction][i] == 'O') {
                StackPtr->m_stack[m_y + p_y][m_x + p_x] = true;
                if (StackPtr->Check_Stack(m_y + p_y)) {
                    m_score++;
                    StackPtr->Delete_Stack(m_y + p_y, m_score);
                    p_y++;
                }
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'L') {
                p_x--;
                StackPtr->m_stack[m_y + p_y][m_x + p_x] = true;
                if (StackPtr->Check_Stack(m_y + p_y)) {
                    m_score++;
                    StackPtr->Delete_Stack(m_y + p_y, m_score);
                    p_y++;
                }
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'R') {
                p_x++;
                StackPtr->m_stack[m_y + p_y][m_x + p_x] = true;
                if (StackPtr->Check_Stack(m_y + p_y)) {
                    m_score++;
                    StackPtr->Delete_Stack(m_y + p_y, m_score);
                    p_y++;
                }
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'U') {
                p_y--;
                StackPtr->m_stack[m_y + p_y][m_x + p_x] = true;
                if (StackPtr->Check_Stack(m_y + p_y)) {
                    m_score++;
                    StackPtr->Delete_Stack(m_y + p_y, m_score);
                    p_y++;
                }
                continue;
            }
            else if (CODE[m_shape][m_direction][i] == 'B') {
                if (CODE[m_shape][m_direction][i - 1] == 'L') {
                    p_x++;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'R') {
                    p_x--;
                    continue;
                }
                else if (CODE[m_shape][m_direction][i - 1] == 'U') {
                    p_y++;
                    continue;
                }
            }
        }
        StackPtr->Print_Stack();
    }

    // Nextdrop Method
    void Nextdrop() {
        m_shape = clock() % 7;
        m_direction = clock() % 4;
        m_x = 4;
        m_y = 4;
        if (Detect_Block())
            cout << "game end" << endl;
    }
};

int main() {

    CODE[0][0] = "OUUU";
    CODE[0][1] = "ORRR";
    CODE[0][2] = "OUUU";
    CODE[0][3] = "ORRR";

    CODE[1][0] = "OURU";
    CODE[1][1] = "RLUL";
    CODE[1][2] = "OURU";
    CODE[1][3] = "RLUL";

    CODE[2][0] = "OUBRU";
    CODE[2][1] = "OUBRU";
    CODE[2][2] = "OUBRU";
    CODE[2][3] = "OUBRU";

    CODE[3][0] = "LRUBR";
    CODE[3][1] = "OURBU";
    CODE[3][2] = "OURBL";
    CODE[3][3] = "OULBU";

    CODE[4][0] = "LRRU";
    CODE[4][1] = "RLUU";
    CODE[4][2] = "LURR";
    CODE[4][3] = "OUUL";

    CODE[5][0] = "OULU";
    CODE[5][1] = "LRUR";
    CODE[5][2] = "OULU";
    CODE[5][3] = "LRUR";

    CODE[6][0] = "RLLU";
    CODE[6][1] = "OUUR";
    CODE[6][2] = "RULL";
    CODE[6][3] = "LRUU";

    int input;
    bool time_flag = false;
    float time_per_drop = 1;


    cout << "Game Start!" << endl;
    cout << "Horizontal Size of the Board : ";
    cin >> g_Board_X;
    cout << "Vertical Size of the Board : ";
    cin >> g_Board_Y;
    cout << "Time per Drop(sec) : ";
    cin >> time_per_drop;

    system("cls");
    cursorsetting();
    printBoard();


    Block dropping_block;
    dropping_block.Detect_Block();
    dropping_block.Print();

    clock_t start = 0;
    clock_t end = 0;
    clock_t interval;

    while (true) {
        
        if (_kbhit()) {
            input = _getch();
            if (input == 224) {
                input = _getch();
                dropping_block.Erase();
                dropping_block.Command(input);
                dropping_block.Print();
            }
        }

        if (((clock() % int(time_per_drop*1000)) < 100) && time_flag == false) {
            end = clock();
            interval = end - start;
            start = clock();
            gotoxy(g_Board_X * 2 + 5, 18);
            cout << "interval : " << interval << "   ";

            dropping_block.Erase();
            dropping_block.Drop();
            dropping_block.Print();
            time_flag = true;
        }
        else if ((clock() % int(time_per_drop*1000)) > 100)
            time_flag = false;

    }
    return 0;
}
