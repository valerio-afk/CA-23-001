/* LIBRARIES */
#include <iostream> // Includes all the printf functions and derivatives 
#include <iomanip> // allows to format streams with manupulation (new c++ crap)
#include <sstream> //this is to have some cool stuff about strings
#include <string> // Functions to deal with strings
#include <vector> //vectors are more flexibles than arrays
#include <stdexcept> //let's use the exceptions :)
#include <string.h> //I need memset from old C

/* C++ crap*/

using namespace std;

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

/* CUSTOMISED TYPES */

typedef unsigned char frame_t;
typedef unsigned short int score_t;

/* CLASSES */

class Frame
{
    protected:
        frame_t n;
        vector<frame_t> rolls;
        score_t score;
    
    public:
        Frame()
        {
            this->n = 0;
            this->score = 0;

            this->rolls = vector<frame_t>(2);
        }

        Frame (frame_t n) : Frame()
        {
            this->n = n;
        }

        void setRoll(frame_t idx, frame_t value)
        {
            this->rolls[idx] = value;
        }

        frame_t getRoll(frame_t idx) { return this->rolls[idx]; }
        frame_t getFrameNumber() { return this->n; }

        score_t getScore()
        {
            return this->score;
        }

        bool isStrike() 
        {
            return this->rolls[0]==PINS;
        }

        bool isSpare() 
        {
            return !this->isStrike() && ((this->rolls[0]+this->rolls[1])==PINS);
        }

        virtual void calculateScore(frame_t *next_rolls)
        {
            frame_t frame_score = this->rolls[0] + this->rolls[1];

            if (this->isStrike()) frame_score += next_rolls[0] + next_rolls[1]; 
            else if (this->isSpare()) frame_score += next_rolls[0]; 

            this->score = frame_score; 
        }


        virtual void calculateScore(Frame *previousFrame, frame_t *next_rolls)
        {
            this->calculateScore(next_rolls);
            this->score += previousFrame->getScore();
        }

        virtual string toString()
        {
            ostringstream buffer;
            buffer << this->printRoll(0)<< " "<< this->printRoll(1);

            return buffer.str();
        }

    protected:
        virtual char printRoll(frame_t idx)
        {
            if ((idx != 1) && (this->getRoll(idx) == PINS)) return STRIKE_SYMBOL;
            //if it is required to get the character of the 2nd roll and we have a strike, we just print an empty space
            else if ((idx==1) && (this->getRoll(idx-1) ==PINS) ) return ' ';
            //if we are not in the first roll and the sum of two neighbour rolls is 10, then it's spare
            else if ((idx>0) && ( (this->getRoll(idx-1) + this->getRoll(idx)) ==PINS) ) return SPARE_SYMBOL;
            //if we didn't knock out any pins, we didn't score!
            else if (this->getRoll(idx) == 0) return NO_POINTS_SYMBOL;
            //all the other cases are numbers in 1-9 -> convert it to the right ASCII digit
            else return this->getRoll(idx) + ASCII_NUM_IDX;
        }
};

class LastFrame : public Frame
{
    public:
        LastFrame () : Frame()
        {
            this->n = N_FRAMES;

            this->rolls = vector<frame_t>(3);
        }

        virtual void calculateScore(Frame *previousFrame, frame_t *)
        {
            this->calculateScore(previousFrame);
        }

        virtual void calculateScore(Frame *previousFrame)
        {
            frame_t frame_score = previousFrame->getScore();

            frame_score += this->getRoll(0) + this->getRoll(1); 

            if (this->isStrike() || this->isSpare()) frame_score += this->getRoll(2);


            this->score = frame_score; 
        }

        virtual string toString()
        {
            ostringstream buffer;
            if (this->isSpare())
            {
                buffer<<Frame::toString();
                buffer<<" "<<this->printRoll(2);
            }
            else if (this->isStrike())
            {
                buffer<<this->printRoll(0)<<" ";
                buffer<<this->printRoll(1)<<" ";
                buffer<<Frame::printRoll(2);
            }
            else buffer << Frame::toString()<<"  ";

            return buffer.str();
        }

    protected:
        virtual char printRoll(frame_t idx)
        {
            if (this->getRoll(idx) == 0) return NO_POINTS_SYMBOL;
            else if (this->getRoll(idx) == PINS) return STRIKE_SYMBOL;
            else return this->getRoll(idx) + ASCII_NUM_IDX;
        }
};

class Game
{
    private:
        Frame *frames[N_FRAMES];
        string player;
    
    public:
        Game();
        Game (string);
        Game (string,string);

        Frame * getFrame(size_t);
        void readSequence(string);
        void calculateScore();
        void printGame();
};

Game::Game ()
{
    for(int i = 0;i<(N_FRAMES-1);i++) this->frames[i] = new Frame(i+1);
    this->frames[N_FRAMES-1] = new LastFrame();

    this->player="Player";
}

Game::Game (string sequence) : Game()
{

    this->readSequence(sequence);
    this->calculateScore();
}

Game::Game (string sequence,string player) : Game(sequence)
{
    this->player = player;
}

Frame* Game::getFrame(size_t idx)
{
    return this->frames[idx];
}

void Game::readSequence(string sequence)
{
    int len = sequence.length();
    int i = 0; 
    int f = 0; 
    int r = 0;

    for (i = 0; i<len;i++) 
    {
        if (f < (N_FRAMES-1)) 
        {
            if (is_number(sequence[i]) || is_miss(sequence[i])) 
            {
                if (is_miss(sequence[i])) this->getFrame(f)->setRoll(r,0);
                else this->getFrame(f)->setRoll(r, sequence [i] - ASCII_NUM_IDX);

                r++;
                if (r>1)
                {
                    f++;
                    r=0;
                }
            }
            else 
            {
                if ((r==0) && is_strike(sequence[i])) 
                {
                    this->getFrame(f)->setRoll(0,PINS);
                    f++;
                }
                else
                {
                    if ((r==1) && is_spare(sequence[i])) 
                    {
                        
                        this->getFrame(f)->setRoll(1, PINS - this->getFrame(f)->getRoll(0));

                        f++; 
                        r=0;
                    }
                    else 
                    {
                        ostringstream error_message;
                        
                        error_message << "I don't understand character " << sequence[i] << " at position " <<i+1;
                        throw runtime_error(error_message.str());
                    }
                }
            }
        }
        else 
        {
            if (is_number(sequence[i]) || is_miss(sequence[i])) 
            {
                if (is_miss(sequence[i])) this->getFrame(f)->setRoll(r,0);
                else this->getFrame(f)->setRoll(r, sequence [i] - ASCII_NUM_IDX);
            } 
            else if(is_strike(sequence[i])) this->getFrame(f)->setRoll(r,PINS);
            else
            {

                if ((r>0) && (is_spare(sequence[i]))) this->getFrame(f)->setRoll(r, PINS - this->getFrame(f)->getRoll(r-1));
                else
                {
                    ostringstream error_message;

                    error_message << "I don't understand character " << sequence[i] << " at position " <<i+1;
                    throw runtime_error(error_message.str());
                }
            }

            r++; 
        }
    }
}

void Game::calculateScore()
{
    frame_t next_rolls[2]; 

    for (int i=0;i<N_FRAMES;i++)
    {
        Frame * current_frame = this->getFrame(i);
        memset(next_rolls,0, sizeof(next_rolls)); 

        if (current_frame->getFrameNumber() < N_FRAMES) 
        {
            next_rolls[0] = this->getFrame(i+1)->getRoll(0);

            if (this->getFrame(i+1)->getFrameNumber() < N_FRAMES)
                next_rolls[1] = this->getFrame(i+1)->isStrike() ? 
                                    this->getFrame(i+2)->getRoll(0) : 
                                    this->getFrame(i+1)->getRoll(1);
            else next_rolls[1] = this->getFrame(i+1)->getRoll(1);
        }

        if (current_frame->getFrameNumber() > 1) current_frame->calculateScore(this->getFrame(i-1),next_rolls);
        else current_frame->calculateScore(next_rolls);
    }
}

void Game::printGame()
{
    ostringstream frame_line1;
    ostringstream frame_line2;
    

    for (Frame *current_frame : this->frames)
    {
        //if (current_frame->getFrameNumber()<N_FRAMES) frame_line1<<current_frame->toString();
        //else frame_line1<<static_cast<LastFrame*>(current_frame)->toString();
        frame_line1<<current_frame->toString()<<"|";

        //print 2nd line with totals
        //the only difference in case it is the last frame is the number of paddind characters used
        //not a big deal but I like nice ASCII graphics
        if (current_frame->getFrameNumber() == N_FRAMES) frame_line2<<setw(5);
        else frame_line2<<setw(3);

        frame_line2<<current_frame->getScore()<<"|";

    }

    string frames = frame_line1.str();

    string dashed_line = string(frames.length()+1,'-');

    cout<<dashed_line<<endl;

    int l_padding = (frames.length() / 2) - (this->player.length() / 2);
    int r_padding = frames.length() - this->player.length() - l_padding-1;

    cout<<"|"; 
    for (int i=0;i<l_padding;i++) cout<<" ";

    cout<<this->player;

    for (int i=0;i<r_padding;i++) cout<<" ";
    cout<<"|"<<endl;
    
    
    //print the top row of the scoring board - just the number of frames
    for(int i=0;i<N_FRAMES;i++) cout<<"| "<<i+1<<" ";
    cout<<" |"<<endl;
    
    cout<<dashed_line<<endl;
    cout<<"|"<<frames<<endl;
    

    cout<<dashed_line<<endl;
    cout<<"|"<<frame_line2.str()<<endl;
    
    cout<<dashed_line<<endl;
}


/*
This is the entry point of our program
*/
int main()
{
    string my_game = "9-XX9-7/XX8-6/XX9"; //this is a real game I have scored a few months ago
    Game valerio = Game(my_game,"Valerio"); //I initialise my game

    valerio.printGame();

    return 0; //we tell the OS that the program has finished successfully
}

