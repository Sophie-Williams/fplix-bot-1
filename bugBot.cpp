#include <map>
#include <set>
#include <cmath>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

#define PB push_back
#define MP make_pair
#define SZ(a) a.size()
#define DEBUG(x) cout << #x << " = " << x << endl;

#define PR(A,n)  { cout << #A << " = "; for (int i = 1, _n = (n); i <= _n; i++) \
                    cout << A[i] << ' '; cout << endl; }
#define PR0(A,n) { cout << #A << " = "; for (int i = 0, _n = (n); i < _n; i++) \
                    cout << A[i] << ' '; cout << endl; }

using namespace std;

const int nRows = 20;
const int nColumns = 30;
enum semanticMoves { RIGHT, UP, LEFT, DOWN };

int board[nRows + 5][nColumns + 5];
int nBots, botId, stable, unstable;
int curRow, curCol;
int lastMove;
int firstMove;
void initBoard(){
    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nColumns; j++){
            board[i][j] = 0;
        }
}

void printBoard(){
    for (int i = 0; i < nRows; i++){
        for (int j = 0; j < nColumns - 1; j++)
            cout << board[i][j];
        cout << board[i][nColumns - 1] << endl;
    }
}

void printMove(semanticMoves move){
    switch (move){
        case RIGHT:
            cout << "RIGHT" << endl;
            break;
        case UP:
            cout << "UP" << endl;
            break;
        case LEFT:
            cout << "LEFT" << endl;
            break;
        case DOWN:
            cout << "DOWN" << endl;
    }
}

bool isThisMoveValid(semanticMoves move){
    switch (move) {
        case RIGHT: {
            if ((curCol + 1 < nColumns) && (board[curRow][curCol + 1] != unstable))
                return true;
            return false;
        }
        case UP: {
            if ((curRow - 1 >= 0 ) && (board[curRow - 1][curCol] != unstable))
                return true;
            return false;
        }
        case LEFT: {
            if ((curCol - 1 >= 0 ) && (board[curRow][curCol - 1] != unstable))
                return true;
            return false;
        }
        case DOWN: {
            if ((curRow + 1 < nRows ) && (board[curRow + 1][curCol] != unstable))
                return true;
            return false;
        }
    }
}

void makeBestMove(){
    // int randomMove = rand() % 4;
    semanticMoves realMove;
    //
    // while (!isThisMoveValid(realMove)){
    //     randomMove = rand() % 4;
    //     semanticMoves realMove = static_cast<semanticMoves>(randomMove);
    // }
    for (int i = 0; i < 4; i ++){
         semanticMoves realMove = static_cast<semanticMoves>(i);
         if (isThisMoveValid(realMove))
            break;
    }
    lastMove = (realMove + 1) % 4;
    realMove = static_cast<semanticMoves>(lastMove);
    printMove(realMove);
}


int main() {
    int tempRow, tempCol;
    char temp;

    initBoard();
    // Read initial inputs
    cin >> nBots;
    cin >> botId;
    stable = 2 * botId - 1;
    unstable = stable + 1;
    //cout << "Unstable: " << " " << unstable << endl;

    while(true){
        if (!feof(stdin)){
            // Read current state of the board
            for (int i = 0; i < nRows; i++){
                for (int j = 0; j < nColumns - 1; j++){
                    cin >> temp;
                    board[i][j] = temp - 48;
                }
                cin >> temp;
            }

            // Read current position of all bots
            for (int i = 1; i <= nBots; i++){
                cin >> tempRow >> tempCol;
                if (i == botId){
                    curRow = tempRow;
                    curCol = tempCol;
                }
                //board[tempRow][tempCol] = 10 * i; //TODO: Mapping function
            }
            //printBoard();
            makeBestMove();
        }
    }
}
