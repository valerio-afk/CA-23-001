class Frame
{
    protected int frameNumber;
    protected int []rolls;
    private int score;
    private bool played;

    // Differently than java, constants don't require to be explictly static. they already are
    public const int PINS = 10;
    public const int N_FRAMES = 10;
    public const char STRIKE_SYMBOL = 'X';
    public const char STRIKE_SYMBOL_LC = 'x';
    public const char SPARE_SYMBOL = '/';
    public const char NO_POINTS_SYMBOL = '-';

    public Frame()
    {
        this.frameNumber = 0;
        this.score = 0;
        this.rolls = new int[2];
        this.played=false;
    }

    public bool isPlayed
    {
        get => this.played;
        set => this.played = true;
    }

    public Frame(int n) : this() //similar syntax as in C++
    {
        this.frameNumber = n;
    }

    //This is a particular method
    //It permits to create indexers (also C++ allows this)
    //Instances of Frame class can be used like arrays
    //This allows me to access rolls in a better way, without calling .getRoll() method as seen before
    public int this[int idx]
    {
        get{return this.rolls[idx];}
        set{this.rolls[idx] = value;}
    }

    //Java doesn't allow for getter/setter. They are normal methods
    //C# permits the creation of properties with getter/setter methods
    //Properties defined in this way can be access as they were public properties
    //e.g. my_frame.Score
    //get and set keyworks allow to define what to do in case of get (example above) and set (my_frame.Score = 10;)
    public int Score
    {
        get {return this.score;}
        set {this.score = value;}
    }

    public int FrameNumber
    {
        //when getter/setter are one line, I can the => operator for inline (or better, lambda) functions
        get => this.frameNumber;
        private set => this.frameNumber = value;
    }

    public bool isStrike() {return this[0] == PINS;}
    public bool isSpare() {return !this.isStrike() && ((this[0]+this[1])==PINS);}
    public bool isOpen() {return ((this[0]+this[1])<PINS);}

    //Differently than java, methods that can be overridden need to be specified virtual
    //We saw something similar to this in C++
    public virtual void calculateScore(int []?next_rolls)
    {
        int frame_score = 0;

        frame_score += this[0] + this[1]; 

        if (next_rolls!=null)
        {
            if (this.isStrike()) frame_score += next_rolls[0] + next_rolls[1]; 
            else if (this.isSpare()) frame_score += next_rolls[0]; 
        }

        this.Score = frame_score;
    }

    public virtual void calculateScore(Frame previousFrame, int []?next_rolls)
    {
        this.calculateScore(next_rolls);
        this.Score += previousFrame.Score;
    }

    public override string ToString()
    {
        if (this.isPlayed) return String.Format("{0} {1}",this.printRoll(0), this.printRoll(1));
        else return "   ";
    }

    protected virtual char printRoll(int idx)
    {
        if ((idx != 1) && (this[idx] == PINS)) return STRIKE_SYMBOL;
        //if it is required to get the character of the 2nd roll and we have a strike, we just print an empty space
        else if ((idx==1) && (this[idx-1] ==PINS) ) return ' ';
        //if we are not in the first roll and the sum of two neighbour rolls is 10, then it's spare
        else if ((idx>0) && ( (this[idx-1] + this[idx]) ==PINS) ) return SPARE_SYMBOL;
        //if we didn't knock out any pins, we didn't score!
        else if (this[idx] == 0) return NO_POINTS_SYMBOL;
        //all the other cases are numbers in 1-9 -> convert it to the right ASCII digit
        else return (char) (this[idx] + '0');
    }

}