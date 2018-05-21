#include <iostream>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <sstream>

using namespace std;

const int GRID_SIZE = 8;
const int CORNER_X[] = {0,0,GRID_SIZE-1,GRID_SIZE-1};
const int CORNER_Y[] = {0,GRID_SIZE-1,0,GRID_SIZE-1};
//const int AROUND_CORNER_X[] = {1, GRID_SIZE-2, 0, 1, GRID_SIZE-2, GRID_SIZE-1, 0, 1, GRID_SIZE-2, GRID_SIZE-1, 1, GRID_SIZE-2};
//const int AROUND_CORNER_Y[] = {0, 0, 1, 1, 1, 1, GRID_SIZE-2x, GRID_SIZE-2, GRID_SIZE-2, GRID_SIZE-2, GRID_SIZE-1, GRID_SIZE-1};
const int P1_KEY = 1;
const int P2_KEY = -P1_KEY;
const int AI_KEY = P2_KEY;
const int HEIGHT = 4; //keep this even, so that the leaf node is AI's turn

struct Grid
{
    int cell[GRID_SIZE][GRID_SIZE];
    int playValue;//1=player -1=player2
    int p1Amount;//no. cells captured by p1
    int p2Amount;
};

int bestIndex;
bool isEndgame = false;

void initialiseGrid(Grid& board);
int incX(int direction);
int incY(int direction);
bool checkDirections(int cellX, int cellY, int direction, Grid& board);
bool check(int cellX, int cellY, bool toUpdate[GRID_SIZE][GRID_SIZE], Grid& board);
void update(bool newUpdate[GRID_SIZE][GRID_SIZE], Grid& board);
vector<int> checkAll(Grid& board);
int convertOneDimension(int x, int y);
bool skipTurn(Grid& board);
void convertCell(int x, int y, Grid& board);
//int findChainLength(int cellX, int cellY, Grid& board);
//void determineDirection(int cellX, int cellY, bool direction[GRID_SIZE], Grid& board);
//void chooseLongestChain(int moves[], int size);
int getNumCells(Grid& board);
int getNumCorners(Grid& board);
int getNumAroundCorners(Grid& board);
float minimax(Grid &board, int depth, bool isMaximizer, float alpha, float beta);
void copyBoard(Grid& board1, Grid& board2);
int convertToCoordinateY(int coordinate);
int convertToCoordinateX(int coordinate);
void makeMove(int move, Grid& board);
bool gameIsOver(Grid& board);
float evaluate(Grid &board);
void makeAIMove(Grid &board);
float maxOf(float a, float b);
float minOf(float a, float b);


//temp draw func
void drawGrid(Grid& board, vector<int> moves);
void getMove(Grid& board);
void alternatePlayers(Grid& board);


/*
 ADD OMEGA SPECFIFC FUNCTIONS
 */

//OMEGA SHIT NEEDED
void initialiseGrid(Grid& board)
{
    //2 tiles each, for p1 and p2
    board.p1Amount = 2;
    board.p2Amount = 2;
    board.playValue = P1_KEY;
    
    //initalizing a board of blank tiles
    for(int y = 0; y < GRID_SIZE; y++)
    {
        for(int x = 0; x < GRID_SIZE; x++)
        {
            board.cell[x][y] = 0;
        }
    }
    
    //createBoard();
    
    //setting up board with appropriate player tiles arranged in centre
    board.cell[GRID_SIZE/2][GRID_SIZE/2] = P1_KEY;
    board.cell[GRID_SIZE/2 - 1][GRID_SIZE/2 - 1] = P1_KEY;
    board.cell[GRID_SIZE/2 - 1][GRID_SIZE/2] = P2_KEY;
    board.cell[GRID_SIZE/2][GRID_SIZE/2 - 1] = P2_KEY;
    
    //temp draw func
    //drawGrid(board, checkAll(board));
}

//temp draw func
void drawGrid(Grid& board, vector<int> moves)
{
    int moveIndex = 0;
    
    cout << "  ";
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        cout << (char)(i + 'A') << " ";
    }
    
    cout << endl;
    
    for(int y = 0; y < GRID_SIZE; y++)
    {
        cout << (y + 1) << " ";
        
        for(int x = 0; x < GRID_SIZE; x++)
        {
            if(board.cell[x][y] == P1_KEY)
                cout << "W ";
            else if(board.cell[x][y] == P2_KEY)
                cout << "B ";
            else if(moveIndex < moves.size() && moves.at(moveIndex) == convertOneDimension(x,y))
            {
                moveIndex++;
                cout << "+ ";
            }
            else
                cout << "- ";
        }
        cout << endl;
    }
    
    cout << endl << endl;
}

//temp draw func
void getMove(Grid& board)
{
    string name;
    int y;
    int x;
    
    if(board.playValue == P1_KEY)
    {
        
        cout << "Move: ";
        
        cin >> name;
        
        //stringstream strVal(name.substr(0,1));
        //strVal >> x;
        x = name[0] - 'a';
        
        //stringstream strVal2(name.substr(1,1));
        //strVal2 >> y;
        y = name[1] - '1';
        
        convertCell(x,y,board);
    }
}

void alternatePlayers(Grid& board)
{
    while(!gameIsOver(board)) //maybe make this a do while loop instead of a normal loop
    {
        drawGrid(board, checkAll(board));
        
        getMove(board);
        
        drawGrid(board, checkAll(board));
        
        cout << "computer is thinking..." << endl;
        
        makeAIMove(board);
    }
}

bool gameIsOver(Grid& board)
{
    if(board.p1Amount + board.p2Amount == GRID_SIZE*GRID_SIZE)
        return true;
    
    else if(skipTurn(board))
    {
        if(skipTurn(board))
            return true;
    }
    
    return false;
}

int incX(int direction)
{
    int incX[] = {0,1,1,1,0,-1,-1,-1};    //increment X by certain value
    
    //x values arranged like:
    
    //    -1    0    1
    //    -1    X    1
    //    -1    0    1
    
    //incX directional indices are:
    
    //    7    0    1
    //    6         2
    //    5    4    3
    
    //so going in the specified direction will change the X-Coordinate accordingly,
    //i.e. returns appropriate X increment of given direction
    
    return incX[direction];
}

int incY(int direction)
{
    int incY[] = {1,1,0,-1,-1,-1,0,1};    //increment Y by certain value
    
    //y values arranged like:
    
    //     1     1     1
    //     0     Y     0
    //    -1    -1    -1
    
    //incY directional indices are:
    
    //    7    0    1
    //    6         2
    //    5    4    3
    
    //so going in the specified direction will change the Y-Coordinate accordingly,
    //i.e. returns appropriate Y increment of given direction
    
    return incY[direction];
}

bool checkDirections(int cellX, int cellY, int direction, Grid& board)
{
    int cellsMoved = 0;
    
    //while((cellX >= 0) && (cellX <= 7) && (cellY >= 0) && (cellY <= 7))
    //cout << "original: " << cellX << ", " << cellY << endl;
    //cout << "direction: " << direction << endl;
    
    
    while(true)
    {
        cellX += incX(direction);
        cellY += incY(direction);
        
        //cout << cellX << ", "  << cellY << endl;
        
        if(cellX < 0 || cellX > 7 || cellY < 0 || cellY > 7)
        {
            return false;
        }
        
        
        cellsMoved++;
        
        if((board.cell[cellX][cellY] == board.playValue) && (cellsMoved > 1))
            return true;
        else if(board.cell[cellX][cellY] == 0)
            return false;
        else if((board.cell[cellX][cellY] == board.playValue) && (cellsMoved == 1))
            return false;
        
    }
    
    //cout << "\n\n\n";
}

bool check(int cellX, int cellY, bool toUpdate[GRID_SIZE][GRID_SIZE], Grid& board)
{
    int originalX = cellX;
    int originalY = cellY;
    bool worked = false;
    
    //worked = false;
    //bool toUpdate[GRID_SIZE][GRID_SIZE];
    
    for(int y = 0; y < GRID_SIZE; y++)
    {
        for(int x = 0; x < GRID_SIZE; x++)
        {
            toUpdate[x][y] = false;
        }
    }
    
    for(int i = 0; i < GRID_SIZE; i++)
    {
        cellX = originalX;
        cellY = originalY;
        
        if(checkDirections(originalX, originalY, i, board))
        {
            
            cellX += incX(i);
            cellY += incY(i);
            
            while(board.cell[cellX][cellY] == board.playValue * -1)
            {
                toUpdate[cellX][cellY] = true;
                
                cellX += incX(i);
                cellY += incY(i);
            }
            
            worked = true;
        }
    }
    
    //cout << "\n\n\n";
    
    return worked;
}
//updates board status
void update(bool newUpdate[GRID_SIZE][GRID_SIZE], Grid& board)
{
    int amountChanged = 0;
    
    for (int y = 0; y < GRID_SIZE; y++)
    {
        for (int x = 0; x < GRID_SIZE; x++)
        {
            if (newUpdate[x][y])
            {
                board.cell[x][y] = board.playValue;
                amountChanged++;
            }
        }
    }
    
    if (board.playValue == P1_KEY)
    {
        
        board.p1Amount = (board.p1Amount + amountChanged + 1);
        board.p2Amount -= amountChanged;
    }
    else
    {
        board.p2Amount = (board.p2Amount + amountChanged + 1);
        board.p1Amount -= amountChanged;
    }
}

int getNumCells(Grid& board)
{
    if(board.playValue == P1_KEY)
        return board.p1Amount;
    
    return board.p2Amount;
}

//OMEGA SHIT NEEDED
vector<int> checkAll(Grid& board)
{
    vector<int> validMove;
    bool checkValid[GRID_SIZE][GRID_SIZE];
    
    for(int y = 0; y < GRID_SIZE; y++)
    {
        for(int x = 0; x < GRID_SIZE; x++)
        {
            if(board.cell[x][y] == 0)
            {
                bool worked = check(x, y, checkValid, board);
                
                if(worked)
                {
                    validMove.push_back(convertOneDimension(x, y));
                }
            }
        }
    }
    
    return validMove;
}

int convertOneDimension(int x, int y)
{
    //return GRID_SIZE * y + x + 1;
    return GRID_SIZE * y + x;
}

//OMEGA SHIT NEEDED
bool skipTurn(Grid& board)
{
    if(checkAll(board).size() == 0)
    {
        cout << "No moves available. Skip turn." << endl;
        
        board.playValue *= -1;
        
        //temp draw func
        //drawGrid(board, moves);
        
        return true;
    }
    
    return false;
}



//OMEGA SHIT NEEDED
//implements change
void convertCell(int x, int y, Grid& board)
{
    bool newUpdate[GRID_SIZE][GRID_SIZE];
    bool worked = check(x, y, newUpdate, board);
    
    if(worked)
    {
        board.cell[x][y] = board.playValue;
        update(newUpdate, board);
        
        board.playValue = -board.playValue;
        
        skipTurn(board);
        
        //temp draw func
        //drawGrid(board, checkAll(board));
    }
}

void makeMove(int move, Grid& board)
{
    convertCell(convertToCoordinateX(move), convertToCoordinateY(move), board);
}

int convertToCoordinateX(int coordinate)
{
//    if(coordinate % GRID_SIZE == 0)
//    {
//        return 7;
//    }
//
//    return coordinate % GRID_SIZE - 1;
    return coordinate % GRID_SIZE;
    
}

int convertToCoordinateY(int coordinate)
{
//    if(coordinate % GRID_SIZE == 0)
//    {
//        return coordinate / 8 -1;
//    }

//    return coordinate / GRID_SIZE;
    
    return coordinate/GRID_SIZE;
}

//AI future checker
void copyBoard(Grid& board1, Grid& board2)
{
    board2.playValue = board1.playValue;
    board2.p1Amount = board1.p1Amount;
    board2.p2Amount = board2.p2Amount;
    
    for(int y = 0; y < GRID_SIZE; y++)
    {
        for(int x = 0; x < GRID_SIZE; x++)
        {
            board2.cell[x][y] = board1.cell[x][y];
        }
    }
}


//COMPUTER PLAYER METHODS
/*
 int findChainLength(int cellX, int cellY, Grid& board)
 {
 int cellsMoved = 0;
 bool direction[GRID_SIZE];
 //    int initialX = cellX;
 //    int initialY = cellY;
 
 determineDirection(cellX, cellY, direction, board);
 
 for(int i = 0; i < GRID_SIZE; i++)
 {
 if(direction[i])
 {
 while ((cellX >= 0) && (cellX <= GRID_SIZE-1) && (cellY >= 0) && (cellY <= GRID_SIZE - 1) &&
 (board.cell[cellX][cellY] != board.playValue))
 {
 cellX += incX(i);
 cellY += incY(i);
 
 cellsMoved++;
 }
 }
 }
 
 return cellsMoved;
 }
 
 void determineDirection(int cellX, int cellY, bool direction[GRID_SIZE], Grid &board)
 {
 for(int i = 0; i < GRID_SIZE; i++)
 {
 direction[i] = false;
 if(checkDirections(cellX, cellY, i, board))
 {
 direction[i] = true;
 }
 }
 }
 */

int getNumCorners(Grid& board)
{
    int numCorners = 0;
    
    for(int i = 0; i < 4; i++)
    {
        if(board.cell[CORNER_X[i]][CORNER_Y[i]] == board.playValue)
            numCorners++;
    }
    
    return numCorners;
}

//int getNumAroundCorners(Grid& board)
//{
//    int numAroundCorners = 0;
//
//    for(int i = 0; i < 12; i++)
//    {
//        if(board.cell[AROUND_CORNER_X[i]][AROUND_CORNER_Y[i]] == board.playValue)
//            numAroundCorners++;
//    }
//
//    return numAroundCorners;
//}

int main()
{
    Grid board;
    
    initialiseGrid(board);
    
    alternatePlayers(board);
    
    return 0;
}

float minimax(Grid &board, int depth, bool isMaximizer, float alpha, float beta){
    
    vector <int> legalMoves = checkAll(board);
    int numChildren = (int)legalMoves.size();
    
    if (depth == 0 || numChildren == 0){
        return evaluate(board);
    }
    
    if(isMaximizer){
        for (int i = 0; i < numChildren; i++){
            
            
            //creating a newBoard with the same board state as board
            //doing this instead of passing by value to minimize
            //load on the stack
            
            Grid newBoard;
            copyBoard(board, newBoard);
            makeMove(legalMoves[i], newBoard);
            
            float temp = minimax(newBoard, depth - 1, false, alpha, beta);
            
            if (depth == HEIGHT){ //first level of tree
                if (temp > alpha){
                    bestIndex = i;
                }
            }
            
            alpha = maxOf(alpha, temp);
            
            if (beta <= alpha){
                return beta;
            }
        }
        return alpha;
    }
    else{
        for (int i = 0; i < numChildren; i++){
            
            //creating a newBoard with the same board state as board
            //doing this instead of passing by value to minimize
            //load on the stack
            
            Grid newBoard;
            copyBoard(board, newBoard);
            makeMove(legalMoves[i], newBoard);
            
            float temp = minimax(newBoard, depth - 1, true, alpha, beta);
            
            beta = minOf(beta, temp);
            
            if (beta <= alpha){
                return alpha;
            }
        }
        return beta;
    }
    
}

//evaluates the current board based on 3 factors:

//    1. Corners Occupied: (100)
//         -Each corner has a weight of 100. Corners are very valuable
//         in Othello because they cannot be reversed.
//
//    2. Mobility: (1)
//        -The number of legal moves the player can make is the main
//         evaluation method. Each legal move the playter can make
//         has a weight of 11.
//
//    3. Disc Count: (1/100)
//        -Discs aren't a very useful factor to determine the strength
//         of a player's position, as discs can easily be flipped later
//         in the game. Each disc has a weight of 1/100.

float evaluate(Grid &board)
{
    const float CORNER_WEIGHT = 100;
    //    const float AROUND_CORNER_WEIGHT = 2;
    const float MOBILITY_WEIGHT = 1;
    const float DISC_COUNT_WEIGHT = 1/100;
    
    float score = 0;
    float oppScore = 0;
    
    
    int playerKey = board.playValue;
    
    //we only evaluate the score of the board from the AI's perspective
    board.playValue = AI_KEY;
    
    score += CORNER_WEIGHT * getNumCorners(board);
    score += DISC_COUNT_WEIGHT * getNumCells(board);
    
    //    score -= AROUND_CORNER_WEIGHT * getNumAroundCorners(board);
    
    board.playValue = P1_KEY;
    
    oppScore += CORNER_WEIGHT * getNumCorners(board);
    oppScore += DISC_COUNT_WEIGHT * getNumCells(board);
    
    //    oppScore -= AROUND_CORNER_WEIGHT * getNumAroundCorners(board);
    
    if (!isEndgame){
        if(playerKey == AI_KEY){ //if it's ai's turn to play, add its mobility to AI board score
            board.playValue = AI_KEY;
            int numLegalMoves = (int)checkAll(board).size();
            score += MOBILITY_WEIGHT * numLegalMoves;
            
        }
        else{ //if it's opponent's turn to play, subtract opponent's mobility from AI board score
            board.playValue = P1_KEY;
            int numLegalMoves = (int)checkAll(board).size();
            oppScore += MOBILITY_WEIGHT * numLegalMoves;
        }
    }
    
    score = score - oppScore;
    
    board.playValue = playerKey;
    
    return score;
}

void makeAIMove(Grid &board)
{
    if (board.playValue == AI_KEY){
        
        if(GRID_SIZE*GRID_SIZE - board.p1Amount - board.p2Amount <= 14)
            isEndgame = true;
        
        minimax(board, HEIGHT, true, -1000, 1000);
        
        
        makeMove(checkAll(board).at(bestIndex), board);
    }
}

float maxOf(float a, float b)
{
    if (a > b)
        return a;
    
    return b;
}

float minOf(float a, float b)
{
    if (a > b)
        return b;
    
    return a;
}

//void outPutScore()
//{
//}

//void outputToLCD()
//{
//    outPutScore();
//}

