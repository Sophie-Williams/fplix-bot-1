// v1.0.6

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
#include <queue>

#define PB push_back
#define MP make_pair
#define SZ(a) a.size()
#define DEBUG(x) cout << #x << " = " << x << endl;

#define PR(A,n)  { cout << #A << " = "; for (int iter = 1, _n = (n); iter <= _n; iter++) \
    cout << A[iter] << ' '; cout << endl; }
#define PR0(A,n) { cout << #A << " = "; for (int iter = 0, _n = (n); iter < _n; iter++) \
    cout << A[iter] << ' '; cout << endl; }

using namespace std;

struct Position {
    int x, y;
};

const int nRows = 20;
const int nColumns = 30;

const int dX[4] = {0, -1, 0, 1};
const int dY[4] = {1, 0, -1, 0};


enum semanticMoves { RIGHT, UP, LEFT, DOWN };

int board[nRows + 5][nColumns + 5];
int nBots, myBotId, myStableNumber, myUnstableNumber;
int curRow, curCol;
int lastMove;
int firstMove;

int prevRow, prevCol;

Position currentBotPosition[10];
Position trace[nRows + 5][nColumns + 5];
queue< pair<int, int> > q;
bool visited[nRows + 5][nColumns + 5];



void initBoard(){
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nColumns; j++){
            board[i][j] = 9;
        }

    }
}

void printBoard(){
    for (int i = 0; i < nRows; i++){
        for (int j = 0; j < nColumns; j++) {
            cout << board[i][j];
//            cout << board[i][nColumns - 1] << endl;
        }
        cout << endl;
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

bool isThisMoveValid(semanticMoves move, int& nextVal){
    semanticMoves lastSemanticMove;
    bool isFirstMove = false;
    if (lastMove == -1)
        isFirstMove = true;
    else
        lastSemanticMove = static_cast<semanticMoves>(lastMove);

    switch (move) {
        case RIGHT: {
                        if (!isFirstMove && lastSemanticMove == LEFT)
                            return false;
                        if ((curCol + 1 < nColumns) && (board[curRow][curCol + 1] != myUnstableNumber)){
                            nextVal = board[curRow][curCol + 1];
                            return true;
                        }
                        return false;
                    }
        case UP: {
                     if (!isFirstMove && lastSemanticMove == DOWN)
                         return false;
                     if ((curRow - 1 >= 0 ) && (board[curRow - 1][curCol] != myUnstableNumber)){
                         nextVal = board[curRow - 1][curCol];
                         return true;
                     }
                     return false;
                 }
        case LEFT: {
                       if (!isFirstMove && lastSemanticMove == RIGHT)
                           return false;
                       if ((curCol - 1 >= 0 ) && (board[curRow][curCol - 1] != myUnstableNumber)){
                           nextVal = board[curRow][curCol - 1];
                           return true;
                       }
                       return false;
                   }
        case DOWN: {
                       if (!isFirstMove && lastSemanticMove == UP)
                           return false;
                       if ((curRow + 1 < nRows ) && (board[curRow + 1][curCol] != myUnstableNumber)){
                           nextVal = board[curRow + 1][curCol];
                           return true;
                       }
                       return false;
                   }
    }
}

semanticMoves defensiveMove(){
    int i, nextVal;
    semanticMoves realMove;
    for (i = 0; i < 4; i++){
        realMove = static_cast<semanticMoves>(i);
        if (isThisMoveValid(realMove, nextVal))
            if (nextVal == myStableNumber)
                return realMove;
    }
}

semanticMoves noNameMove(){
    int i, nextVal;
    semanticMoves realMove;
    for (i = 0; i < 4; i++){
        realMove = static_cast<semanticMoves>(i);
        if (isThisMoveValid(realMove, nextVal))
            return realMove;
    }
}

semanticMoves fromPosition(int destRow, int destCol) {
    for (int i = 0; i <= 3; i++) {
        if (dX[i] == (destRow - curRow) && dY[i] == (destCol - curCol)) {
            return static_cast<semanticMoves>(i);
        }
    }
}

bool isPosOfAnotherBot(int x, int y) {
    for (int i = 1; i <= nBots - 1; i++) {
        if (x == currentBotPosition[i].x && y == currentBotPosition[i].y) {
            return true;
        }
    }

    return false;
}

bool isStableCell(int x, int y) {
    return (board[x][y] == myStableNumber);
}

bool isInsideBoard(int x, int y) {
    return ((x >= 0 && x < nRows) && (y >= 0 && y < nColumns));
}

int numAdjStableCell(int x, int y) {
    int count = 0;
    for (int i = 0; i <= 3; i++) {
        int uu = x + dX[i];
        int vv = y + dY[i];
        if (isStableCell(uu, vv)) {
            count++;
        }
    }

    return count;
}

semanticMoves safeStrategyFromStable() {
    // reset q and visited
    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));


    q.push(make_pair(curRow, curCol));
    visited[curRow][curCol] = true;

    bool haveSecondaryPos = false;
    Position secondaryPos;

    bool havePrimaryPos = false;
    Position primaryPos;
    
    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();
        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];
            semanticMoves move = static_cast<semanticMoves>(i);
            if (visited[uNext][vNext]) continue;
            if (isStableCell(uNext, vNext)) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));    
                trace[uNext][vNext] = {u, v};
            } else {
                if (isInsideBoard(uNext, vNext)) {         
                    if (numAdjStableCell(uNext, vNext) == 2) {
                        // uNext, vNext is the expected position.
                        primaryPos = {uNext, vNext};
                        havePrimaryPos = true;
                        trace[uNext][vNext] = {u, v};
                        break;
                    } else {
                        if (!haveSecondaryPos) {
                            haveSecondaryPos = true;
                            secondaryPos = {uNext, vNext};
                            trace[uNext][vNext] = {u, v};
                        }
                    }
                }
            }
        }
        if (havePrimaryPos) break;
    }

    if (!havePrimaryPos) {
        havePrimaryPos = true;
        primaryPos = secondaryPos;
    }

    int u = primaryPos.x;
    int v = primaryPos.y;

    while (true) {
        int uu = trace[u][v].x;
        int vv = trace[u][v].y;

        if (uu == curRow && vv == curCol) {
            return fromPosition(u, v);
        }

        u = uu;
        v = vv;
    }
}

bool isDangerous() {
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nColumns; j++) {
            if (board[i][j] == myUnstableNumber) {
                for (int k = 1; k <= nBots - 1; k++) {
                    int u = currentBotPosition[k].x;
                    int v = currentBotPosition[k].y;

//                    DEBUG(u);
//                    DEBUG(v);
                    if (abs(u - i) + abs(v - j) <= 5) return true;
                }
            }
        }
    }
    
    return false;
}

semanticMoves safeStrategyFromUnstable() {
    int nextVal;

    // di vao o an toan (den duoc stable trong 1 nuoc)
    //
 
//    cout << isDangerous() << endl;
//    cout << myUnstableNumber << endl;

    if (!isDangerous()) {
        for (int i = 0; i <= 3; i++) {
            int uu = curRow + dX[i];
            int vv = curCol + dY[i];
            semanticMoves move = static_cast<semanticMoves>(i);
            if (isThisMoveValid(move, nextVal)) {
                if (!isStableCell(uu, vv)) {
                    bool ok = false;
                    for (int j = 0; j <= 3; j++) {
                        if (uu + dX[j] != curRow || vv + dY[j] != curCol) {
                            if (isStableCell(uu + dX[j], vv + dY[j])) {
                                return move;
                            }
                        }       
                    }
                }
            }
        }
    }




    for (int i = 0; i <= 3; i++) {
        int uu = curRow + dX[i];
        int vv = curCol + dY[i];
        semanticMoves move = static_cast<semanticMoves>(i);

        if (isThisMoveValid(move, nextVal)) {
            if (isStableCell(uu, vv)) {
               return move;
            }
        }
    }

    // TODO: Co truong hop nao ma minh dang dung o unstable, nhung khong co stable de di?
    // neu khong co stable de di -> tim stable gan nhat
    
    //random move
    
    for (int i = 0; i <= 3; i++) {
        int uu = curRow + dX[i];
        int vv = curCol + dY[i];
        semanticMoves move = static_cast<semanticMoves>(i);

        if (isThisMoveValid(move, nextVal)) {
            return move;
        }
    }

}

// safe stratery move: 
// if current pos is adjacent to another bot -> go to that bot's pos             (1)
//
// if current pos is stable: find an empty cell that beside at lease 2 stable cell
// if current pos is unstable: 
//      + if we find a adj cell that from it we can go to stable area in 1 move -> move to it
//      + else: go to the adj stable cell.
//

semanticMoves safeStrategyMove() {
    int nextVal;
    bool isFirstMove = false;

    if (lastMove == -1)
        isFirstMove = true;


    for (int i = 0; i <= 3; i++) {
        int nextRow = curRow + dX[i];
        int nextCol = curCol + dY[i];
        semanticMoves move = static_cast<semanticMoves>(i);
        
        // (1)
        if (isThisMoveValid(move, nextVal)) {
            if (isPosOfAnotherBot(nextRow, nextCol) && board[nextRow][nextCol] % 2 == 0) {   
                return move;
            }    
        }      
    }

    if (board[curRow][curCol] == myStableNumber) {
        return safeStrategyFromStable();
    } else {
        return safeStrategyFromUnstable();
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

    int i;
    int nextVal;

    // You want to try a new strategy?
    // Please create a method with descriptive name and call it like this
    // realMove = noNameMove();
//    realMove = defensiveMove();
    
    realMove = safeStrategyMove();

    lastMove = realMove;
    printMove(realMove);
}


int main() {

//    freopen("bug.txt", "r", stdin);
    int tempRow, tempCol;
    char temp;

    initBoard();
    // Read initial inputs
    cin >> nBots;
    cin >> myBotId;

    myStableNumber = 2 * myBotId - 1;
    myUnstableNumber = myStableNumber + 1;
    lastMove = -1;
    //cout << "myUnstableNumber: " << " " << myUnstableNumber << endl;

    prevRow = -1;
    prevCol = -1;


    while(true){
        if (!feof(stdin)){
            // Read current state of the board
            for (int i = 0; i < nRows; i++){
                for (int j = 0; j < nColumns; j++){
                    cin >> temp;
                    board[i][j] = temp - 48;
                }
            }
            
//            printBoard();

            int otherBotCounts = 0;
            // Read current position of all bots
            for (int i = 1; i <= nBots; i++){
                cin >> tempRow >> tempCol;
                if (i == myBotId){
                    curRow = tempRow;
                    curCol = tempCol;
                } else {
                    otherBotCounts++;
                    currentBotPosition[otherBotCounts] = {tempRow, tempCol};
                }
                //board[tempRow][tempCol] = 10 * i; //TODO: Mapping function
            }
            // printBoard();
            makeBestMove();
        }
    }
}
