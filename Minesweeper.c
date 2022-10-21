// See end of file for test cases
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h> // to clear the screen
#define MIN_WIDTH 5
#define MAX_WIDTH 30
#define MIN_HEIGHT 5
#define MAX_HEIGHT 30
#define MIN_MINES 1
#define DEBUG_MODE 0
_Bool test_mode = 0;
// max_mine is width * height / 4

// Clear Screen
void clear_screen()
{
    if (DEBUG_MODE || test_mode)
        printf("\n\n-- Screen Cleared --\n\n");
    else
        system("cls || clear");
}


/* Game Play Functions */
// initialize_map
//   int width: width of map
//   int height: height of map
//   int * map: pointer to output map array
// Initializes each value to 0
void initialize_map (int width, int height, int * map);

// plant_mines
//   int num: number of mines
//   int width: width of map
//   int height: height of map
//   int[][2] mine_positions: pointer to output array of mine positions
// Sets the positions of each mine in the mine_positions array
//   mine_positions[i][0] = row
//   mine_positions[i][1] = column
void plant_mines (int num, int width, int height, int mine_positions[][2]);

// generate_map
//   int width: width of map
//   int height: height of map
//   int num_mines: number of mines
//   int * free_positions: number of free positions (adds one for each duplicate mine)
//   int[][2] mine_positions: positions of mines
//   int * map: pointer to output map array w/ each tile init. to 0
//     -19-10 = guessed mine
//     -9 = mine
//     -8 through -1 = closed with surrounding mines
//     0 = closed with no surrounding mines
//     1-8 = open with surrounding mines
//     9 = revealed mine (for end game)
//     10 = open with no surrounding mines
//
// Generates the map by looping through each mine and subtracting 1 from each surrounding tile
// If two or more mines share the same location, it skips that mine and adds one to the number of free positions
void generate_map (int width, int height, int num_mines, int * free_positions, int mine_positions[][2], int * map);

// reveal_tile
//   int column: column of revealed tile (starts at 0)
//   int row: row of revealed tile (starts at 0)
//   int * score: pointer to score variable
//   time_t: start time of the game
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map array
// If the tile is a mine, prints the game over screen
// If the tile is not a mine, it
//      * if it's already open, it does nothing
//      * otherwise, it flips the tile to open
//      * if it's 0 (now 9), it recursively flips the neighboring tiles
//      * otherwise, it stops
//      * increments score whenever a tile is flipped.
void reveal_tile (int column, int row, int * score, time_t start_time, int width, int height, int * map);

// reveal_map
//   int width
//   int height
//   int * map
// Reveals the entire map
void reveal_map (int width, int height, int * map);

// int mark_tile
//   int row
//   int column
//   int * map
// If the tile is hidden, marks the tile. Returns the original value of the tile or -100 if the row, column is out of range
int mark_tile (int row, int column, int width, int height, int * map);

/* UI Functions */
// welcome_screen
//   int * width: pointer to map width variable
//   int * height: pointer to map height variable
//   int * num_mines: pointer to number of mines variable
void welcome_screen (int * width, int * height, int * num_mines);

// lose_screen
//   int score: number of tiles overturned.
//   int time: time since start of game in seconds.
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map array
// Prints a lose screen displaying your score, time, and the revealed map
void lose_screen (int score, int start_time, int width, int height, int * map);

// win_screen
//   int score: number of tiles overturned.
//   int start_time: time of start of game in seconds.
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map array
// Prints a win screen displaying your score, time and the revealed map
void win_screen (int score, int start_time, int width, int height, int * map);

// mark_screen
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map
// Lets the user guess the position of a mine
void mark_screen (int width, int height, int * map);

// guess_screen
//   int * score: pointer to veriable for number of tiles cleared
//   time_t start_time: start time of the game
//   int * free_positions: pointer to variable for number of free positions.
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map array
// Prints the number of free positions and your current score
// Prints map with new guess.
// Asks user for row and column of guessed tile. 
void guess_screen (int * score, time_t start_time, int * free_positions, int width, int height, int * map);

// test_screen
// Asks user if they want to play the game or test the game.  If they want to test, it runs test cases and then exists the program
void test_screen();

// draw_tile
//   int row: row of the tile (starts at 0)
//   int column: column of tile (starts at 0)
//   int width: width of map
//   int * map: pointer to map array
// Prints a single map tile according to the following format
//      -19 through -10 = ?
//      -9 through 0 = #
//      1-8 = 1-8
//      9 = *
//      10 = 0
void draw_tile (int column, int row, int width, int * map);

// draw_map
//   int width: width of map
//   int height: height of map
//   int * map: pointer to map array
// Prints the map with the number for each column and row and each tile using draw_tile. 
void draw_map (int width, int height, int * map);


/* Main */
int main (void)
{
    // Random
    srand(time(NULL));

    // Ask if user wants to play the game or test the game
    test_screen();

    // Map
    int width, height, num_mines;
    welcome_screen(&width, &height, &num_mines);

    int map[width][height];
    int mine_positions[num_mines][2];
    initialize_map(width, height, (int *)map);
    plant_mines(num_mines, width, height, mine_positions);
    
    // Score
    time_t start_time = time(NULL);
    int score = 0; // number of cleared tiles
    int num_free = width * height - num_mines; // number of free spaces left
    
    // Generate Map
    printf("num_mines = %d\n", num_mines);
    generate_map(width, height, num_mines, &num_free, mine_positions, (int *)map);
    printf("num_mines = %d\n", num_mines);

    // Make Guesses Until the Game is over
    while (num_free > 0)
    {
        guess_screen(&score, start_time, &num_free, width, height, (int *)map);
    }

    return 0;
}


// Initinalize the map to have all 0's
void initialize_map (int width, int height, int * map)
{
    for (int row = 0; row < height; row ++)
    {
        for (int column = 0; column < width; column ++)
        {
            *(map + row*width + column) = 0; // Set map[i][j] = 0
        }
    }
}

// Generate a bunch of random mine positions
void plant_mines (int num, int width, int height, int mine_positions[][2])
{
    for (int i = 0; i < num; i ++)
    {
        mine_positions[i][0] = rand() % height; // Random row position
        mine_positions[i][1] = rand() % width; // Random column position
    }
}

// Create the list of numbers that are hidden behind each tile (assumes map is initialized with 0's)
void generate_map (int width, int height, int num_mines, int * free_positions, int mine_positions[][2], int * map)
{
    // For each mine, add one to the surrounding zero
    for (int i = 0; i < num_mines; i ++)
    {
        int * mine = mine_positions[i];

        if (DEBUG_MODE) draw_map(width, height, map);

        // Set the mine position in the map as a mine
        int * tile = (map + mine[0]*width + mine[1]);
        if (*tile == -9 || *tile == -19 | *tile == 9) // If the tile already has a mine placed, ingore the second one
        {
            *free_positions = *free_positions + 1;
            continue;
        }
        // otherwise, set the tile to be a mine
       *tile = -9;

        // Subtract one from all neighboring tiles (unless it's a mine)
        for (int j = -1; j <= 1; j ++)
        {
            for (int k = -1; k <= 1; k ++)
            {
                // Don't change the tile we just turned into a mine
                if (j == 0 && k == 0) continue;

                // Get the row and column of the neighboring tile
                int row = mine[0] + j;
                int column = mine[1] + k;
                
                // If the index is out of range
                if (row < 0 || column < 0 || row >= height || column >= width) continue;

                // Get the address of the neighboring tile
                tile = map + (row*width + column);

                // If the neighbor's a mine
                // Technically, only (*tile == -9) matters since the other options only show after
                //   the game starts and the map is already generated
                if (*tile == -9 || *tile == -19 || *tile == 9) continue;

                // If tile is negative (hidden), subtract one (add one neighboring mine)
                if (*tile <= 0) *tile = *tile - 1;
                // If the tile is 10 (open 0), set it to 1
                else if (*tile == 10) *tile = 1;
                // If the tile is positive (open), add one
                else *tile = *tile + 1;
            }
        }
    }
}

void reveal_tile (int column, int row, int * score, time_t start_time, int width, int height, int * map)
{
    // Check if the tile is out of range
    if (column >= 0 && column < width && row >= 0 && row < height)
    {
        // Pointer to the map tile to be revealed
        int * tile = map + (row * width + column);

        // If it's a mine
        if (*tile == -9 || *tile == -19 || *tile == 9)
        {
            lose_screen(*score, start_time, width, height, map);
        }
        // Otherwise, only if the tile hasn't already been flipped, flip it
        else if (*tile <= 0)
        {
            // Increment the score
            *score = *score + 1;

            // If it's zero, set it to ten and flip its neighbors
            if (*tile == 0 || *tile == -10)
            {
                *tile = 10; // Flip the tile
                
                // Loop through neighbors
                for (int i = -1; i <= 1; i ++)
                {
                    for (int j = -1; j <= 1; j ++)
                    {
                        if (i == 0 && j == 0) continue;
                        reveal_tile(column + i, row + j, score, start_time, width, height, map);
                    }
                }
            }       
            // If it has been flagged, add ten and then flip the sign
            else if (*tile < -10)
                *tile = -(*tile + 10);
            // If it's just an ordinary hidden tile, flip its sign
            else
                *tile = -(*tile);
        }
    }
}

// Reveals the entire map for end game
void reveal_map (int width, int height, int * map)
{
    // Loop through each tile
    for (int column = 0; column < width; column ++)
    {
        for (int row = 0; row < height; row ++)
        {
            int * tile = map + (row*width + column);

            // Format each tile correctly
            if (*tile >= -19 && *tile < -10) // Hidden Tile Marked as Potential Mine
                *tile = -(*tile + 10);
            else if (*tile < 0) // Hidden Tile
                *tile = -*tile;
            else if (*tile == -10 || *tile == 10 || *tile == 0) // Tile with no neighbors
                *tile = 10; // Make all tiles with no neighbors have the same format
        }
    }
}

// Draw an individual tile
void draw_tile (int column, int row, int width, int * map)
{
    int tile = *(map + row*width + column); 
    if (DEBUG_MODE) // so that you can print extra information when debugging
    {
        if (tile >= -19 && tile <= -10) // Hidden Tile Marked as Potential Mine
            printf("?");
        else if (tile == 10 || tile == 0) // Revealed Tile with No Neighboring Mines
            printf(".");
        else if (tile < 0) // Hidden Tile
            printf("%d", -tile);
        else if (tile <= 8) // Revealed Tile with Neighboring Mines
            printf("%d", tile);
        else if (tile == 9) // Revealed Mines
            printf("#");
        else // Error
            printf("E: %d", tile);
    }
    else // here's what normally gets printed
    {
        if (tile >= -19 && tile <= -10) // Hidden Tile Marked as Potential Mine
            printf("?");
        else if (tile <= 0) // Hidden Tile
            printf(".");
        else if (tile <= 8) // Revealed Tile with Neighboring Mines
            printf("%d", tile);
        else if (tile == 9) // Revealed Mines
            printf("#");
        else if (tile == 10) // Revealed Tile with No Neighboring Mines
            printf(" ");
        else // Error
            printf("E: %d", tile);
    }
}

// Draw the entire map
void draw_map (int width, int height, int * map)
{
    // Loop through the map, plus an extra column and row before and after for printing column/row
    //   numbers and for printing the map border
    for (int row = -1; row < height+1; row ++)
    {
        for (int column = -1; column < width+1; column ++)
        {
            // Each table "cell" consists of four characters (except for the last column)

            // Blank tab in the corner
            if(row == -1 && column == -1)
                printf("    ");
            // Bottom corner
            else if (row == height && column == width)
                printf("+\n");
            // Right Border
            else if (column == width)
                printf("|\n");
            // Bottom Bornder
            else if (row == height)
                printf("----");
            // Column Numbers
            else if (row == -1)
            {
                char colStr[5];
                sprintf(colStr, "C%d", column + 1);
                printf("%-4s", colStr); // Pads with spaces on the right so that it's total width is 4 
            }
            // Row Numbers
            else if (column == -1)
            {
                char rowStr[5];
                sprintf(rowStr, "R%d", row + 1);
                printf("%-4s", rowStr); // Pads with spaces on the right so that it's total width is 4
            }
            // Tiles
            else
            {
                draw_tile(column, row, width, map);
                printf("   "); // Print 3 extra spaces so that it's total width is 4 (since tiles are one character long)
            }
        }
    }
}

// Welcome Screen and initial user input
void welcome_screen (int * width, int * height, int * num_mines)
{
    // Clear screen
    clear_screen();

    printf("Welcome to Minesweep!\n\n");
    printf("SETTINGS:\n");
    
    // Get the width of the map
    do {
        printf("How many COLUMNS would you like the map to have?\n");
        printf("Please enter a while number between %d and %d: ", MIN_WIDTH, MAX_WIDTH);
        if (scanf("%d", width) != 1 || *width < MIN_WIDTH || *width > MAX_WIDTH)
            printf("ERROR: Please type a whole number within the proper range\n");

        while (getchar() != '\n') {}
    } while (*width < MIN_WIDTH || *width > MAX_WIDTH);
    printf("#COLUMNS: %d\n\n", *width);

    // Get the height of the map
    do {
        printf("How many ROWS would you like the map to have?\n");
        printf("Please enter a while number between %d and %d: ", MIN_HEIGHT, MAX_HEIGHT);
        if (scanf("%d", height) != 1 || *height < MIN_HEIGHT || *height > MAX_HEIGHT)
            printf("ERROR: Please type a whole number within the proper range\n");

        while (getchar() != '\n') {}
    } while (*height < MIN_HEIGHT || *height > MAX_HEIGHT);
    printf("#ROWS: %d\n\n", *height);

    // Get the number of mines
    int max_mines = *width * *height / 9; // maximum number of mines you're allowed to place 
    do {
        printf("How many MINES would you like the map to have?\n");
        printf("Please enter a while number between %d and %d: ", MIN_MINES, max_mines);
        if (scanf("%d", num_mines) != 1 || *num_mines < MIN_MINES || *num_mines > max_mines)
            printf("ERROR: Please type a whole number within the proper range");

        while (getchar() != '\n') {}
    } while (*num_mines < MIN_MINES || *num_mines > max_mines);
    printf("#MINES: %d\n\n", *num_mines);

    printf("Perfect!  Now let's get started!\n\n");
}

// Mark a tile as a potential mine
int mark_tile (int row, int column, int width, int height, int * map)
{
    // If (row, column) is out of range, return -100
    if (row >= height || column >= width || row < 0 || column < 0) return -100;

    // Find the tile to be marked
    int * tile = map + row*width + column;
    int original_tile = *tile;

    // If the tile is hidden but unmarked, mark it
    if (*tile >= -9 && *tile <= 0)
        *tile = *tile - 10;
    // If the tile is hidden but already marked, unmark it
    else if (*tile < -9)
        *tile = *tile + 10;
    // Otherwise, do nothing to it

    // Return the original_tile so that you can error handle when unhidden tiles are marked
    return original_tile;
}

// User input for marking tiles
void mark_screen (int width, int height, int * map)
{
    // Clear the screen
    clear_screen();

    // Display title and map
    printf("Mark/Unmark a Tile as a Potential Mine:\n\n");
    draw_map(width, height, map);

    // Get the row and column of the tile you want to mark
    int row, column;
    printf("ENTER THE TILE YOU WANT TO MARK/UNMARK\n");
    do
    {
        printf("Enter the row and column number separated by a comma (e.g. 5, 3)\n");
        printf("Row must be whole number from 1-%d\n", height);
        printf("Column must be whole number from 1-%d\n", width);

        // Make sure the input is valid
        if (scanf("%d, %d", &row, &column) != 2)
            printf("ERROR: Incorrect format.  Make sure you enter two integers separated by just a comma and optionally a space.\nTry again.\n\n");
        else if (row < 1 || column < 1 || row > height || column > width)
        {
            printf("Please make sure the row and column are within the correct range.\n");
            printf("You inputted row = %d, column = %d\n", row, column);
            printf("Try again.\n\n");
        }

        // Clear the rest of the buffer
        while (getchar() != '\n') {}
    } while (row < 1 || column < 1 || row > height || column > width);

    clear_screen();
    printf("Mark/Unmark a Tile as a Potential Mine:\n\n");

    int result = mark_tile(row-1, column-1, width, height, map);
    draw_map(width, height, map);
    if (result > 0) printf("\nTile has already been revealed.\n\n");

    printf("Would you like to mark/unmark another tile?\n");
    char cont;
    do {
        printf("Enter y for yes, or n for no: ");
        cont = getchar();
        while (getchar() != '\n') continue;
        if (cont != 'y' && cont != 'n') printf("You must enter either 'y' or 'n' (without the quotes).\nTry again.\n\n");
    } while (cont != 'y' && cont != 'n');
    printf("You entered %c\n\n", cont);

    if (cont == 'y') mark_screen(width, height, map);
}


void guess_screen (int * score, time_t start_time, int * free_positions, int width, int height, int * map)
{
    // Clear screen
    clear_screen();

    // Print current score/status
    printf("STATUS:\n");
    printf("Score: %d\n", *score);
    printf("Remaining Tiles to Clear: %d\n\n", *free_positions);

    // Print map
    printf("MAP:\n");
    draw_map(width, height, map);
    printf("\n");

        // Get Row and Column
    int row, column;
    do
    {
        char option;
        do {
            printf("ENTER m TO MARK A MINE, g TO GUESS AN EMPTY SPACE OR q TO QUIT: ");
            option = getchar();
            while (getchar() != '\n') continue;
            if (option != 'm' && option != 'g' && option != 'q') printf("Option not recognised.  Please type either 'm', 'g', or 'q' (without the quotes).\n\nTry again\n");
        } while (option != 'm' && option != 'g' && option != 'q');
        printf("You entered %c\n\n", option);

        if (option == 'q')
            exit(0);
        else if (option == 'm')
            mark_screen(width, height, map);

        printf("\nGuess a Clear space\nENTER GUESS:\n");

        printf("Enter the row and column number separated by a comma (e.g. 5, 3)\n");
        printf("Row must be whole number from 1-%d\n", height);
        printf("Column must be whole number from 1-%d\n", width);
        if (scanf("%d, %d", &row, &column) != 2)
            printf("ERROR: Incorrect format.  Make sure you enter two integers separated by just a comma and optionally a space.\nTry again.\n\n");
        else if (row < 1 || column < 1 || row > height || column > width)
        {
            printf("Please make sure the row and column are within the correct range.\n");
            printf("You inputted row = %d, column = %d\n", row, column);
            printf("Try again.\n\n");
        }
        while (getchar() != '\n') {}
    } while (row < 1 || column < 1 || row > height || column > width);

    // Flip the 
    *free_positions = *free_positions + *score; // Get back original free positions
    reveal_tile(column-1, row-1, score, start_time, width, height, (int *)map);
    *free_positions = *free_positions - *score; // Adjust the number of free positions
    printf("free_positions: %d\n", *free_positions); // Subtract total score from original free positions
                                               
    // Win Screen
    if (*free_positions <= 0)
    {
        win_screen(*score, start_time, width, height, map);
    }
}
    
void lose_screen (int score, int start_time, int width, int height, int * map)
{
    clear_screen();

    printf("KABOOM!!!!\n");
    printf("You stepped on a mine!\n\n");
    
    printf("Score: %d\n", score);
    printf("Time: %d seconds\n", time(NULL) - start_time);
    
    printf("MAP:\n");
    reveal_map(width, height, map);
    draw_map(width, height, map);

    printf("\nBetter luck next time!\n");

    if (!test_mode) exit(0);
}
void win_screen (int score, int start_time, int width, int height, int * map)
{
    clear_screen();

    printf("YOU WIN!!!\n");
    printf("Congratulations!  You made it through the mine field!\n\n");

    printf("Score: %d\n", score);
    printf("Time: %d seconds\n", time(NULL) - start_time);

    printf("MAP:\n");
    draw_map(width, height, map);
}

void test_screen()
{
    // Ask user if they want to test or play
    printf("Would you like to play the game or test it?\n");
    char option;
    do {
        printf("Enter p to play and t to test: ");
        option = getchar();
        while(getchar() != '\n') continue;
        if (option != 'p' && option != 't')
            printf("Please enter either 'p' or 't' (without the quotes).\n\nTry again.\n");
    } while (option != 'p' && option != 't');
    printf("You entered %c\n\n", option);

    // Test the game
    if (option == 't')
    {
        test_mode = 1;
        int width = 5;
        int height = 4;
        int map[height][width];
        int score = 0;
        time_t start_time = time(NULL);

        printf("\nTesting the game:\n\n");
        // Test case 1: Win the game
        printf("Test Case 1: Winning the game\n\n");
        printf("Initializing map with 0's...\n");
        initialize_map(width, height, (int *)map);

        printf("Adding mine to last position...\n");
        map[height-1][width-1] = -9;
        map[height-2][width-1] = -1;
        map[height-2][width-2] = -1;
        map[height-1][width-2] = -1;

        printf("Drawing hidden map...\n");
        draw_map(width, height, (int *)map);

        printf("Guessing position (0, 0)...\n");
        reveal_tile(1, 1, &score, start_time, width, height, (int *)map);

        printf("Drawing new map...");
        draw_map(width, height, (int *)map);

        win_screen(score, start_time, width, height, (int *)map);

        printf("Press ENTER to continue to next test");
        while (getchar() != '\n') continue;

        printf("\n\nTest Case 2: Losing the game\n\n");
        printf("Keeping the same map as before, resetting score and time...\n");
        score = 0;
        start_time = time(NULL);

        printf("Guessing position (width-1, height-1)...\n");
        reveal_tile(width-1,height-1, &score, start_time, width, height, (int *)map);

        printf("Press ENTER to continue to next test");
        while (getchar() != '\n') continue;

        printf("\n\nTest Case 3: Too many mines\n");
        printf("Randomly generating more mines than tiles...\n");
        int free_positions = width * height;
        int num_mines = width * height * 2;
        int mine_positions[num_mines][2];
        plant_mines(num_mines, width, height, mine_positions);

        printf("Printing mine positions:\n");
        for (int i = 0; i < num_mines; i ++) printf("Mine #%d: [%d, %d]\n", i, mine_positions[i][0], mine_positions[i][1]);

        printf("Generating map:\n");
        generate_map(width, height, 2*width*height, &free_positions, mine_positions, (int *)map);

        printf("Revealing map:\n");
        reveal_map(width, height, (int *)map);

        printf("Drawing map:\n");
        draw_map(width, height, (int *)map);

        printf("If some positions are not covered with mines, that's because mines are randomly placed and redundant mines are ingored, meaning there can be fewer mines that the user inputted.\n");

        printf("Press ENTER to continue to next test");
        while (getchar() != '\n') continue;

        printf("Test Case 4: Invalid Input\n");
        printf("Now play the game in test mode (does not clear screen) with invalid input:\n\n");

    }
}

