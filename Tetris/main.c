#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include <string.h>

////without buffer: 1250/1327 tiles per second

CONSOLE_FONT_INFOEX cfi;
COORD cursor = {0, 0};

struct SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
};

struct printableVar{
    int current;
    int previous;
    COORD position;
};



struct regularPiece{
    int pieceID;
    COORD previousPosition;
    COORD position;
    int color;
    int merge;
    int parts[4][2];
};

int nextPiece;

struct timeb lastUpdate;

struct printableVar score;

int xoffset = 20; //30
int yoffset = 10; //10

int boardX = 10;
int boardY = 20;
int board[10][20][2];
COORD boardPosition = {1,1};

int Tilesize = 4; //4
int fontsize = 5; //5

char tiles[10][4][4]= {

    {{255,255,255,255},
     {255,255,255,255}, //eraser
     {255,255,255,255},
     {255,255,255,255}},

    {{219,219,219,219},
     {219,177,177,219}, //178
     {219,177,177,219},
     {219,219,219,219}},

     //board pieces

    {{186,176,221,219}, //right board
     {186,176,221,219},
     {186,176,221,219},
     {186,176,221,219}},

    {{219,222,176,186}, //left board
     {219,222,176,186},
     {219,222,176,186},
     {219,222,176,186}},

    {{205,205,205,205}, //bottom
     {176,176,176,176},
     {223,223,223,223},
     {219,219,219,219}},

    {{219,219,219,219}, //top
     {220,220,220,220},
     {176,176,176,176},
     {205,205,205,205}},

    {{219,219,219,219}, //top left
     {219,219,220,220},
     {219,222,176,176},
     {219,222,176,201}},

    {{219,219,219,219}, //top right
     {220,220,219,219},
     {176,176,221,219},
     {187,176,221,219}},

    {{219,222,176,200}, //bottonm left
     {219,222,176,176},
     {219,219,223,223},
     {219,219,219,219}},

    {{188,176,221,219}, //bottom right
     {176,176,221,219},
     {223,223,219,219},
     {219,219,219,219}},


};

int pieces[7][4][2]={
    {{0,0},
     {1,0}, //square
     {0,-1},
     {1,-1}},

    {{0,1},
     {0,0},
     {0,-1}, //bar
     {0,-2}},

    {{0,1},
     {0,0}, //L
     {0,-1},
     {1,-1}},

    {{0,1},
     {0,0}, // L e
     {0,-1},
     {-1,-1}},

    {{-1,-1},
     {0,-1},
     {0,0}, //Z
     {1,0}},

    {{1,-1},
     {0,-1},
     {0,0}, // S
     {-1,0}},

    {{-1,0},
     {0,0}, //t
     {1,0},
     {0,-1}},
};

char numbers[10][8][4] = {
    {{255,220,220,255},
     {222,219,219,221},
     {222,221,222,221},
     {222,221,222,221},
     {222,221,222,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,223,223,255}},

    {{255,255,220,255},
     {255,222,219,255},
     {255,219,219,255},
     {255,223,219,255},
     {255,255,219,255},
     {255,255,219,255},
     {255,219,219,221},
     {255,223,223,188}},

    {{255,220,220,255},
     {222,219,219,221},
     {222,221,222,221},
     {255,255,219,221},
     {255,222,219,255},
     {255,219,255,255},
     {222,219,220,221},
     {200,223,223,188}},

    {{255,220,220,255},
     {222,219,219,221},
     {222,221,222,221},
     {255,220,188,221},
     {255,223,187,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,223,223,255}},

    {{201,187,201,187},
     {222,221,222,221},
     {222,221,222,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,255,222,221},
     {255,255,222,221},
     {255,255,200,188}},

    {{201,220,220,187},
     {222,219,219,221},
     {222,221,255,255},
     {222,219,219,187},
     {255,255,222,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,223,223,255}},

    {{255,220,220,255},
     {222,219,219,221},
     {222,221,255,255},
     {222,219,219,255},
     {222,221,222,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,223,223,255}},

    {{201,220,220,187},
     {222,219,219,221},
     {255,255,222,221},
     {255,255,219,255},
     {255,222,221,255},
     {255,219,255,255},
     {222,221,255,255},
     {200,188,255,255}},

    {{255,220,220,255},
     {222,219,219,221},
     {222,221,222,221},
     {222,219,219,221},
     {222,221,222,221},
     {222,221,222,221},
     {222,219,219,221},
     {255,223,223,255}},

    {{255,220,220,255},
     {222,219,219,221},
     {222,221,222,221},
     {222,221,222,221},
     {255,219,219,221},
     {255,255,222,221},
     {222,219,219,221},
     {255,223,223,255}},
};

void adjustWindowSize()
{
    struct SMALL_RECT test;

    HANDLE hStdout;
    COORD coord;
    BOOL ok;

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    coord.X = 1;
    coord.Y = 1;
    ok = SetConsoleScreenBufferSize(hStdout, coord);

    test.Left = 0;
    test.Top = 0;
    test.Right = coord.X-1;
    test.Bottom = coord.Y-1;

    SetConsoleWindowInfo(hStdout, ok, &test);

}

void adjustFont(){
    cfi.cbSize = sizeof cfi;
    cfi.nFont = 0;
    cfi.dwFontSize.X = fontsize;
    cfi.dwFontSize.Y = cfi.dwFontSize.X*1.04;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;

    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);
}

int random(int min, int max){               ///-|
    int num;                                ///-|
    num = (rand() % (max - min + 1)) + min; ///-1-
    return(num);                            ///-|
}

void textcolor (int color)                                      /// -|
{                                                               /// -|
    static int __BACKGROUND;                                    /// -|
                                                                /// -|
    HANDLE h = GetStdHandle ( STD_OUTPUT_HANDLE );              /// -|
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;                        /// -|
                                                                /// -2-
                                                                /// -|
    GetConsoleScreenBufferInfo(h, &csbiInfo);                   /// -|
                                                                /// -|
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE),  /// -|
                             color + (__BACKGROUND << 4));      /// -|
}

void ShowConsoleCursor(int showFlag)
{
    HANDLE hStdOut;
    CONSOLE_CURSOR_INFO curInfo;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleCursorInfo(hStdOut, &curInfo);
    curInfo.bVisible = 0;
    SetConsoleCursorInfo(hStdOut, &curInfo);
}

void gotoxy (COORD position)                                               /// -|
{
    position.X += xoffset * Tilesize; position.Y += yoffset * Tilesize; /// X and Y coordinates      /// -3-
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);   /// -|
}



void printTile(int cell[2]){
    textcolor(cell[1]);
    cursor.X = cursor.X*Tilesize;
    cursor.Y = cursor.Y*Tilesize;
    for(int x=0; x<Tilesize; x++){
        gotoxy(cursor);
        printf("%c%c%c%c", tiles[cell[0]][x][0],tiles[cell[0]][x][1], tiles[cell[0]][x][2], tiles[cell[0]][x][3]);
        cursor.Y = cursor.Y+1;
    }
}

void printDigit(int cell[2]){
    textcolor(cell[1]);
    cursor.X = cursor.X*Tilesize;
    cursor.Y = cursor.Y*Tilesize;
    for(int x=0; x<Tilesize*2; x++){
        gotoxy(cursor);
        printf("%c%c%c%c", numbers[cell[0]][x][0],numbers[cell[0]][x][1], numbers[cell[0]][x][2], numbers[cell[0]][x][3]);
        /*
        textcolor(cell[1]);
        printf("%c", numbers[cell[0]][x][0]);
        textcolor(cell[1]);
        printf("%c", numbers[cell[0]][x][1]);
        textcolor(cell[1]);
        printf("%c", numbers[cell[0]][x][2]);
        textcolor(cell[1]);
        printf("%c", numbers[cell[0]][x][3]);
        textcolor(cell[1]);
        */
        cursor.Y++;
    }
}

void printNumber(struct printableVar input){

    if(input.current != input.previous){
        int current = input.current;
        int previous = input.previous;
        int currentDigit = 0;
        int previousDigit = 1;
        int step=0;

        do{
            currentDigit = current % 10;
            previousDigit = previous % 10;

            if(currentDigit != previousDigit || previous == 0){
                cursor.X = input.position.X-step, cursor.Y = input.position.Y;
                //score.previous = score.current;
                printDigit((int[2]){currentDigit,1});

                //score.current+=2;
            }

            current = current/10;
            previous = previous/10;
            step++;
        }while(current>0);
        score.previous = score.current;
    }
}

void printBoard(){
    for(int x=0; x<boardX; x++){
        for(int y=0; y<boardY; y++){
            if (1==1){
                cursor.X = x + boardPosition.X, cursor.Y = y + boardPosition.Y;
                printTile(board[x][y]);

                //score.current++;
            }
        }
    }
}

void drawFrame(int position[2],int  size[2],int color){

    int tile[2];
    tile[1] = color;
    int tilebase = 2;

    cursor.X = position[0];
    cursor.Y = position[1];
    tile[0] = tilebase+4;
    printTile(tile);

    cursor.X = position[0];
    cursor.Y = position[1]+size[1]-1;
    tile[0] = tilebase+6;
    printTile(tile);


    for(int x=1; x<size[0]-1; x++){
        cursor.X = x + position[0];
        cursor.Y = position[1];
        tile[0] = tilebase+3;
        printTile(tile);

        cursor.X = x + position[0];
        cursor.Y = position[1] + size[1] -1;
        tile[0] = tilebase+2;
        printTile(tile);
    }
    cursor.X = position[0]+size[0]-1;
    cursor.Y = position[1];
    tile[0] = tilebase+5;
    printTile(tile);

    cursor.X = position[0] + size[0]-1;
    cursor.Y = position[1] + size[1]-1;
    tile[0] = tilebase+7;
    printTile(tile);

    for(int y=1; y<size[1]-1; y++){
        cursor.X = position[0];
        cursor.Y = position[1]+y;
        tile[0] = tilebase+1;
        printTile(tile);

        cursor.X = position[0]+size[0]-1;
        cursor.Y = position[1]+y;
        tile[0] = tilebase;
        printTile(tile);
    }
}

void printPiece(struct regularPiece piece){
    //int tile;

    for(int x = 0; x<4; x++){
        cursor.X = piece.position.X + piece.parts[x][0]+ boardPosition.X;
        cursor.Y = piece.position.Y + piece.parts[x][1]+ boardPosition.Y;

        if(cursor.Y >0){
            int tile[] = {1, piece.color};
            printTile(tile);
        }
    }
}

void erasePiece(struct regularPiece piece){
    //int tile;

    for(int x = 0; x<4; x++){
        cursor.X = piece.previousPosition.X + piece.parts[x][0]+ boardPosition.X;
        cursor.Y = piece.previousPosition.Y + piece.parts[x][1]+ boardPosition.Y;

        if(cursor.Y >0){
            int tile[] = {0, 0};
            printTile(tile);
        }
    }
}

struct regularPiece checkCollision(struct regularPiece piece){
    for(int x =0; x< 4; x++){


        if(piece.position.X + piece.parts[x][0] > boardX -1) piece.position.X = piece.previousPosition.X;
        if(piece.position.X + piece.parts[x][0] < 0) piece.position.X = piece.previousPosition.X;

        if(piece.position.Y + piece.parts[x][1] > boardY -1){
            if(piece.position.Y > piece.previousPosition.Y)piece.merge=1;
            piece.position.Y = piece.previousPosition.Y;
        }

        if(piece.position.Y + piece.parts[x][1] >=0){
            if(board[piece.position.X + piece.parts[x][0]][piece.position.Y + piece.parts[x][1]][0] > 0){
                if(piece.position.Y > piece.previousPosition.Y)piece.merge=1;
                piece.position.Y = piece.previousPosition.Y;
                piece.position.X = piece.previousPosition.X;
            }
        }
    }
    return piece;
}

struct regularPiece turnPiece(struct regularPiece piece){
    int position[4][2];
    memcpy(position, piece.parts, sizeof position);
    for(int x =0; x<4; x++){
        piece.parts[x][0] = piece.parts[x][1];
        piece.parts[x][1] = position[x][0] * -1;
    }
    return piece;
}

int checkRow(){
    int lineFull=0;
    int numberOfRows = 0;

    for(int y=boardY-1; y>= 0; y--){
        lineFull=1;
        for(int x =0; x<boardX; x++){
            if(board[x][y][0] == 0){
                lineFull=0;
                break;
            }
        }
        if(lineFull){
            for(int j = y; j>=0; j--){
                for(int i=0; i< boardX; i++){
                    if(j > 0){
                        board[i][j][0] = board[i][j-1][0];
                        board[i][j][1] = board[i][j-1][1];
                    }
                    else{
                        board[i][j][0] = 0;
                        board[i][j][1] = 0;
                    }
                }
            }
            y++;
            numberOfRows ++;
        }

    }
    return numberOfRows;
}

struct regularPiece mergePiece(struct regularPiece piece){
    int a[2],b[2];
    for(int j=0; j<4; j++){
        board[piece.position.X+piece.parts[j][0]][piece.position.Y+piece.parts[j][1]][0]=1;
        board[piece.position.X+piece.parts[j][0]][piece.position.Y+piece.parts[j][1]][1]=piece.color;
    }

    piece.merge=0;

    piece.pieceID = nextPiece;
    piece.color = piece.pieceID+1;
    piece.position.X = boardX+5;
    piece.position.Y = 7;
    piece.previousPosition.X = boardX+5;
    piece.previousPosition.Y=7;
    memcpy(piece.parts, pieces[piece.pieceID], sizeof piece.parts);
    erasePiece(piece);

    piece.pieceID= random(0,6);
    piece.color = piece.pieceID+1;
    a[0] = boardX+2, a[1] =4;
    b[0] = 10, b[1] =8;
    drawFrame(a, b, piece.color);
    memcpy(piece.parts, pieces[piece.pieceID], sizeof piece.parts);
    printPiece(piece);

    //nextPiece = piece.pieceID;

    piece.pieceID += nextPiece;
    nextPiece = piece.pieceID - nextPiece;
    piece.pieceID = piece.pieceID - nextPiece;


    piece.color = piece.pieceID+1;
    piece.position.X = 4;
    piece.position.Y = 1;
    piece.previousPosition.X = 4;
    piece.previousPosition.Y = 1;
    memcpy(piece.parts, pieces[piece.pieceID], sizeof piece.parts);
    printPiece(piece);

    return piece;
}

void maximizeWindow(){
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_SHOWMAXIMIZED);
}

int main()
{
    //adjustWindowSize();
    //system("MODE CON COLS=500 LINES=2000");

    srand(time(NULL));
    adjustFont();
    ShowConsoleCursor(0);

    maximizeWindow();

    char input;

    int turn = 0;

    int clearRows =0;

    int a[] ={0,0}, b[]={boardX+2,boardY+2};
    drawFrame(a, b, 15);

    a[0] = boardX+2, a[1] = 0;
    b[0] = 10, b[1] = 4;
    drawFrame(a, b, 14);




    score.current=0;
    score.previous=1;
    score.position.X = boardX + 10, score.position.Y = 1;

    printBoard();


    struct regularPiece piece;

    piece.merge=0;

    piece.pieceID = random(0,6);
    piece.color = piece.pieceID+1;
    piece.position.X = boardX + 5;
    piece.position.Y = 7;
    memcpy(piece.parts, pieces[piece.pieceID], sizeof piece.parts);


    a[0] = boardX+2, a[1] =4;
    b[0] = 10, b[1] =8;
    drawFrame(a, b, piece.color);
    printPiece(piece);

    nextPiece = piece.pieceID;

    piece.pieceID = random(0,6);
    piece.color = piece.pieceID+1;
    piece.position.X = 4;
    piece.position.Y = 1;
    piece.previousPosition.X = 4;
    piece.previousPosition.Y = 1;
    memcpy(piece.parts, pieces[piece.pieceID], sizeof piece.parts);


    printNumber(score);
    printPiece(piece);

    getch();

    time_t start_t, end_t;

    time(&start_t);

    //struct timeb currentTtime;


    for(;;){

        time(&end_t);

        if(difftime(end_t, start_t) > 0.3){
            time(&start_t);
            piece.position.Y+=1;
        }


        if(kbhit()){
            input = getch();
            //printPiece(piece);
            switch(input){
                case 't':
                    score.current+=1001;
                    break;

                case 'q':
                    piece.pieceID++;
                    piece.color = piece.pieceID +1;
                    break;

                case 'w':
                    turn = 1;
                    break;

                case 's':
                    piece.position.Y++;
                    break;

                case 'a':
                    piece.position.X--;
                    break;

                case 'd':
                    piece.position.X++;
                    break;

                case 32:
                    piece.merge = 1;
                    break;
                default:
                    break;

            }
        }

        piece= checkCollision(piece);

        if(piece.merge){

            piece = mergePiece(piece);
        }


        clearRows = checkRow();
        if(clearRows){
            printBoard();
            printPiece(piece);
            score.current += clearRows * 50;
        }

        if(piece.position.X != piece.previousPosition.X || piece.position.Y != piece.previousPosition.Y || turn){
            erasePiece(piece);

            if(turn){
                piece = turnPiece(piece);
                turn = 0;
            }
            printPiece(piece);
            piece.previousPosition.X = piece.position.X;
            piece.previousPosition.Y = piece.position.Y;
        }


        if(score.previous != score.current){
            printNumber(score);
        }
        //input = getch();

        //printPiece(piece);

        //printBoard();
    }

    //getchar();

    return 0;
}

//struct timeb start, end;
//int step = 200;
//int t = 0;

//ftime(&start);
//t=start.millitm;

//ftime(&end);
//if(end.millitm > t+step)
