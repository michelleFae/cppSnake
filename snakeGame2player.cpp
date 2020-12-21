
#include <ncurses.h>
#include <cstdlib>
#include <tuple>

bool gameOver;
const int width = 50, height = 50;

int x, y, fruitX, fruitY, score;

int xP1, yP1, scoreP1;

#define SNAKE_GREEN 1 
#define RED_FOOD 2
#define BLUE_BORDER 3
#define SNAKE_PINK 4

enum eDirection {STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirection dir;
eDirection dir1;

const int maxSnakeLen = 100;
// think of each X & Y elem as linked to he corresponding element in Y/X
int tailX[maxSnakeLen], tailY[maxSnakeLen]; // this has to be an array and not a len - because he snake can curve
// to do: try using a queue? // no - becuase need to check for collisions with head;
// todo: dynamic array in C++? // no becuase dont want snake to not fit on board
int tailX1[maxSnakeLen], tailY1[maxSnakeLen]; // this has to be an array and not a len - because he snake can curve
int nTail = 0;
int nTail1 = 0;

struct retVals {        
    int x, y, nTail;
};

void Setup() 
{

    // init screen
    initscr();
    clear();
    noecho(); // The echo and noecho routines control whether characters typed by the user are echoed by getch as they are typed
    cbreak();
    curs_set(0); // hide cursor

    if (has_colors() == true) {
        start_color();
        init_pair(SNAKE_GREEN, COLOR_GREEN, COLOR_WHITE);
        init_pair(RED_FOOD, COLOR_RED, COLOR_WHITE);
        init_pair(BLUE_BORDER, COLOR_BLUE, COLOR_WHITE);
        init_pair(SNAKE_PINK, COLOR_MAGENTA, COLOR_WHITE);
    }


    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    xP1 = width / 2;
    yP1 = height / 4;

    // fruit must be located randomly on map
    fruitX = rand() % (width) + 1;
    fruitY = rand() % (height) + 1;

    //init score
    score = 0;
    //init score for player 1
    scoreP1 = 0;


     
}

void Draw()
{
    clear();

    // board
    for (int i = 0; i < width+2; i++) {
        // 0th a nd 1st arg are cooordinates
        attron(COLOR_PAIR(BLUE_BORDER));
        mvprintw(0, i, "+");
    }
    for (int i = 0; i < height+2; i++) {
        for (int j = 0; j < width+2; j++) {
            // creates boundary
            if (i == 0 || i == width + 1) {
                attron(COLOR_PAIR(BLUE_BORDER));
                mvprintw(i, j, "+");
            } else if (j == 0 || j == height + 1) { 
                attron(COLOR_PAIR(BLUE_BORDER));
                mvprintw(i, j, "+");
            }

             // Snake head
             else if (i == y && j == x) {
                attron(COLOR_PAIR(SNAKE_GREEN));
                mvprintw(i, j, "O");
             }
             // Snake head 1
             else if (i == yP1 && j == xP1) {
                attron(COLOR_PAIR(SNAKE_PINK));
                mvprintw(i, j, "X");
             }
             // fruit
             else if (i == fruitY && j == fruitX) {
                attron(COLOR_PAIR(RED_FOOD));
                mvprintw(i, j, "@");
             }
             // NOT HEAD - but still part of snake
             else {
                 for (int k = 0; k < nTail; k++) 
                 {
                    if (tailX[k] == j && tailY[k] == i)
                    {
                        attron(COLOR_PAIR(SNAKE_GREEN));
                        mvprintw(i, j, "o");
                    }
                 }

                 for (int k = 0; k < nTail1; k++) 
                 {
                    if (tailX1[k] == j && tailY1[k] == i)
                    {
                        attron(COLOR_PAIR(SNAKE_PINK));
                        mvprintw(i, j, "x");
                    }
                 }
            }

        }

    }
    // display score on bottom left
    attron(COLOR_PAIR(SNAKE_GREEN));
    mvprintw(width + 3, 0, "Green Snake Score %d", score);

    attron(COLOR_PAIR(SNAKE_PINK));
    mvprintw(width + 6, 0, "Pink Snake Score %d", scoreP1);



    // refresh will display all of this
    refresh();

   

}

void Input()
{
    // how what you do affects things

    // NOTIFIES HAT IS IT KEYPAD INPUT
    keypad(stdscr, TRUE);

    // SNAKE KEEPS MOVING INTHA DIRECTION ONCE THE ARROW IS PRESSED (you dont have to keep pressing it)
    halfdelay(1);

    int c = getch(); // get char

    switch (c)
    {
    case KEY_LEFT:
        dir = LEFT;
        break;
    case KEY_RIGHT:
        dir = RIGHT;
        break;
    case KEY_UP:
        dir = UP;
        break;
    case KEY_DOWN:
        dir = DOWN;
        break;
    case 97:
        dir1 = LEFT;
        break;
    case 100:
        dir1 = RIGHT;
        break;
    case 119:
        dir1 = UP;
        break;
    case 115:
        dir1 = DOWN;
        break;
    case 113: // user presses q to quit
        gameOver = true;
        break;
    }


}

retVals * SnakeLogic(int x, int y, int tailX[], int tailY[], int nTail, eDirection dir)
{
    // we are manually moving each segment of the snake to the next positon
    // potenital optimization - just change the last piece

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i  < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    // for changing direction
    switch(dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            // user does nothing
            break;  
    }

    if (x > width || x < 1 || y > height || y < 1 ) {
        gameOver = true;
    }

    // if you get a fruit
    if (x == fruitX  && y == fruitY) {
        score++;
        // fruit position must change
        fruitX = rand() % (width) + 1;
        fruitY = rand() % (height) + 1;

        // increment tail
        nTail++;

    }

    // go through each tail piece and see if it is the same as head - collisiion check
    for (int i = 0; i < nTail; i++) 
    {
        if (tailX[i] == x && tailY[i] == y) {
            gameOver = true;
        }

    }




    struct retVals *RetVals = (retVals *) malloc(sizeof (struct retVals));
    RetVals->x = x;
    RetVals->y = y;
    RetVals->nTail = nTail;

    return RetVals;
}


void Logic()
{
    // we are manually moving each segment of the snake to the next positon
    // potenital optimization - just change the last piece

    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;
    for (int i = 1; i  < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    // for changing direction
    switch(dir)
    {
        case LEFT:
            x--;
            break;
        case RIGHT:
            x++;
            break;
        case UP:
            y--;
            break;
        case DOWN:
            y++;
            break;
        default:
            // user does nothing
            break;  
    }

    if (x > width || x < 1 || y > height || y < 1 ) {
        gameOver = true;
    }

    // if you get a fruit
    if (x == fruitX  && y == fruitY) {
        score++;
        // fruit position must change
        fruitX = rand() % (width) + 1;
        fruitY = rand() % (height) + 1;

        // increment tail
        nTail++;

    }

    // go through each tail piece and see if it is the same as head - collisiion check
    for (int i = 0; i < nTail; i++) 
    {
        if (tailX[i] == x && tailY[i] == y) {
            gameOver = true;
        }

    }


    // snake 2

    prevX = tailX1[0];
    prevY = tailY1[0];

    tailX1[0] = xP1;
    tailY1[0] = yP1;
    for (int i = 1; i  < nTail1; i++)
    {
        prev2X = tailX1[i];
        prev2Y = tailY1[i];
        tailX1[i] = prevX;
        tailY1[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    // for changing direction
    switch(dir1)
    {
        case LEFT:
            xP1--;
            break;
        case RIGHT:
            xP1++;
            break;
        case UP:
            yP1--;
            break;
        case DOWN:
            yP1++;
            break;
        default:
            // user does nothing
            break;  
    }

    if (xP1 > width || xP1 < 1 || yP1 > height || yP1 < 1 ) {
        gameOver = true;
    }

    // if you get a fruit
    if (xP1 == fruitX  && yP1 == fruitY) {
        scoreP1++;
        // fruit position must change
        fruitX = rand() % (width) + 1;
        fruitY = rand() % (height) + 1;

        // increment tail
        nTail1++;

    }

    // go through each tail piece and see if it is the same as head - collisiion check
    for (int i = 0; i < nTail1; i++) 
    {
        if (tailX1[i] == xP1 && tailY1[i] == yP1) {
            gameOver = true;
        }

    }


}



int main()
{
    Setup();

    while (!gameOver) {
        Draw();
        Input();
        Logic();
    }

    // clear();
    getch();
    if (scoreP1 == score) 
    {
        attron(COLOR_PAIR(BLUE_BORDER));
        mvprintw(width/2, height/2, "It's a tie!");
    }
    else if (scoreP1 > score)
    {
        attron(COLOR_PAIR(SNAKE_PINK));
        mvprintw(width/2, height/2, "PINK SNAKE WINS!");
    }
    else 
    {
        attron(COLOR_PAIR(SNAKE_GREEN));
        mvprintw(width/2, height/2, "GREEN SNAKE WINS!");
    }
    mvprintw(width/2, 3*height/4, "PRESS q TO Quit and n TO PLAY AGAIN!");
    
    
    bool isEnd = true;
    while (isEnd) {
        char next = getch();
        switch (next)
        {
            case 110:
                isEnd = false;
                main();
                break;
                
            case 113:
                isEnd = false;
                endwin();
                break;
        }
    }
   

    getch();



    endwin();

}