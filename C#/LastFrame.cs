class LastFrame : Frame
{
    public LastFrame () 
    {
        this.frameNumber = Frame.N_FRAMES;
        this.rolls = new int[3];
    }

    public override void calculateScore(Frame previousFrame, int []next_rolls)
    {
        this.calculateScore(previousFrame);
    }

    public void calculateScore(Frame previousFrame)
    {
        int frame_score = previousFrame.Score;

        frame_score += this[0] + this[1]; 

        if (this.isStrike() || this.isSpare()) frame_score += this[2];


        this.Score = frame_score;
    }

    public override string ToString()
    {
        string  buffer = "";
        
        if (!this.isPlayed) return "     ";

        if (this.isSpare())
        {
            buffer+=base.ToString(); //here ToString must be called explicitily because base seems to be seen as a keyword rather than a regular object.
            buffer+=" ";
            buffer+=this.printRoll(2);
        }
        else if (this.isStrike())
        {
            buffer+=this.printRoll(0);
            buffer+=" ";
            buffer+=this.printRoll(1);
            buffer+=" ";
            buffer+=base.printRoll(2);
        }
        else
        {
            buffer+=base.ToString(); 
            buffer+="  ";
        }

        return buffer;
    }

    protected override char printRoll(int idx)
    {
        if (this[idx] == 0) return NO_POINTS_SYMBOL;
        else if (this[idx] == PINS) return STRIKE_SYMBOL;
        else return (char)(this[idx] + '0');
    }
}