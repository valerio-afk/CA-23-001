using System;
using System.Collections;

class Game : IEnumerable, IEnumerator
{
    private Frame []frames;
    private string player;
    private int enum_index;

    IEnumerator IEnumerable.GetEnumerator(){
        this.ResetIterator();
        return this;
    }

    bool IEnumerator.MoveNext()
    {
        enum_index++;

        return enum_index < Frame.N_FRAMES;
    }

    void IEnumerator.Reset()
    {
        this.ResetIterator();
    }

    object IEnumerator.Current
    {
        get => this.frames[this.enum_index];
    }

    protected void ResetIterator()
    {
        enum_index = -1;
    }

    public Game ()
    {
        frames = new Frame[Frame.N_FRAMES];

        for(int i = 0;i<(Frame.N_FRAMES-1);i++) this.frames[i] = new Frame(i+1);
        frames[Frame.N_FRAMES-1] = new LastFrame();

        this.player="Player";
        this.ResetIterator();
    }

    public Game (string sequence) : this() //similar syntax as in C++
    {
        this.readSequence(sequence);
        this.calculateScore();
    }

    public Game (string sequence,string player) : this(sequence)
    {
        this.player = player;
    }

    public Frame this[int idx]
    {
        get => this.frames[idx];
        private set => this.frames[idx] = value;
    }

    protected void readSequence(string sequence)
    {
        int i = 0; 
        int f = 0; 
        int r = 0;

        foreach (char c in sequence) 
        {
            if (f < (Frame.N_FRAMES-1)) 
            {
                if (Game.is_number(c) || Game.is_miss(c)) 
                {
                    if (Game.is_miss(c)) this[f][r] = 0;
                    else this[f][r] = c - '0';

                    r++;
                    if (r>1)
                    {
                        this[f].isPlayed = true;
                        f++;
                        r=0;
                    }
                }
                else 
                {
                    if ((r==0) && Game.is_strike(c)) 
                    {
                        this[f][0] = Frame.PINS;
                        this[f].isPlayed = true;
                        f++;
                    }
                    else
                    {
                        if ((r==1) && Game.is_spare(c)) 
                        {
                            
                            this[f][1] = Frame.PINS - this[f][0];
                            this[f].isPlayed = true;

                            f++; 
                            r=0;
                        }
                        else throw new Exception(String.Format("I don't undernstanad character {} at position {}",c,i+1));
                    }
                }
            }
            else 
            {
                if (Game.is_number(c) || Game.is_miss(c)) 
                {
                    if (Game.is_miss(c)) this[f][r] = 0;
                    else this[f][r] = c - '0';
                } 
                else if(Game.is_strike(c)) this[f][r] = Frame.PINS;
                else
                {

                    if ((r>0) && (is_spare(c))) this[f][r] = Frame.PINS - this[f][r-1];
                    else throw new Exception(String.Format("I don't undernstanad character {} at position {}",c,i+1));
                    
                }

                r++;

                if ((this[f][0]==Frame.PINS)&& (r==3)) this[f].isPlayed = true;
                else if( ((this[f][0]+this[f][1])==Frame.PINS) && (r==3)) this[f].isPlayed = true;
                else if (r==2) this[f].isPlayed = true;
            }
        }
    }

    public string Player
    {
        get => this.player;
        set => this.player = value;
    }

    protected int[]? getNextRolls(int idx)
    {
        if (!this[idx].isPlayed || this[idx].isOpen() || (idx==(Frame.N_FRAMES-1)) || (!this[idx+1].isPlayed) ) return null;
        else{
            int [] next_rolls = {0,0};
            next_rolls[0] = this[idx+1][0];

            if (this[idx].isStrike()) 
            {
                if (this[idx+1].isStrike() && (this[idx+1].FrameNumber<Frame.N_FRAMES))
                {
                    if (this[idx+2].isPlayed) next_rolls[1] = this[idx+2][0];
                    else return null;
                }
                else next_rolls[1] = this[idx+1][1];
            }
            return next_rolls;
        }
    }

    public void calculateScore()
    {
        int[]? next_rolls = null;

        for (int i=0;i<Frame.N_FRAMES;i++)
        {
            Frame current_frame = this[i];
            next_rolls = this.getNextRolls(i);
            
            if (current_frame.FrameNumber > 1) current_frame.calculateScore(this[i-1],next_rolls);
            else current_frame.calculateScore(next_rolls);
        }
    }

    public override string ToString()
    {
        string final_string = "";

        string frame_line1 = "";
        string frame_line2 = "";
        

        foreach (Frame current_frame in this)
        {
            frame_line1 += current_frame;
            frame_line1 += "|";

            if (current_frame.isPlayed && (current_frame.isOpen() || (current_frame.FrameNumber == Frame.N_FRAMES) || (this.getNextRolls(current_frame.FrameNumber-1)!=null)))
            {
                if (current_frame.FrameNumber == Frame.N_FRAMES) frame_line2+= String.Format("{0,5}",current_frame.Score);
                else frame_line2+= String.Format("{0,3}",current_frame.Score);
                
            }
            else frame_line2+=new string(' ',current_frame.FrameNumber == Frame.N_FRAMES?5:3);

            frame_line2 += "|";

        }

        //Explicit call to ToString is not necessary. The runtime does this for us. Sometimes it also work in Java
        string frames = frame_line1;//.ToString(); 
        string dashed_line = "+" + new string('-',frames.Length-1)+"+\n";
        string player_line = this.Player;

        

        final_string += dashed_line;
        
        int l_padding = (frames.Length / 2) - (this.Player.Length / 2);
        int r_padding = frames.Length - this.Player.Length - l_padding-1;
       

        final_string += "|"+ new string(' ',l_padding) +player_line+ new string(' ',r_padding) +"|\n";
        
        
        //print the top row of the scoring board - just the number of frames
        for(int i=0;i<Frame.N_FRAMES;i++) final_string +="| "+(i+1)+" ";
        final_string += " |\n";

        
        
        
        final_string += dashed_line;
        final_string += "|"+frames+"\n";


        final_string += dashed_line;
        final_string += "|"+frame_line2+"\n";
        
        final_string += dashed_line;


        return final_string;
    }

    public void PrintGame()
    {
        Console.Write(this);
    }

    public static bool is_strike(char c) {return ((c == Frame.STRIKE_SYMBOL) || (c==Frame.STRIKE_SYMBOL_LC)); }
    public static bool is_spare(char c)  {return (c == Frame.SPARE_SYMBOL); }
    public static bool is_miss(char c)   {return (c == Frame.NO_POINTS_SYMBOL); }
    public static bool is_number(char c) {return (c>='0') && (c<='9'); }
};