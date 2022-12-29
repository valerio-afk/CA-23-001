using System;
class Game
{
    private Frame []frames;
    private string player;
    
    public Game ()
    {
        frames = new Frame[Frame.N_FRAMES];

        for(int i = 0;i<(Frame.N_FRAMES-1);i++) this.frames[i] = new Frame(i+1);
        frames[Frame.N_FRAMES-1] = new LastFrame();

        this.player="Player";
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
                        f++;
                        r=0;
                    }
                }
                else 
                {
                    if ((r==0) && Game.is_strike(c)) 
                    {
                        this[f][0] = Frame.PINS;
                        f++;
                    }
                    else
                    {
                        if ((r==1) && Game.is_spare(c)) 
                        {
                            
                            this[f][1] = Frame.PINS - this[f][0];

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
            }
        }
    }

    public string Player
    {
        get => this.player;
        set => this.player = value;
    }

    public void calculateScore()
    {
        int [] next_rolls = new int[2]; 

        for (int i=0;i<Frame.N_FRAMES;i++)
        {
            Frame current_frame = this[i];
            Array.Fill(next_rolls,0); //similar to memset in C/C++

            if (current_frame.FrameNumber < Frame.N_FRAMES) 
            {
                next_rolls[0] = this[i+1][0];

                if (this[i+1].FrameNumber < Frame.N_FRAMES)
                    next_rolls[1] = this[i+1].isStrike() ? this[i+2][0] : this[i+1][1];
                else next_rolls[1] = this[i+1][1];                
            }
            
            if (current_frame.FrameNumber > 1) current_frame.calculateScore(this[i-1],next_rolls);
            else current_frame.calculateScore(next_rolls);
        }
    }

    public void printGame()
    {
        string frame_line1 = "";
        string frame_line2 = "";
        

        foreach (Frame current_frame in this.frames)
        {
            frame_line1 += current_frame;
            frame_line1 += "|";

            if (current_frame.FrameNumber == Frame.N_FRAMES) frame_line2+= String.Format("{0,5}",current_frame.Score);
            else frame_line2+= String.Format("{0,3}",current_frame.Score);

            frame_line2 += "|";

        }

        //Explicit call to ToString is not necessary. The runtime does this for us. Sometimes it also work in Java
        string frames = frame_line1;//.ToString(); 
        string dashed_line = new string('-',frames.Length+1);
        string player_line = this.Player;

        

        Console.WriteLine(dashed_line);
        
        int l_padding = (frames.Length / 2) - (this.Player.Length / 2);
        int r_padding = frames.Length - this.Player.Length - l_padding-1;
       

        Console.WriteLine("|"+ new string(' ',l_padding) +player_line+ new string(' ',r_padding) +"|");
        
        
        //print the top row of the scoring board - just the number of frames
        for(int i=0;i<Frame.N_FRAMES;i++) Console.Write("| "+(i+1)+" ");
        Console.WriteLine(" |");
        
        
        Console.WriteLine(dashed_line);
        Console.WriteLine("|"+frames);


        Console.WriteLine(dashed_line);
        Console.WriteLine("|"+frame_line2);
        
        Console.WriteLine(dashed_line);
    }

    public static bool is_strike(char c) {return ((c == Frame.STRIKE_SYMBOL) || (c==Frame.STRIKE_SYMBOL_LC)); }
    public static bool is_spare(char c)  {return (c == Frame.SPARE_SYMBOL); }
    public static bool is_miss(char c)   {return (c == Frame.NO_POINTS_SYMBOL); }
    public static bool is_number(char c) {return (c>='0') && (c<='9'); }
};