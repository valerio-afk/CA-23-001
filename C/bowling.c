/* LIBRARIES */
#include <stdio.h> // Includes all the printf functions and derivatives 
#include <stdbool.h> // This includes (kind of) a new type for boolean
#include <string.h> // Functions to deal with strings

/* SOME CONSTATS */

//This is indeed 10-pin bowling :)
#define PINS 10
//Max number of rolls
#define N_ROLLS 3

//Max number of frames
#define N_FRAMES 10

//Max number of characters (this is needed to print the game on screen)
#define BUFFER 256

//The character '0' is located a position 48 in ASCII encoding
#define ASCII_NUM_IDX 48

#define STRIKE_SYMBOL 'X'
#define STRIKE_SYMBOL_LC 'x'
#define SPARE_SYMBOL '/'
#define NO_POINTS_SYMBOL '-'

/* SOME NICE MACROS */
#define is_strike(c) (((c) == STRIKE_SYMBOL) || ((c)==STRIKE_SYMBOL_LC))
#define is_spare(c)  ((c) == SPARE_SYMBOL)
#define is_miss(c)   ((c) == NO_POINTS_SYMBOL)
#define is_number(c) ((c)>=ASCII_NUM_IDX) && ((c)<(ASCII_NUM_IDX+10))

#define has_scored_a_strike(f) ((f).rolls[0]==PINS)
#define has_scored_a_spare(f)  ((!has_scored_a_strike(f)) && (((f).rolls[0]+(f).rolls[1])==PINS))

#define print_line(len) for(int i=0;i<=(len);i++) printf("-"); printf("\n");


/* FUNCTION DECLARATIONS (I just do one to show how it's done) */
void strreplacechar(char *,char,char); //function prototype

/* DATA STRUCTURES */

//This struct identifies a frame
struct frame
{
    unsigned short int frame_number; // 1-10
    unsigned char rolls[N_ROLLS];    //here I put the information regarding the rolls
    unsigned short int score;        //Score up to this frame
};

//A game is just a set of 10 frames! I could've added the name, but who cares!
struct game
{
    struct frame frames[N_FRAMES];
};

/* FUNCTION DEFINITIONS */

/* Function start_game 
This function returns a new game initialised with 0s.
Frame numbers are set.

Takes: Nothing
Returns: struct game
*/

struct game start_game()
{
    struct game new_game = {0}; // this initialised the whole struct with 0s

    for (int i = 0; i<N_FRAMES; i++) new_game.frames[i].frame_number = i+1; //set at least the frame number
    
    
    return new_game; //returns the initialised game
}


/* Function set_scores 

This function takes a sequence of characters identifying a bowling game and
changes the content in the provided pointer to struct with appropriate scores
Scores are at frame level, not a game level.

Takes: 
- Pointer to struct game: the game we want to set the score. This pointer will be changed when this function is called
- String: the string is a sequence of characters that represent a bowling game

Returns: Nothing
*/
void set_scores(struct game *g, const char * sequence)
{
    int len = strlen(sequence);
    int i = 0; // string position
    int f = 0; // frame position
    int r = 0; // roll position

    for (i = 0; i<len;i++) //iterate for each character in the sequence
    {
        if (f < (N_FRAMES-1)) //let's distinguish two cases: last or non-last frame
        { //in case it's NOT the last frame
            if (is_number(sequence[i]) || is_miss(sequence[i])) //check if it is a number (1-9) or a miss (basically 0 which is a dash char)
            {
                if (is_miss(sequence[i])) g->frames[f].rolls[r] = 0; 
                else g->frames[f].rolls[r] = sequence [i] - ASCII_NUM_IDX; //this converts char digit in integer digit

                r++; //the roll number is increase. HOWEVER...
                if (r>1) //...if it gets more than 2 (remember it's zero-based!)
                {
                    f++; //...go to the next frame ...
                    r=0; //...and reset the roll number
                }
            }
            else //In this case, I check if there's a strike or a spare
            {
                if ((r==0) && is_strike(sequence[i])) //distinguish the case there's a strike on the first roll
                {
                    g->frames[f].rolls[0] = PINS; //a strike is 10 points!
                    f++; //and let's go to the next frame directly!
                }
                else
                {
                    if ((r==1) && is_spare(sequence[i])) //there's a spare
                    {
                        //the spare is represented by a slash symbol /
                        //As there are only 10 pins, to get the number of pins knocked out in this case
                        //it is enough to perform the following operation (yep, the one you see below!)
                        g->frames[f].rolls[1] = PINS - g->frames[f].rolls[0];

                        f++; //let's go to the next frame
                        r=0; //and let's also reset the number of rolls
                    }
                    else //someone is trying to cheat :P
                    {
                        printf("I don't understand character %c at position %i",sequence[i],i+1);
                        return;
                    }
                }
            }
        }
        else //last frame - problems here. We need to distinguish a few cases!
        {
            //some of the stuff here is similar as above
            //comments will cover new stuff!
            if (is_number(sequence[i]) || is_miss(sequence[i])) 
            {
                if (is_miss(sequence[i])) g->frames[f].rolls[r] = 0;
                else g->frames[f].rolls[r] = sequence [i] - ASCII_NUM_IDX;
            } 
            else if(is_strike(sequence[i])) g->frames[f].rolls[r] = PINS;
            else
            {
                //we can have a spare on the 2nd or 3rd roll
                //a few examples below
                // 9/X or X2/ 
                //I am aware some bugs might happen here - not really important

                if ((r>0)&&(is_spare(sequence[i])))   g->frames[f].rolls[r] = PINS - g->frames[f].rolls[r-1];
                else
                {
                    printf("I don't understand character %c at position %i",sequence[i],i+1);
                    return;
                }
            }

            r++; //lets go to the next roll!
        }
    }
}


/* Function calculate_score_frame 

Calculates the score of the current frame, summing it up with the score from the previous frame (if any)

Takes: 
- Pointer to struct frame: the frame you want to calculate the score
- Array to rolls: values for the next two rolls, whichever they are. Please make sure this is a pointer to an array with 2 elements
- previous score: score from previous frame. Set to 0 if you are calculating the score for the first frame

Returns: Nothing
*/
void calculate_score_frame(struct frame*frame, unsigned char *next_rolls, unsigned short int prev_score)
{
    unsigned short int frame_score = prev_score; //the score for this frame starts from the previous frame score

    frame_score += frame->rolls[0] + frame->rolls[1]; //let's add up the rolls for this frame...
    if (frame->frame_number == N_FRAMES) frame_score += frame->rolls[2]; //..and, if this is the last frame, let's add the 3rd roll (if any)

    if (has_scored_a_strike(*frame)) frame_score += next_rolls[0] + next_rolls[1]; //if strike, adds up the next two rolls
    else if (has_scored_a_spare(*frame)) frame_score += next_rolls[0]; //if spare, adds up only the next roll

    frame->score = frame_score; //set the score to the frame
}


/* Function calculate_score 

Calculates the score of a game

Takes: 
- Pointer to struct game: the game you want to calculate the score

Returns: Nothing
*/
void calculate_score(struct game *g)
{
    struct frame *current_frame; //pointer to the current game (useful)
    unsigned char next_rolls[2]; //array containing the values for the next two rolls
    unsigned short int prev_score = 0; //useful to contain the score up to the previous frame

    for (int i=0;i<N_FRAMES;i++)
    {
        memset(next_rolls,0, sizeof(next_rolls)); //sets everything in next_rolls to 0 (always good to initialise/clean up stuff)

        current_frame = &g->frames[i]; //gets the address to the i-th frame
        if (current_frame->frame_number < N_FRAMES) //if it is not the last frame, next_rolls are filled with actual values
        {
            next_rolls[0] = g->frames[i+1].rolls[0]; //the value of the first next roll is always the first roll for the next frame
            //but the second next roll it is not necessarily the 2nd roll for the next frame.
            //if you have another strike, the 2nd next roll is in the next next frame

            if (g->frames[i+1].frame_number < N_FRAMES)
                next_rolls[1] = has_scored_a_strike(g->frames[i+1]) ? g->frames[i+2].rolls[0] : g->frames[i+1].rolls[1];
            else next_rolls[1] = g->frames[i+1].rolls[1];
        }

        //gets the previous score (if any)
        if (current_frame->frame_number > 1) prev_score = g->frames[i-1].score;

        //now that all the information are collected, call the function above to calculat the proper score :)
        calculate_score_frame(current_frame, next_rolls, prev_score);
    }
}


/* Function print_roll 

Returns the correct character to represent a roll.
To do this effectively, it requires all the rolls in a frame.

For example, a frame made of 7 3 should be printed as 7 /
To effectively represent a spare, this function needs neighbour information too

Takes: 
- Pointer to rolls: all the rolls in a frame (of length either 2 or 3)
- Index: index of the roll to get the correct character (ie, the index inside roll array)
- last frame: whether to consider the provided rolls as in the last frame. In this case, each roll is treated individually

Returns: a character representing the correct symbol for a bowling game (1-9 X / -)
*/

char print_roll(unsigned char *rolls,unsigned int idx, bool last_frame)
{
    if (last_frame == false) //if it is NOT the last frame ...
    {   //...strikes can occur only for the first roll (ie when it is not in the 2nd roll)
        if ((idx != 1) && (rolls[idx] == PINS)) return STRIKE_SYMBOL;
        //if it is required to get the character of the 2nd roll and we have a strike, we just print an empty space
        else if ((idx==1) && (rolls[idx-1]==PINS) ) return ' ';
        //if we are not in the first roll and the sum of two neighbour rolls is 10, then it's spare
        else if ((idx>0) && (rolls[idx-1]+rolls[idx]==PINS) ) return SPARE_SYMBOL;
        //if we didn't knock out any pins, we didn't score!
        else if (rolls[idx] == 0) return NO_POINTS_SYMBOL;
        //all the other cases are numbers in 1-9 -> convert it to the right ASCII digit
        else return rolls[idx] + ASCII_NUM_IDX;
    }
    else //if we consider this as last frame, then we have similar stuff as above, but more simplified
    {
        if (rolls[idx] == 0) return NO_POINTS_SYMBOL;
        else if (rolls[idx] == PINS) return STRIKE_SYMBOL;
        else return rolls[idx] + ASCII_NUM_IDX;
    }

    //the function never reaches this point - return here is not required 
}


/* Function print_frame 

This function returns a string representation of a frame and it returns the generated
content i the provided pointer to string.
This function only works for all the frame except the last one

Takes: 
- Pointer to frame: the frame we want to print
- Pointer to the buffer: it will contain the result of this function
- buffer_length: maximum length of the buffer to avoid segmentation fault

Returns: Nothing
*/
void print_frame(struct frame *f, char*buffer, size_t buffer_length)
{
    //snprintf is a safer version of sprintf that avoids segmentation faults
    //here indivudal rolls are printed taking advantage of the print_roll function above
    snprintf(buffer, buffer_length,"%c %c",print_roll(f->rolls,0,false),print_roll(f->rolls,1,false));
}


/* Function print_game 

Prints on screen a game with a nice ASCII table

Takes: 
- Pointer to a game struct

Returns: Nothing
*/
void print_game(struct game*g)
{
    char frame_line1[BUFFER]; //the first line is meant for the rolls of each frame
    char frame_line2[BUFFER]; //the second line is meant of the total score up to a frame
    char str[10], str2[10]; //these are helper buffer used to make frames for the first line.
    struct frame * current_frame; //useful pointer to the current frame that is set at each iteration of the for-loop below

    //firstly, the bytes in the line buffers are set to 0
    memset(frame_line1,0,sizeof(frame_line1)); 
    memset(frame_line2,0,sizeof(frame_line2));

    for (int i=0;i<N_FRAMES;i++)
    {
        current_frame = &g->frames[i]; //gets the address of the current frame

        //if we are not in the last frame, the frame is simply printed using the function above
        if (current_frame->frame_number < N_FRAMES) print_frame(current_frame,str,sizeof(str)); 
        else //issues happen with the last frame
        {
            /*
            We need to consider three different scenarios:

            Scenario 1 - A spare: A spare gives an extra roll with a total of 3 rolls
            Scenario 2 - A strke: A strike gives two extra rolls
            Scenario 3 - None of the above: in this case, the last frame will have only two rolls as for any other frames
            */
            if (has_scored_a_spare(*current_frame)) //one extra roll
            {
                print_frame(current_frame,str,sizeof(str)); //the first two rolls are printed as normally done for other frames
                sprintf(str2," %c",print_roll(current_frame->rolls,2,true)); //the extra roll is printed setting the last_frame param to TRUE

                strcat(str,str2); //concatenates the two results in and puts the result into str
            }
            else if(current_frame->rolls[0] == PINS) //two extra rolls
            {
                sprintf(str,"%c %c %c",print_roll(current_frame->rolls,0,true), // the first roll is treated individually
                                       print_roll(current_frame->rolls,1,true), // same for the second roll (we cannot have a spare bc it's treated above)
                                       print_roll(current_frame->rolls,2,false)); //the 3rd roll can be a spare and we need to treat it as it was a normal frame


            }
            else print_frame(current_frame,str,sizeof(str)); //only two rolls -> print it as a normal frame
        }

        sprintf(str2,"|"); //adds a vertical bar between two frames for graphical reasons
        strcat(str,str2); //concatentas the vertical bar to what was generated above
        strcat(frame_line1,str); //concatenates everything in the first line buffer


        //print 2nd line with totals
        //the only difference in case it is the last frame is the number of paddind characters used
        //not a big deal but I like nice ASCII graphics
        if (current_frame->frame_number == N_FRAMES) snprintf(str,sizeof(str),"%5d|",current_frame->score);
        else snprintf(str,sizeof(str),"%3d|",current_frame->score);

        //concatenates everything in the second line buffer and repeat
        strcat(frame_line2,str);
    }

    //at this point, no scores are printed as 0. All 0s are converted with a dash as per bowling scoring standards
    strreplacechar(frame_line1,'0',NO_POINTS_SYMBOL);

    //print a long line with as many dashes as the length of the frame_line1 array
    print_line(strlen(frame_line1));
    
    //print the top row of the scoring board - just the number of frames
    for(int i=0;i<N_FRAMES;i++) printf("| %d ",i+1);
    printf(" |\n");
    
    print_line(strlen(frame_line1)); //another long line of dashes
    printf("|%s\n",frame_line1); //print the scores (add a vertical bar for style)

    print_line(strlen(frame_line1)); //another long line of dashes 
    printf("|%s\n",frame_line2); //print the scores below each frame (and if I have done my calculations correct, length of the two lines should be the same)
    print_line(strlen(frame_line1));   //another long line of dashes 
}


/*
This is the entry point of our program
*/
int main()
{
    const char * my_game = "9-XX9-7/XX8-6/XX9"; //this is a real game I have scored a few months ago
    struct game valerio = start_game(); //I initialise my game

    set_scores(&valerio, my_game); //parses the string into the game struct
    calculate_score(&valerio); //calculates the scores of my game
    print_game(&valerio); //boom - I print my scoring board :)

    return 0; //we tell the OS that the program has finished successfully
}

/* Function strreplacechar 

Replaces a character with another one in a string.
Changes on the string are performed in place

Takes: 
- Pointer to a string
- character to be replaced
- new character

Returns: Nothing
*/
void strreplacechar(char *str,char c1,char c2)
{
    //this for-loop is a bit unconventional
    //It is assumed that the last character is \0 and it stops then
    for(int i=0;str[i];i++) if(str[i]==c1) str[i]=c2; //if c1 is found, it's replaced with c2 
}