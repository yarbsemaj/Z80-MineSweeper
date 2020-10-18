// Include files
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// Definitions
#define BOARDSIZE 12
// VT100 Attributes
#define BRIGHT 1
#define DIM 2
#define UNDERSCORE 4
#define BLINK 5
#define REVERSE 7
#define HIDDEN 8

#define EMPTY_SPACE '-'
#define FLAG '>'
#define MINE '*'

// Global variables
char board[BOARDSIZE][BOARDSIZE];
char gboard[BOARDSIZE][BOARDSIZE];

// Function prototypes
void build_board();
void build_gboard();
void print_board();
void print_gboard();
int findnearbymines(int, int);
void flushoutnearbymines(int, int);
void loss();
void win();
void playAgain();
bool checkforwin();
void playagain();
void display_welcome();
void start_game();
void cls();
void go_to_home();
void game_header();
void set_colour(int, int);
void reset_attributes();
void set_attribute(int);
int count_mines();
int count_flags();

int numberOfMines;

/**********************************************
 * Main function. Handles gameplay.
 **********************************************/
int main()
{
    int seed;

    cls();
    go_to_home();

    display_welcome();

    printf("Enter a seed for the random number: ");
    scanf("%d", &seed);
    srand(seed);

    start_game();
    return 0;
}

/******************************************
 * Simply prints the full board with mines.
 ******************************************/
void print_board()
{
    int i, j;
    game_header();
    // Print the first row of numbers.
    printf("   ");
    for (i = 1; i < BOARDSIZE - 1; i++)
        printf(" %d ", i);
    printf("\n");

    // Print actual board w/ column numbers.
    for (i = 0; i < BOARDSIZE; i++)
    {
        for (j = 0; j < BOARDSIZE; j++)
        {
            switch (board[i][j])
            {
            case EMPTY_SPACE:
                set_colour(7, 7);
                break;
            case MINE:
                set_colour(61, 7);
                break;
            }
            printf(" %c ", board[i][j]);
            reset_attributes();
        }
        if (i > 0 && i < BOARDSIZE - 1)
            printf("%d", i);
        printf("\n");
    }
}

/*********************************************
 * Prints the board used for most in-game
 * functions.
 *********************************************/
void print_gboard()
{
    int i, j;
    game_header();
    // Print the first row of numbers.
    printf("   ");
    for (i = 1; i < BOARDSIZE - 1; i++)
        printf(" %d ", i);
    printf("\n");

    // Hide the first and last rows along
    // with the first and last columns.
    for (i = 0; i < BOARDSIZE; i++)
        for (j = 0; j < BOARDSIZE; j++)
            if (i == 0 || i == BOARDSIZE - 1)
                gboard[i][j] = ' ';

    for (j = 0; j < BOARDSIZE; j++)
        for (i = 0; i < BOARDSIZE; i++)
            if (j == 0 || j == BOARDSIZE - 1)
                gboard[i][j] = ' ';

    // Print the actual board w/ column numbers.
    for (i = 0; i < BOARDSIZE; i++)
    {
        for (j = 0; j < BOARDSIZE; j++)
        {
            switch (gboard[i][j])
            {
            case '0':
                set_colour(67, 67);
                break;
            case '1':
                set_colour(4, 67);
                break;
            case '2':
                set_colour(3, 67);
                break;
            case '3':
                set_colour(1, 67);
                break;
            case '4':
                set_colour(64, 67);
                break;
            case '5':
            case '6':
            case '7':
            case '8':
                set_colour(0, 67);
                break;
            case EMPTY_SPACE:
                set_colour(7, 7);
                break;
            case FLAG:
                set_colour(61, 7);
                break;
            default:
                break;
            }
            printf(" %c ", gboard[i][j]);
            reset_attributes();
        }
        if (i > 0 && i < BOARDSIZE - 1)
            printf("%d", i);
        printf("\n");
    }
    printf("Mines Remaining: %d\n", numberOfMines - count_flags());
}

/**********************************************
 * Builds the board with the mines. Uses two
 * extra rows and two extra columns with no
 * mines for checks performed later.
 **********************************************/
void build_board()
{
    int i, j;

    // Set all elements in the board to '-'
    for (i = 0; i < BOARDSIZE; i++)
        for (j = 0; j < BOARDSIZE; j++)
            board[i][j] = EMPTY_SPACE;

    // Place random mines (marked with '*')
    for (j = 0; j < BOARDSIZE; j++)
    {
        int randomr = rand() % (BOARDSIZE - 1) + 1;
        int randomc = rand() % (BOARDSIZE - 1) + 1;
        board[randomr][randomc] = MINE;
    }

    numberOfMines = count_mines();

    // Make sure the first & last row, along with
    // the first and last column contain no mines.
    for (i = 0; i < BOARDSIZE; i++)
        for (j = 0; j < BOARDSIZE; j++)
            if (i == 0 || i == BOARDSIZE - 1)
                board[i][j] = ' ';

    for (j = 0; j < BOARDSIZE; j++)
        for (i = 0; i < BOARDSIZE; i++)
            if (j == 0 || j == BOARDSIZE - 1)
                board[i][j] = ' ';
}

/**********************************************
 * Builds the board for user interaction. Same 
 * dimensions as the first board, just without
 * the mines. Every element will be a '-' 
 * character except for the extra rows and cols.
 ************************************************/
void build_gboard()
{
    int i, j;

    // Set all elements in the board to '-'
    for (i = 0; i < BOARDSIZE; i++)
        for (j = 0; j < BOARDSIZE; j++)
            gboard[i][j] = EMPTY_SPACE;

    // Make sure the first & last row, along with
    // the first and last column contain no mines.
    for (i = 0; i < BOARDSIZE; i++)
        for (j = 0; j < BOARDSIZE; j++)
            if (i == 0 || i == BOARDSIZE - 1)
                gboard[i][j] = ' ';

    for (j = 0; j < BOARDSIZE; j++)
        for (i = 0; i < BOARDSIZE; i++)
            if (j == 0 || j == BOARDSIZE - 1)
                gboard[i][j] = ' ';
}

/************************************************
 * Finds nearby mines, checks in all 8 directions.
 * Returns the number of nearby mines found.
 *************************************************/
int findnearbymines(int row, int col)
{
    int mines = 0;

    // Check up, down, left, right.
    if (board[row - 1][col] == MINE)
        mines++;
    if (board[row + 1][col] == MINE)
        mines++;
    if (board[row][col - 1] == MINE)
        mines++;
    if (board[row][col + 1] == MINE)
        mines++;

    // Check all diagonal directions
    if (board[row - 1][col + 1] == MINE)
        mines++;
    if (board[row - 1][col - 1] == MINE)
        mines++;
    if (board[row + 1][col + 1] == MINE)
        mines++;
    if (board[row + 1][col - 1] == MINE)
        mines++;

    return mines;
}

/************************************************
 * Handles a loss and asks if the user wishes
 * to play again..
 ************************************************/
void loss()
{
    printf("\nYou've lost the game!");
    playAgain();
}
/************************************************
 * Handles a winning game, asks user if they
 * wish to play again..
 ************************************************/
void win()
{
    printf("\nYou've won the game!");
    playAgain();
}

void playAgain()
{
    char ans;
    printf("\nWould you like to play again? (y/n): ");
    scanf(" %c", &ans);

    if (toupper(ans) == 'Y')
    {
        start_game();
    }

    else
        exit(0);
}

/*************************************************
 * Flushes out all the nearby mines. First checks
 * the current user position for nearby mines, 
 * then checks in all eight directions for other
 * locations without nearby mines until it finds
 * a position with a mine touching that position.
 *
 * It updates the game board with the number of 
 * mines touching that position.
 *************************************************/
void flushoutnearbymines(int row, int col)
{
    int nearbymines = 0;
    int i = 0;

    nearbymines = findnearbymines(row, col);

    //If we have alreay visited this space, skip it
    if (gboard[row][col] != EMPTY_SPACE && gboard[row][col] != FLAG)
    {
        return;
    }

    gboard[row][col] = (char)(((int)'0') + nearbymines);

    nearbymines = 0;
    // Checking nearby mines going up
    while (nearbymines < 1 && i < row)
    {
        nearbymines = findnearbymines(row - i, col);
        flushoutnearbymines(row - i, col);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going down
    while (nearbymines < 1 && row + i < BOARDSIZE - 1)
    {
        nearbymines = findnearbymines(row + i, col);
        flushoutnearbymines(row + i, col);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going left
    while (nearbymines < 1 && i < col)
    {
        nearbymines = findnearbymines(row, col - i);
        flushoutnearbymines(row, col - i);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going right
    while (nearbymines < 1 && col + i < BOARDSIZE - 1)
    {
        nearbymines = findnearbymines(row, col + i);
        flushoutnearbymines(row, col + i);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going right, down
    while (nearbymines < 1 && col + i < BOARDSIZE - 1 && row + i < BOARDSIZE - 1)
    {
        nearbymines = findnearbymines(row + i, col + i);
        flushoutnearbymines(row + i, col + i);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going right, up
    while (nearbymines < 1 && col + i < BOARDSIZE - 1 && i < row)
    {
        nearbymines = findnearbymines(row - i, col + i);
        flushoutnearbymines(row - i, col + i);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going left, up
    while (nearbymines < 1 && i < col && i < row)
    {
        nearbymines = findnearbymines(row - i, col - i);
        flushoutnearbymines(row - i, col - i);
        i++;
    }
    i = 0;
    nearbymines = 0;
    // Checking nearby mines going left, down
    while (nearbymines < 1 && i < col && row + i < BOARDSIZE - 1)
    {
        nearbymines = findnearbymines(row + i, col - i);
        flushoutnearbymines(row + i, col - i);
        i++;
    }
}

/*************************************************
 * Checks for a game that has been won. Searches
 * the entire game board for a position without a
 * mine that still has a '-' symbol indicating
 * that the user still has plays to make. If there
 * are no locations with a '-' symbol that does
 * not contain a mine at that cooresponding position, 
 * the player has won.
 **************************************************/
bool checkforwin()
{
    int i, j;

    for (i = 1; i < BOARDSIZE - 1; i++)
        for (j = 1; j < BOARDSIZE - 1; j++)
        {
            if (
                (board[i][j] == MINE && gboard[i][j] != FLAG) || 
                (board[i][j] != MINE && gboard[i][j] == FLAG) || 
                 (board[i][j] != MINE && gboard[i][j] == EMPTY_SPACE) )
                return false;
        }

    return true;
}

int count_flags()
{
    int i, j;

    int flags = 0;

    for (i = 1; i < BOARDSIZE - 1; i++)
        for (j = 1; j < BOARDSIZE - 1; j++)
        {
            if (gboard[i][j] == FLAG)
                flags++;
        }

    return flags;
}

int count_mines()
{
    int i, j;

    int mines = 0;

    for (i = 1; i < BOARDSIZE - 1; i++)
        for (j = 1; j < BOARDSIZE - 1; j++)
        {
            if (board[i][j] == MINE)
                mines++;
        }

    return mines;
}

/***********************************************
 * Starts the game and handles the loop to 
 * either continue playing or quitting.
 ***********************************************/
void start_game()
{
    int row, col;
    char mode;

    // Build both game boards

    build_board();
    build_gboard();

    cls();
    go_to_home();

    print_gboard();

    for (;;)
    {
        bool validInput = true;
        do
        {
            validInput = true;
            printf("Enter selection:\n");
            printf("Col: ");
            scanf("%d", &col);
            printf("Row: ");
            scanf("%d", &row);
            printf("Mode [(f)lag, (r)emove]: ");
            scanf(" %c", &mode);

            mode = toupper(mode);
            if (row < 1 || row > BOARDSIZE - 2)
            {
                printf("Row invlid!\n");
                validInput = false;
            }

            if (col < 1 || col > BOARDSIZE - 2)
            {
                printf("Colum invlid!\n");
                validInput = false;
            }

            if (mode != 'F' && mode != 'R')
            {
                printf("Mode invlid!\n");
                validInput = false;
            }
            if (mode == 'F' && gboard[row][col] != EMPTY_SPACE)
            {
                printf("You cant put a flag there!\n");
                validInput = false;
            }

        } while (!validInput);

        if (mode == 'R')
        {
            if (board[row][col] == MINE)
            {
                cls();
                go_to_home();
                print_board();
                printf("You hit a MINE!\n");
                loss();
            }
            else
            {
                flushoutnearbymines(row, col);
            }
        }
        else if (mode == 'F')
        {
            if (gboard[row][col] == FLAG)
            {
                gboard[row][col] = EMPTY_SPACE;
            }
            else
            {
                gboard[row][col] = FLAG;
            }
        }

        cls();
        go_to_home();

        print_gboard();

        if (checkforwin())
            win();
    }
}

// Displays the welcome message, and the GNU License
void display_welcome()
{
    game_header();
}

void game_header()
{
    printf("************* Minesweeper *************\n");
}

void cls()
{
    printf("\033[2J");
}

void go_to_home()
{
    printf("\033[H");
}

void set_colour(int forground, int background)
{
    forground = forground + 30;
    background = background + 40;
    printf("\033[%d;%dm", forground, background);
}

void set_attribute(int attribute)
{
    printf("\033[%dm", attribute);
}

void reset_attributes()
{
    printf("\033[0m");
}