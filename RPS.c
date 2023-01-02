// NOTE TO GRADER:
// In order for the program to work, make sure to move paper.txt, paper_hand.txt, rock.txt,
// rock_hand.txt, scissors.txt, scissors_hand.txt, and shoot.txt to a folder called
// RPS_ASCII_ART.  Gradescope wouldn't let me upload the entire folder, so I had to upload
// the files individually.
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#define MAX_ASCII_LINE_LENGTH 256
#define DEBUG_MODE false 

// Sleep Function
#ifdef _WIN32
#include <windows.h>
#define sleep(x) if (!DEBUG_MODE) Sleep(x)
#define EOF_TRIGGER "<Ctrl-C>"
#else
#include <unistd.h>
#define sleep(x) if (!DEBUG_MODE) usleep((x)*1000)
#define EOF_TRIGGER "<Ctrl-D>"
#endif

// Note: "player" refers to the computer player.
// "user" refers to the human player
typedef struct
{
    int id; // whether it's player 1 or player 2
    char hand; // what the player throws (r=rock, p=paper, s=scissors)
} Player;

// welcome_screen -> void
//      int *wins: pointer to variable with number of wins
//      int *rounds: pointer to the variable with total number of rounds
//      int *ties: pointer to the variable with the number of ties
// * Asks the user if they want to play (p) or test (t)
//  - If they want to play, asks the user if they want
//  to play against player 1 or 2 and then calls 'game_screen'.
//  - If they want to test, calls 'test_screen'.
// NOTE: Clears screen
void welcome_screen (int *wins, int *rounds, int *ties);

// game_screen -> void
//      Player comp: the computer player struct
//      int *wins: pointer to variable with number of wins
//      int *rounds: pointer to variable with total number of rounds
//      int *ties: pointer to the variable with the number of ties
// * Prompts the user to guess rock (r), paper (p), or scissors (s)
// and passes the player's choice to the 'shoot' function.
// NOTE: Clears screen
void game_screen (Player comp, int *wins, int *rounds, int *ties);

// test_screen -> void
//      int *wins: pointer to variable with number of wins
//      int *rounds: pointer to the variable with total number of rounds
//      int *ties: pointer to the variable with the number of ties
// * Runs test cases
// NOTE: Clears screen
void test_screen (int *wins, int *rounds, int *ties);

// shoot -> bool 
//      char user_hand: the user's hand (r=rock, p=paper, s=scissors)
//      Player comp: the computer player struct
//      int *wins: pointer to variable with number of wins
//      int *rounds: pointer to the variable with total number of rounds
//      int *ties: pointer to the variable with the number of ties
// * Pick's the computer's hand
// * Increments wins as necessary.
// * Displays a beautiful ASCII art animation of rock, paper, scissors
// * Reveals the computer's hand
// * Displays calls either win_screen or lose_screen
// * Calls 'prompt_quit' and exits if the user desires
// * Calls 'game_screen' if the user wishes to continue.
// NOTE: Clears screen
void shoot (char user_hand, Player comp, int *wins, int *rounds, int *ties);

// stats_screen
//      char user_hand: the user's hand (r, p, or s)
//      Player comp: the computer player struct
//      int wins: number of user wins 
//      int rounds: number of total rounds
//      int ties: number of ties
// * Displays a the computer's and user's hand as well as how many rounds won, tied, etc.
void stats_screen(char user_hand, Player comp, int wins, int rounds, int ties);

// win_screen
//      char user_hand: the user's hand (r, p, or s)
//      Player comp: the computer player struct
//      int wins: number of user wins 
//      int rounds: number of total rounds
//      int ties: number of ties
// * Displays a "You Win" message along with the computer's hand and
// the user's hand. 
// * Returns the result of 'prompt_quit'
void  win_screen(char user_hand, Player comp, int wins, int rounds, int ties);


// loose_screen
//      char user_hand: the user's hand (r, p, or s)
//      Player comp: the computer player struct
//      int wins: number of user wins 
//      int rounds: number of total rounds
//      int ties: number of ties
// * Displays a "You Lose" message along with the computer's hand and
// the user's hand. 
// * Returns the result of 'prompt_quit'
void loose_screen(char user_hand, Player comp, int wins, int rounds, int ties);

// tie_screen
//      char user_hand: the user's hand (r, p, or s)
//      Player comp: the computer player struct
//      int wins: number of user wins 
//      int rounds: number of total rounds
//      int ties: number of ties
// * Displays a "Tie" message along with the computer's hand and
// the user's hand. 
// * Returns the result of 'prompt_quit'
void tie_screen(char user_hand, Player comp, int wins, int rounds, int ties);


// prompt_quit -> bool
// Prompts the user whether to quit (q) or continue (y).
// Returns true for quit and false for continue.
char prompt_quit ();

// char_select -> char
//      const char *prompt: message of the prompt
//      int num_options: number of options
//      const char options[num_options]: array of possible options
//      const char *descriptions[num_options]: array of description strings
//                                      for each option
char char_select (  const char *prompt,
                    int num_options,
                    const char options[num_options],
                    const char *descriptions[num_options] );

// Clear screen
void cls();

/* ASCII Art Functions **/
// Prints the welcome screen
void print_welcome();

// animate_ascii_sprite -> void
//      int max_sprite_size: maximum number of characters that a sprite frame can take 
//          (should not need to exceed 256*60, as <256 characters typically fit on a line
//          and <60 lines typically fit on a screen)
//      int num_frames: number of frames in the animation
//      char *sprite_array[num_frames]: output array of sprite frames
//      int frame_rate: frame rate of animation
//      char *before_str: string printed before sprite frame
//      char *after_str: string printed after sprite frame
// Prints the before_str followed by the sprite frame and then after_str
// Delays 1000 miliseconds / frame_rate
// Prints the next frame
// WARNING: *sprite_array must be array of valid strings ending in '\0'
void animate_ascii_sprite(int max_sprite_size, int num_frames, char sprite_array[num_frames][max_sprite_size],
                          int frame_rate, char *before_str, char *after_str);

// get_sprite_array -> void
//      FILE *sprite: a pointer to the file with the sprite frames
//      char separator_char: char that separates each sprite frame 
//          (this chararacter must be alone on a line between frames)
//      int max_sprite_size: maximum number of characters that a sprite frame can take 
//          (should not need to exceed 256*60, as <256 characters typically fit on a line
//          and <60 lines typically fit on a screen)
//      int num_frames: number of frames in the animation
//      char sprite_array[num_frames][max_sprite_size]: output array of sprite frames
//  Uses 'separator_line' to split a file containing ASCII sprite frames into a list of strings containing
//  each frame of animation
void get_sprite_array ( FILE *sprite, char separator_char, int max_sprite_size,
                        int num_frames, char sprite_array[num_frames][max_sprite_size]);

int main(void)
{
    srand(time(NULL));

    int wins = 0;
    int rounds = 0;
    int ties = 0;
    welcome_screen(&wins, &rounds, &ties);
    return 0;
}


char char_select (  const char *prompt,
                    int num_options,
                    const char options[num_options],
                    const char *descriptions[num_options] )
{
    char selection;
    char buffer;
    _Bool verified = 0;
    do {
        // Verify the selection
        // Print the prompt and options
        printf("%s\n", prompt);
        for (int i = 0; i < num_options; i++)
            printf("'%c' = %s\n", options[i], descriptions[i]);

        // Get the selection
        printf("Enter your choice: ");
        selection = getchar();

        // Clear the buffer
        while ((buffer=getchar()) != '\n' && buffer != EOF) continue;
        if (buffer == EOF)
        {
            printf("User entered EOF (%s).  Exiting program.\n", EOF_TRIGGER);
            exit(0);
        }

        for (int i = 0; i < num_options; i++)
        {
            if (selection == options[i])
            {
                verified = 1;
                continue;
            }
        }
        if (!verified)
            printf("Invalid input '%c' (%d).  Please enter one of the options above "
                    "(without quotes)\n\nTry again...\n", selection, selection);
    } while (!verified);

    return selection;
}

void cls()
{
    system("cls || clear");
}

// Asks the user if they want to play (p) or test (t)
//  - If they want to play, asks the user if they want
//  to play against player 1 or 2 and then calls 'game_screen'.
//  - If they want to test, calls 'test_screen'.
// NOTE: Clears screen
void welcome_screen (int *wins, int *rounds, int *ties)
{
    char start_option;
    char start_options[2] = {'p', 'q'};
    char *option_descriptions[2] = {"play", "quit"};

    // Welcome
    print_welcome();
    start_option = char_select( "Would you like to play or test the game?",
                                2, start_options, (const char**)option_descriptions);

    // If quit
    if (start_option == 'q') 
        exit(0);
    // If test
    else if (start_option == 't')
        test_screen(wins, rounds, ties);
    // If play
    else
    {
        // Prompt the user which (computer) player they want to play against
        char player_option;
        char player_options[3] = {'1', '2', 'q'};
        char *player_desctiptions[3] = {"player1 (medium)", "player2 (easy)", "quit"};
        player_option = char_select("Which player would you like to play against?",
                                    3, player_options, (const char **)player_desctiptions);
        
        // Create the player
        Player computer;
        if (player_option == '1') computer.id = 1;
        else if (player_option == '2') computer.id = 2;
        else exit(0);

        // Play the game
        game_screen(computer, wins, rounds, ties);
    }
}

// Prompts the user to guess rock (r), paper (p), or scissors (s)
// and passes the player's choice to the 'shoot' function.
// NOTE: Clears screen
void game_screen (Player comp, int *wins, int *rounds, int *ties)
{
    cls();

    // Prompt user for their hand
    char hand;
    char hands[3] = {'r', 'p', 's'};
    char *hand_descriptions[3] = {"rock", "paper", "scissors"};
    hand = char_select( "What would you like to throw?", 
                        3, hands, (const char **)hand_descriptions);

    // Shoot!
    shoot(hand, comp, wins, rounds, ties);

    // Check if the user wants to quit or play again
    if (prompt_quit() == 'q') exit(0);

    // Play again
    game_screen(comp, wins, rounds, ties);
}

// Runs test cases
// NOTE: Clears screen
void test_screen (int *wins, int *rounds, int *ties)
{
    // Initialize Variables for Prompting User to Start Test or Quit
    char option;
    const char options[] = {'s', 'q'};
    const char *descriptions[] = {"Start", "Quit"};
    
    // initialize Players
    Player player1;
    Player player2;
    player1.id = 0;
    player2.id = 2;

    // Test Player 1
    cls();
    printf("Testing ROCK, PAPER, SCISSORS!!!\n");
    printf("First, we test PLAYER 1.\n\n");
    
    option = char_select("First test for Player 1:\nPLAYER 1 = \"paper\"\nUSER = \"rock\"\nDo you want to start the test?",
                            2, options, (const char **)descriptions);
    if (option == 'q') exit(0);
    player1.hand = 'p';
    shoot('r', player1, wins, rounds, ties);

    option = char_select("\nSecond test for Player 1:\nPlayer 1 = \"scissors\"\nUSER = \"rock\"\nDo you want to start the test?",
                            2, options, (const char **)descriptions);
    if (option == 'q') exit(0);
    player1.hand = 's';
    shoot('r', player1, wins, rounds, ties);

    option = char_select("\nThird test for Player 1:\nPlayer 1 = \"scissors\"\nUSER = \"paper\"\nDo you want to start the test?",
                            2, options, (const char **)descriptions);
    if (option == 'q') exit(0);
    player1.hand = 's';
    shoot('p', player1, wins, rounds, ties);

    option = char_select("\nFourth test for Player 1:\nPlayer 1 = \"scissors\"\nUSER = \"scissors\"\nDo you want to start the test?",
                            2, options, (const char **)descriptions);
    if (option == 'q') exit(0);
    player1.hand = 's';
    shoot('s', player1, wins, rounds, ties);
    
    // Test Player 2
    printf("\n\nFinally, we test PLAYER 2.\nFor these tests, the USER will always pick \"paper\"");

    for (int i = 1; i <= 10; i++)
    {
        printf("\n\nPLAYER 2, Test #%d/10\n", i);
        option = char_select("Do you want to start the test?",
                2, options, (const char **)descriptions);
        if (option == 'q') exit(0);
        shoot('p', player2, wins, rounds, ties);
    }

    // End
    printf("All Tests Complete\n\n");
}

void print_welcome()
{
    char sprite_frames[8][60*256];
    char before_string[60*256*3] = "Welcome to\n";
    int frame_rate = 6;
    FILE *rock_sprite = fopen("RPS_ASCII_ART/rock.txt", "r");
    FILE *paper_sprite= fopen("RPS_ASCII_ART/paper.txt", "r");
    FILE *scissors_sprite = fopen("RPS_ASCII_ART/scissors.txt", "r");
    
    // Animate Rock, Paper, Scissors
    get_sprite_array(rock_sprite, '*', 60*256, 4, sprite_frames); 
    animate_ascii_sprite(60*256, 4, sprite_frames, frame_rate, before_string, "");
    strcat(before_string, sprite_frames[3]); // Add ASCII "ROCK" to before string
    sleep(500);

    get_sprite_array(paper_sprite, '*', 60*256, 5, sprite_frames); 
    animate_ascii_sprite(60*256, 5, sprite_frames, frame_rate, before_string, "");
    strcat(before_string, sprite_frames[4]); // Add ASCII "PAPER" to before string
    sleep(500);

    get_sprite_array(scissors_sprite, '*', 60*256, 8, sprite_frames); 
    animate_ascii_sprite(60*256, 8, sprite_frames, frame_rate, before_string, "");
    strcat(before_string, sprite_frames[2]); // Add ASCII "PAPER" to before string
    sleep(500);

    fclose(rock_sprite);
    fclose(paper_sprite);
    fclose(scissors_sprite);
}

// * Pick's the computer's hand
// * Increments wins as necessary.
// * Displays a beautiful ASCII art animation of rock, paper, scissors
// * Reveals the computer's hand
// * Displays calls either win_screen or lose_screen
// * Asks the user if they want to keep playing (exit program if not)
// NOTE: Clears screen
void shoot (char user_hand, Player comp, int *wins, int *rounds, int *ties)
{
    // Increment rounds
    (*rounds)++;

    // Select the computer's hand
    if (comp.id == 1) // player1: randomly select r, p, or s
    {
        switch (rand() % 3)
        {
            case 0:
                comp.hand = 'r';
                break;
            case 1:
                comp.hand = 'p';
                break;
            case 2:
                comp.hand = 's';
                break;
        }
    }
    else if (comp.id == 2)
    { 
        switch (rand() % 4) // paper is chosen 2/4 times = 50%
        {
            case 0:
                comp.hand = 'r';
                break;
            case 1:
                comp.hand = 'p';
                break;
            case 2:
                comp.hand = 'p';
                break;
            case 3:
                comp.hand = 's';
                break;
        }
    }    
    // If comp.id != 1 or 2, then it = 0 and is a test player where you don't change the hand

    // Animate the computer's throw
    // Open the various files
    FILE *rock_sprite = fopen("RPS_ASCII_ART/rock_hand.txt", "r");
    FILE *paper_sprite = fopen("RPS_ASCII_ART/paper_hand.txt", "r");
    FILE *scissors_sprite = fopen("RPS_ASCII_ART/scissors_hand.txt", "r");
    FILE *rock_text_sprite = fopen("RPS_ASCII_ART/rock.txt", "r");
    FILE *paper_text_sprite = fopen("RPS_ASCII_ART/paper.txt", "r");
    FILE *scissors_text_sprite = fopen("RPS_ASCII_ART/scissors.txt", "r");
    FILE *shoot_text_sprite = fopen("RPS_ASCII_ART/shoot.txt", "r");

    // Initialize various frame arrays
    char rock_frames[2][256*60];// = {{'\0'}};
    char paper_frames[1][256*60];// = {{'\0'}};
    char scissors_frames[1][256*60];// = {{'\0'}};
    char rock_text_frames[4][256*60];// = {{'\0'}};
    char paper_text_frames[5][256*60];// = {{'\0'}};
    char scissors_text_frames[8][256*60];// = {{'\0'}};
    char shoot_text_frames[1][256*60];// = {{'\0'}};

    // Get various frame arrays
    get_sprite_array(rock_sprite, '*', 256*60, 2, rock_frames);
    get_sprite_array(paper_sprite, '*', 256*60, 1, paper_frames);
    get_sprite_array(scissors_sprite, '*', 256*60, 1, scissors_frames);
    get_sprite_array(rock_text_sprite, '*', 256*60, 4, rock_text_frames);
    get_sprite_array(paper_text_sprite, '*', 256*60, 5, paper_text_frames);
    get_sprite_array(scissors_text_sprite, '*', 256*60, 8, scissors_text_frames);
    get_sprite_array(shoot_text_sprite, '*', 256*60, 1, shoot_text_frames);
    
    // Close various files
    fclose(rock_sprite);
    fclose(paper_sprite);
    fclose(scissors_sprite);
    fclose(rock_text_sprite);
    fclose(paper_text_sprite);
    fclose(scissors_text_sprite);
    fclose(shoot_text_sprite);

    // Animate throw (closed rist bobbing up and down with "Rock, Paper, Scissors, Shoot" text above)
    animate_ascii_sprite(256*60, 2, rock_frames, 2, "\n\n\n\n\n\n\n", ""); // Show no text above hand
    animate_ascii_sprite(256*60, 2, rock_frames, 2, rock_text_frames[3], ""); // Show text "ROCK" above hand
    animate_ascii_sprite(256*60, 2, rock_frames, 2, paper_text_frames[4], ""); // Show text "PAPER" above hand
    animate_ascii_sprite(256*60, 2, rock_frames, 2, scissors_text_frames[7], ""); // Show text "Scissors" above hand

    // Display the Shoot
    cls();
    switch (comp.hand)
    {
        case 'r':
            printf("%s%s", shoot_text_frames[0], rock_frames[0]);
            break;
        case 'p':
            printf("%s%s", shoot_text_frames[0], paper_frames[0]);
            break;
        case 's':
            printf("%s%s", shoot_text_frames[0], scissors_frames[0]);
            break;
        default:
            printf("ERROR in shoot function! comp.hand = '%c' (%d); comp.id = %d\n", comp.hand, comp.hand, comp.id);
    }
    sleep(1000);

    // Find out who won 
    // If tie
    if (user_hand == comp.hand)
    {
        (*ties)++;
        tie_screen(user_hand, comp, *wins, *rounds, *ties);
    }
    // If won
    else if (   (user_hand == 'r' && comp.hand == 's')
            ||  (user_hand == 'p' && comp.hand == 'r')
            ||  (user_hand == 's' && comp.hand == 'p'))
    {
        (*wins)++;
        win_screen(user_hand, comp, *wins, *rounds, *ties);
    }
    else
        loose_screen(user_hand, comp, *wins, *rounds, *ties);

}

// * Displays a the computer's and user's hand as well as how many rounds won, tied, etc.
void stats_screen(char user_hand, Player comp, int wins, int rounds, int ties)
{
    printf("Your Hand: %s\nComputer Hand: %s\n",
            (user_hand == 'r') ? "Rock" : ((user_hand == 'p') ? "Paper" : "Scissors"),
            (comp.hand == 'r') ? "Rock" : ((comp.hand == 'p') ? "Paper" : "Scissors"));
    printf("Rounds won: %d\nRounds tied: %d\nTotal Rounds: %d\n\n", wins, ties, rounds);

    // Write score to file
    /*FILE *score_file = fopen("RPS_score.txt", "w");*/
    /*fprintf(score_file, "Rounds Won: %d", wins);*/
    /*fclose(score_file);*/
}

// * Displays a "You Win" message along with the computer's hand and
// the user's hand. 
void win_screen(char user_hand, Player comp, int wins, int rounds, int ties)
{
    printf("You won!!!\n");
    stats_screen(user_hand, comp, wins, rounds, ties);
}

// * Displays a "You Lose" message along with the computer's hand and
// the user's hand. 
void loose_screen(char user_hand, Player comp, int wins, int rounds, int ties)
{
    printf("You lost!!!\n");
    stats_screen(user_hand, comp, wins, rounds, ties);
}

// * Displays a "Tie" message along with the computer's hand and
// the user's hand. 
void tie_screen(char user_hand, Player comp, int wins, int rounds, int ties)
{
    printf("You tied!!!\n");
    stats_screen(user_hand, comp, wins, rounds, ties);
}

// Prompts the user whether to quit (q) or continue (y).
// Exits if user chooses (q)
char prompt_quit ()
{
    char options[2] = { 'y', 'q' };
    char *option_descriptions[2] = { "yes", "quit" };
    char option = char_select(  "Do you want to play again?", 
                                2, options, (const char **)option_descriptions); 

    return option;
}

//  Breaks the 'sprite' file into frames based on 'separator_line'.
//  Prints before_str, followed by a new line, followed by the current frame,
//      followed by a new line and then after_str.
//  Waits for 1/frame_rate seconds and then draws the next frame.
void animate_ascii_sprite(int max_sprite_size, int num_frames, char sprite_array[num_frames][max_sprite_size],
                          int frame_rate, char *before_str, char *after_str)
{
    // Loop through each frame of animation
    for (int frame = 0; frame < num_frames; frame++)
    {
        // Clear screen
        cls();

        // Print the frame
        printf("%s%s%s", before_str, sprite_array[frame], after_str);

        // Delay for 1000 miliseconds / frame_rate
        sleep(1000 / frame_rate);
    }
}

//  Uses 'separator_line' to split a file containing ASCII sprite frames into a list of strings containing
//  each frame of animation
void get_sprite_array ( FILE *sprite, char separator_char, int max_sprite_size,
                        int num_frames, char sprite_array[num_frames][max_sprite_size])
{
    // Make sure the file is valid
    if (sprite == NULL)
        printf("ERROR in get_sprite_array: File pointer NULL\n"
                "Note: Make sure the following files are in a folder called 'RPS_ASCII_ART':\n"
                "paper.txt, paper_hand.txt, rock.txt, rock_hand.txt, scissors.txt, scissors_hand.txt, and shoot.txt\n");
    else
    {
        // Loop through each frame in the animation, adding the appropriate sprite string
        // to the array.
        bool hit_eof = false; // keep track of whether we hit EOF
        for (int f = 0; f < num_frames; f++)
        {
            char *frame  = sprite_array[f];
            
            // If we already hit the EOF, just add '\0' to the remaining frames
            if (hit_eof) 
            {
                frame[0] = '\0';
                continue;
            }

            // Loop through each character in the file, stopping when we hit EOF or separator_char
            for (int ch = 0; ch < max_sprite_size - 1; ch ++)
            {
                // Get the current character in the sprite file
                char character = fgetc(sprite);
                if (feof(sprite)) 
                {
                    hit_eof = true;
                    frame[ch] = '\0';
                    break;
                }
                
                // If we hit the separator_char, add a null character to the string,
                //  clear the '\n' following separator_char from the buffer,
                //  and break  the loop
                if (character == separator_char)
                {
                    frame[ch] = '\0';
                    fgetc(sprite);
                    break;
                }
                // Otherwise, add the character to the list
                else 
                {
                    frame[ch] = character;
                }

            }
            frame[max_sprite_size - 1] = '\0'; // Make sure it ends in a null character
        }
    }
}
