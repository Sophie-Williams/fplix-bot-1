
// v1.2.7

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

const int oo = 100000;
const int ThresHold = 0;
const int HomeThresHold = 4;

const int nRows = 20;
const int nColumns = 30;

const int dX[4] = {0, -1, 0, 1};
const int dY[4] = {1, 0, -1, 0};

enum semanticMoves { RIGHT, UP, LEFT, DOWN };

int board[nRows + 5][nColumns + 5];
int stableScore[nRows + 5][nColumns + 5];
int nBots, myBotId, myStableNumber, myUnstableNumber;
int curRow, curCol;
int lastMove;
int firstMove;

int prevRow, prevCol;

Position currentBotPosition[10];
Position trace[nRows + 5][nColumns + 5];
queue< pair<int, int> > q;
bool visited[nRows + 5][nColumns + 5];

Position currentDestination = {-1, -1};
Position startPostition = {-1, -1};
vector<Position> exDestination;

int *perm;

semanticMoves moveBeforeGoOutTo(int u, int v);
semanticMoves otherStrategyMove();
semanticMoves greedyMove();

int f[nRows + 5][nColumns + 5];

int lastMaxToHome;
bool isDesToHome;
int cntMaxMove = 0;

// variables here
int sample[12][4] = {{0, 1, 2, 3}, {1, 2, 0, 3}, {2, 1, 0, 3}, {3, 2, 0, 1}, {0, 1, 3, 2},
                    {0, 2, 1, 3}, {0, 2, 3, 1}, {0, 3, 1, 2}, {0, 3, 2, 1},
                    {1, 2, 3, 0}, {1, 3, 0, 2}, {3, 0, 1, 2}};

int *getPerm() {
	srand (time(NULL));
	int r = rand() % 12;
	int *perm = sample[r];

	return perm;
}

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

bool isStableCell(int x, int y) {
    return (board[x][y] == myStableNumber);
}

bool haveTwoOpositeUnstable(int u, int v) {
    bool check[4];
    int cnt = 0;
    for (int i = 0; i <= 3; i++) {
        int uu = u + dX[i];
        int vv = v + dY[i];
        if (!isStableCell(uu, vv)) {
            check[i] = true;
            cnt ++;
        }
        else check[i] = false;
    }

    if (cnt == 2) {
         if (check[0] && check[2]) return true;
         if (check[1] && check[3]) return true;
    }

    return false;
    
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

int distanceToAnotherBot(int x, int y) {
    int res = 0;
    for (int i = 1; i <= nBots - 1; i++) {
        int u = currentBotPosition[i].x;
        int v = currentBotPosition[i].y;

        if (abs(u - x) + abs(v - y) > res)
            res = abs(u - x) + abs(v - y);
    }

    return res;
}

semanticMoves dumbStableMove() {
    int nextVal;
    // CASE: have a stable cell to arrive.
    // --> random move to that stable cell
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

    for (int i = 0; i <= 3; i++) {
        int uu = curRow + dX[i];
        int vv = curCol + dY[i];
        semanticMoves move = static_cast<semanticMoves>(i);

        if (isThisMoveValid(move, nextVal)) {
            return move;
        }
    }

    return static_cast<semanticMoves>(0);
}

semanticMoves moveBeforeGoOutTo(int u, int v) {

    for (int i = 1; i <= nBots - 1; i++) {
        int xBot = currentBotPosition[i].x;
        int yBot = currentBotPosition[i].y;

        if (abs(xBot - curRow) + abs(yBot - curCol) <= 5) {
            exDestination.push_back({u, v});

            return otherStrategyMove();
        }
    }

    exDestination.clear();
    currentDestination = {-1, -1};
    cntMaxMove = 0;
    startPostition = {u, v};
    return fromPosition(u, v);

}

semanticMoves greedyMove() {
//    DEBUG("greedy");
//    DEBUG(currentDestination.x);
//    DEBUG(currentDestination.y);


    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));
    memset(trace, 0, sizeof(trace));
    memset(f, 0, sizeof(f));

    q.push(make_pair(curRow, curCol));
    visited[curRow][curCol] = true;
    f[curRow][curCol] = 0;

    int nextVal;

//    visited[curRow - dX[lastMove]][curCol - dY[lastMove]] = true;

    bool ok = false;

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);
            if (u == curRow && v == curCol) {
                if (!isThisMoveValid(move, nextVal)) continue;
            }
            
            if (!isInsideBoard(uNext, vNext)) continue;
            if (visited[uNext][vNext]) continue;
            //            if (uNext == uEx && vNext == vEx) continue;
            
            if (numAdjStableCell(u, v) == 1) continue;
            if (isStableCell(uNext, vNext)) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
//                f[uNext][vNext] = f[u][v] + 1;
                trace[uNext][vNext] = {u, v};
            } else {
                if (haveTwoOpositeUnstable(u, v)) continue;
                if (uNext == currentDestination.x && vNext == currentDestination.y) {
                    trace[uNext][vNext] = {u, v};
                    ok = true;
                    break;
                }
            }
        }
        if (ok) break;
    }

    if (!ok) {

        int maxDis = -1;
        semanticMoves maxMove;
        
        for (int i = 0; i < 4; i++) {
            int uNext = curRow + dX[i];
            int vNext = curCol + dY[i];
            semanticMoves move = static_cast<semanticMoves>(i);

            if (isThisMoveValid(move, nextVal)) {
                int dis = distanceToAnotherBot(uNext, vNext);
                if (dis > maxDis) {
                    maxDis = dis;
                    maxMove = move;
                } 
            }
        }

        return maxMove;

    } else {

        int u = currentDestination.x;
        int v = currentDestination.y;
        
        while (true) {
            int uu = trace[u][v].x;
            int vv = trace[u][v].y;
            
            if (uu == curRow && vv == curCol) {

                if (u == currentDestination.x && v == currentDestination.y) {
                    return moveBeforeGoOutTo(u, v);
                }

                return fromPosition(u, v);
            }
            u = uu;
            v = vv;
        }
    }

    

}


semanticMoves otherStrategyMove() {

    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));
    //    memset(trace, 0, sizeof(trace));
    memset(f, 0, sizeof(f));

    q.push(make_pair(curRow, curCol));
    visited[curRow][curCol] = true;
    f[curRow][curCol] = 0;

    bool havePrimaryPos = false;
    Position primaryPos;
    int primDis = 100000;

    bool haveSecondaryPos = false;
    Position secondaryPos;
    int sedDis = 100000;

    for (int i = 0; i < exDestination.size(); i++) {
        visited[exDestination[i].x][exDestination[i].y] = true;
    }

    int nextVal;

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);
            
            if (!isInsideBoard(uNext, vNext)) continue;
            if (u == curRow && v == curCol) {
                if (!isThisMoveValid(move, nextVal)) continue;
            }

            if (visited[uNext][vNext]) continue;
            if (isPosOfAnotherBot(uNext, vNext)) continue;
            if (numAdjStableCell(u, v) == 1) continue;

            if (isStableCell(uNext, vNext)) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
                f[uNext][vNext] = f[u][v] + 1;
            } else {
                if (isInsideBoard(uNext, vNext)) {
                    if (haveTwoOpositeUnstable(u, v)) continue;
                    if (numAdjStableCell(uNext, vNext) >= 2) {
                        // uNext, vNext is the expected position.
                        primaryPos = {uNext, vNext};
                        havePrimaryPos = true;
                        f[uNext][vNext] = f[u][v] + 1;
                        primDis = f[uNext][vNext];
                        break;
                    } else {
                        if (!haveSecondaryPos) {
                            haveSecondaryPos = true;
                            secondaryPos = {uNext, vNext};
                            f[uNext][vNext] = f[u][v] + 1;
                            sedDis = f[uNext][vNext];

                        }
                    }
                }
            }
        }
        if (havePrimaryPos) break;
    }

    if (havePrimaryPos || haveSecondaryPos) {
        if (!havePrimaryPos) {
            havePrimaryPos = true;
            primaryPos = secondaryPos;

        } else {
            if (haveSecondaryPos) {
                if (sedDis + ThresHold < primDis) {
                    havePrimaryPos = true;
                    primaryPos = secondaryPos;
                }
            }
        }

        currentDestination = primaryPos;
//                DEBUG(currentDestination.x);
//                DEBUG(currentDestination.y);

        return greedyMove();
    }

    return dumbStableMove();

}


semanticMoves safeStrategyFromStable() {
    // reset q and visited
    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));
    memset(trace, 0, sizeof(trace));
    memset(f, 0, sizeof(f));


    q.push(make_pair(curRow, curCol));
    visited[curRow][curCol] = true;
    f[curRow][curCol] = 0;

    bool haveSecondaryPos = false;
    Position secondaryPos;
    int sedDis = 100000;

    bool havePrimaryPos = false;
    Position primaryPos;
    int primDis = 100000;

     for (int i = 0; i < exDestination.size(); i++) {
        visited[exDestination[i].x][exDestination[i].y] = true;
    }
    
    int nextVal;

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);
            
            if (!isInsideBoard(uNext, vNext)) continue;
            if (u == curRow && v == curCol) {
                if (!isThisMoveValid(move, nextVal)) continue;
            }
            if (visited[uNext][vNext]) continue;
            if (isPosOfAnotherBot(uNext, vNext)) continue;
            if (numAdjStableCell(u, v) == 1) continue; 

            if (isStableCell(uNext, vNext)) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
                trace[uNext][vNext] = {u, v};
                f[uNext][vNext] = f[u][v] + 1;
            } else {
                if (isInsideBoard(uNext, vNext)) {
                    if (haveTwoOpositeUnstable(u, v)) continue; 
                    if (numAdjStableCell(uNext, vNext) >= 2) {
                        // uNext, vNext is the expected position.
                        primaryPos = {uNext, vNext};
                        havePrimaryPos = true;
                        trace[uNext][vNext] = {u, v};
                        f[uNext][vNext] = f[u][v] + 1;
                        primDis = f[uNext][vNext];
                        break;
                    } else {
                        if (!haveSecondaryPos) {
                            haveSecondaryPos = true;
                            secondaryPos = {uNext, vNext};
                            trace[uNext][vNext] = {u, v};
                            f[uNext][vNext] = f[u][v] + 1;
                            sedDis = f[uNext][vNext];
                        }
                    }
                }
            }
        }
        if (havePrimaryPos) break;
    }


    if (havePrimaryPos || haveSecondaryPos) {
        if (!havePrimaryPos) {
            havePrimaryPos = true;
            primaryPos = secondaryPos;
        } else {
            if (haveSecondaryPos) {
                if (sedDis + ThresHold < primDis) {
                    havePrimaryPos = true;
                    primaryPos = secondaryPos;
                }
            }
        }

        currentDestination = primaryPos;
//        DEBUG(currentDestination.x);
//        DEBUG(currentDestination.y);

        return greedyMove();

    }
    return dumbStableMove();
}



bool isDangerous() {

    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nColumns; j++) {
            if (board[i][j] == myUnstableNumber) {
                for (int k = 1; k <= nBots - 1; k++) {
                    int u = currentBotPosition[k].x;
                    int v = currentBotPosition[k].y;

                    if (curRow == i && curCol == j) {
                        if (abs(u - i) + abs(v - j) <= 4) return true;
                    } else
                    if (abs(u - i) + abs(v - j) <= 2) return true;
                }
            }
        }
    }

    return false;
}

int distanceToStable(int u, int v) {
    int res = 10000000;
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nColumns; j++) {
            if (isStableCell(i, j)) {
                if (abs(u - i) + abs(v - j) < res) {
                    res = abs(u - i) + abs(v - j);
                    
                }
            }
        }
    }

    return res;
}

int matDis(Position p1, Position p2) {
    return (abs(p1.x - p2.x) + abs(p1.y - p2.y));
}


int emptyCellsDistance(Position start, Position traceToHome[nRows + 5][nColumns +5], Position& desStable) {

    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));
    memset(f, 0, sizeof(f));

    q.push(make_pair(start.x, start.y));
    visited[start.x][start.y] = true;
    f[start.x][start.y] = 0;

    visited[curRow][curCol] = true;

    int res = oo;

    if (isStableCell(start.x, start.y)) {
        desStable = start;
        return 0;
    }

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);

            if (visited[uNext][vNext]) continue;
            if (!isInsideBoard(uNext, vNext)) continue;
            if (distanceToStable(uNext, vNext) + f[u][v] + 2 >= distanceToAnotherBot(uNext, vNext)) continue;
//            if (uNext == 5 && vNext == 3) {
//                cout << "VT = " << distanceToStable(uNext, vNext) << endl;
//                cout << "f[u][v] = " << f[u][v] << endl;
//                cout << "VP = " << distanceToAnotherBot(uNext, vNext) << endl;
//                DEBUG("5, 3 safe");
//            }

            if (board[uNext][vNext] != myUnstableNumber) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
                f[uNext][vNext] = f[u][v] + 1;
                traceToHome[uNext][vNext] = {u, v};

                if (isStableCell(uNext, vNext) && stableScore[uNext][vNext] > f[uNext][vNext]) {
                    if (f[uNext][vNext] < res)  {
                        res = f[uNext][vNext];
                        desStable = {uNext, vNext};

                    }

                }
            }
        }
    }
    return res;
}

int distanceFromOtherBotToUnstable(int xNext, int yNext) {
    int res = oo;

    for (int k = 1; k <= nBots - 1; k++) {
        int u = currentBotPosition[k].x;
        int v = currentBotPosition[k].y;

        for (int i = 0; i < nRows; i++) {
            for (int j = 0; j < nColumns; j++) {
                if (board[i][j] == myUnstableNumber) {

                    if (abs(u - i) + abs(v - j) < res)
                        res = abs(u - i) + abs(v - j);
                }
            }
        }

        if (abs(xNext - u) + abs(yNext - v) < res)
            res = abs(xNext - u) + abs(yNext - v);

        if (abs(curRow - u) + abs(curCol - v) < res)
            res = abs(curRow - u) + abs(curCol - v);

    }

    return res;
}

bool doubleCheckWayToHome(Position start, Position desStable) {

    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));
    memset(f, 0, sizeof(f));

    q.push(make_pair(start.x, start.y));
    visited[start.x][start.y] = true;
    f[start.x][start.y] = 0;

    visited[curRow][curCol] = true;

    int res = oo;

    int ok = false;

    if (start.x == desStable.x && start.y == desStable.y) {
        return true;
    }

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);

            if (visited[uNext][vNext]) continue;
            if (!isInsideBoard(uNext, vNext)) continue;
            if (matDis({uNext, vNext}, desStable) + f[u][v] + 2 >= distanceToAnotherBot(uNext, vNext)) continue;
//            if (uNext == 5 && vNext == 3) {
//                cout << "VT = " << distanceToStable(uNext, vNext) << endl;
//                cout << "f[u][v] = " << f[u][v] << endl;
//                cout << "VP = " << distanceToAnotherBot(uNext, vNext) << endl;
//                DEBUG("5, 3 safe");
//            }

            if (board[uNext][vNext] != myUnstableNumber) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
                f[uNext][vNext] = f[u][v] + 1;

                if (isStableCell(uNext, vNext) && stableScore[uNext][vNext] > f[uNext][vNext]) {
                    if (uNext == desStable.x && vNext == desStable.y) {
                        ok = true;
                        break;
                    }
                }
            }
        }
        if (ok) break;
    }
    return ok;
}


bool isSafe(int xNext, int yNext, int& disToHome, Position traceToHome[nRows + 5][nColumns +5], Position& desStable) {
    int wayToHome = emptyCellsDistance({xNext, yNext}, traceToHome, desStable);
    int wayToHell = distanceFromOtherBotToUnstable(xNext, yNext) - 1; // -1 for next move

    bool okWayToHome = doubleCheckWayToHome({xNext, yNext}, desStable);
    if (!okWayToHome) return false;

//    DEBUG(wayToHome);
//    DEBUG(wayToHell);

    disToHome = wayToHome;
    return wayToHome < wayToHell;
}

int calArea(Position pos, Position traceToHome[nRows + 5][nColumns +5], Position desStable) {
    int newBoard[nRows + 5][nColumns + 5];
    memset(newBoard, 0, sizeof(newBoard));
    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nColumns; j++)
            if (board[i][j] == myStableNumber || board[i][j] == myUnstableNumber)
                newBoard[i + 1][j + 1] = board[i][j];

    Position temp = desStable;
    while (temp.x != pos.x || temp.y != pos.y) {
        if (board[temp.x][temp.y] != myStableNumber)
            newBoard[temp.x + 1][temp.y + 1] = myUnstableNumber;
        // cout << temp.x << " " << temp.y << endl;
        temp = traceToHome[temp.x][temp.y];
    }

    if (newBoard[pos.x + 1][pos.y + 1] != myStableNumber)
        newBoard[pos.x + 1][pos.y + 1] = myUnstableNumber;

    // BFS
    q = queue< pair<int, int> >();

    q.push(make_pair(0, 0));

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            if ((uNext < 0 || uNext >= nRows + 2 || vNext < 0 || vNext >= nColumns + 2)) continue;
            if (newBoard[uNext][vNext] == 0) {
                q.push(make_pair(uNext, vNext));
                newBoard[uNext][vNext] = myStableNumber;
            }
        }
    }

    // for (int i = 0; i < nRows + 2; i++){
        // for (int j = 0; j < nColumns + 2; j++) {
            // cout << newBoard[i][j];
        // }
        // cout << endl;
    // }
    // cout << endl;
    int area = 0;
    for (int i = 0; i < nRows + 2; i++){
        for (int j = 0; j < nColumns + 2; j++)
            if (newBoard[i][j] == 0 || newBoard[i][j] == myUnstableNumber) area += 1;
    }


    return area;
}

void adjustMyStable(Position enemyStable, Position traceToEnemyHome[nRows + 5][nColumns + 5], int distance) {
    int newBoard[nRows + 5][nColumns + 5];
    memset(newBoard, 0, sizeof(newBoard));
    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nColumns; j++)
            if (board[i][j] != myStableNumber && board[i][j] != myUnstableNumber)
                newBoard[i + 1][j + 1] = board[i][j];

    Position temp = enemyStable;
    Position enemyBot = currentBotPosition[1];
    while (temp.x != enemyBot.x || temp.y != enemyBot.y) {
        newBoard[temp.x + 1][temp.y + 1] = 1;
        temp = traceToEnemyHome[temp.x][temp.y];
    }

    newBoard[enemyBot.x + 1][enemyBot.y + 1] = 1;
    // BFS
    q = queue< pair<int, int> >();

    q.push(make_pair(0, 0));

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            if ((uNext < 0 || uNext >= nRows + 2 || vNext < 0 || vNext >= nColumns + 2)) continue;
            if (newBoard[uNext][vNext] == 0) {
                q.push(make_pair(uNext, vNext));
                newBoard[uNext][vNext] = 1;
            }
        }
    }

    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nColumns; j++)
            if (board[i][j] == myStableNumber && newBoard[i + 1][j + 1] == 0 && stableScore[i][j] > distance) {
                  stableScore[i][j] = distance;
            }
}

void createStableScore() {
    q = queue< pair<int, int> >();
    bool visited[nRows + 5][nColumns + 5];
    Position traceToEnemyHome[nRows + 5][nColumns + 5];
    memset(visited, false, sizeof(visited));
    memset(f, 0, sizeof(f));

    Position enemy = currentBotPosition[1];

    q.push(make_pair(enemy.x, enemy.y));
    visited[enemy.x][enemy.y] = true;
    f[enemy.x][enemy.y] = 0;

    int res = oo;

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            if (visited[uNext][vNext]) continue;
            if (!isInsideBoard(uNext, vNext)) continue;
            if (board[uNext][vNext] == myUnstableNumber || board[uNext][vNext] % 2 == 1 || board[uNext][vNext] == 0) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
                f[uNext][vNext] = f[u][v] + 1;
                traceToEnemyHome[uNext][vNext] = {u, v};
                if (board[uNext][vNext] == myStableNumber) stableScore[uNext][vNext] = f[uNext][vNext];
            }
        }
    }

    for (int i = 0; i < nRows; i++)
        for (int j = 0; j < nColumns; j++)
            if (f[i][j] > 0 && (board[i][j] % 2 == 1 && board[i][j] != myStableNumber))
                adjustMyStable({i, j}, traceToEnemyHome, f[i][j]);

}

semanticMoves safeStrategyFromUnstable() {
    currentDestination = {-1, -1};
    exDestination.clear();

    int nextVal;
    int disToHome;
    int maxDisToHome = -1;
    int maxDisToStart = -1;
    semanticMoves maxMove;
    Position traceToHome[nRows + 5][nColumns + 5];
    Position desStable;
    int maxArea = -1;

    createStableScore();
    // for (int i = 0; i < nRows; i++){
        // for (int j = 0; j < nColumns; j++) {
            // cout << stableScore[i][j];
        // }
        // cout << endl;
    // }


    for (int i = 0; i < 4; i++) {
        int xNext = curRow + dX[i];
        int yNext = curCol + dY[i];
        semanticMoves move = static_cast<semanticMoves>(i);
        
//        DEBUG(xNext);
//        DEBUG(yNext);
        if (isThisMoveValid(move, nextVal)) {

            if (isSafe(xNext, yNext, disToHome, traceToHome, desStable)) {
//                DEBUG("have safe");
                int area = calArea({xNext, yNext}, traceToHome, desStable);
                if (area > maxArea) {
                    maxMove = move;
                    // maxDisToHome = disToHome;
                    maxArea = area;
//                    if (lastMaxToHome > maxDisToHome) {
//                        isDesToHome = true;
//                        break;
//                    }
//                    lastMaxToHome = maxDisToHome;
                }
            }

        }
    }

    if (maxArea == -1) {
        semanticMoves minMove;
        int minDisToHome = oo;

        for (int i = 0; i < 4; i++) {
            int xNext = curRow + dX[i];
            int yNext = curCol + dY[i];

            semanticMoves move = static_cast<semanticMoves>(i);

            if (isThisMoveValid(move, nextVal)) {
                int wayToHome = emptyCellsDistance({xNext, yNext}, traceToHome, desStable);
                // cout << xNext << " " << yNext << endl;
//                 DEBUG(wayToHome);
                // cout << endl;
                if (wayToHome < minDisToHome) {
                    minDisToHome = wayToHome;
                    minMove = move;

                }
            }
        }
       // DEBUG(minDisToHome);
//        cout << "min move" << endl;
       // DEBUG(minMove);
        if (minDisToHome != oo) {
            return minMove;
        } else {
            semanticMoves homeMove;
            int minHome = oo;
            for (int i = 0; i < 4; i++) {
                int xNext = curRow + dX[i];
                int yNext = curCol + dY[i];

                semanticMoves move = static_cast<semanticMoves>(i);
                if (isThisMoveValid(move, nextVal)) {
                    int wayToHome = distanceToStable(xNext, yNext);
                    if (wayToHome < minHome) {
                        minHome = wayToHome;
                        homeMove = move;
                    }
                }


            }
            return homeMove;

        }

    } else {
        // cout << "max move" << endl;
        cntMaxMove ++;
        return maxMove;
    }
}

bool checkCurrentDest() {
    q = queue< pair<int, int> >();
    memset(visited, false, sizeof(visited));

    q.push(make_pair(curRow, curCol));
    visited[curRow][curCol] = true;

    int nextVal;

    bool ok = false;

    while (!q.empty()) {
        int u = q.front().first;
        int v = q.front().second;
        q.pop();

        for (int i = 0; i <= 3; i++) {
            int uNext = u + dX[i];
            int vNext = v + dY[i];

            // NOTE
            semanticMoves move = static_cast<semanticMoves>(i);
            if (u == curRow && v == curCol) {
                if (!isThisMoveValid(move, nextVal)) continue;
            }

            if (!isInsideBoard(uNext, vNext)) continue;
            if (visited[uNext][vNext]) continue;
            if (numAdjStableCell(u, v) == 1) continue;
            if (isStableCell(uNext, vNext)) {
                visited[uNext][vNext] = true;
                q.push(make_pair(uNext, vNext));
            } else {
                if (uNext == currentDestination.x && vNext == currentDestination.y) {
                    ok = true;
                    break;
                }
            }
        }
        if (ok) break;
    }

    if (!ok) {
        exDestination.push_back(currentDestination);
        currentDestination = {-1, -1};
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
            if (board[nextRow][nextCol] % 2 == 0 && board[nextRow][nextCol] > 0 && board[nextCol][nextCol] != myUnstableNumber) {
                return move;
            }
        }
    }

    if (board[curRow][curCol] == myStableNumber) {
        if (currentDestination.x != -1)
            checkCurrentDest();

        if (currentDestination.x != -1) {
            return greedyMove();
        }

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
    lastMaxToHome = -1;
    isDesToHome = false;

    srand(time(NULL));
//    freopen("bug.txt", "r", stdin);
    int tempRow, tempCol;
    char temp;

    //    initBoard();
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
//            clock_t begin = clock();
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

//            lastMove = RIGHT;
//            currentDestination = {3, 3};
//            exDestination.push_back({5, 27});
//            exDestination.push_back({5, 28});
//            exDestination.push_back({5, 29});


            makeBestMove();
//            clock_t end = clock();
//            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//            DEBUG(elapsed_secs);
        }
    }
}
//
