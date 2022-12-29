class LastFrame extends Frame
{
    public LastFrame () 
    {
        this.frameNumber = Frame.N_FRAMES;
        this.rolls = new int[3];
    }

    public void calculateScore(Frame previousFrame, int next_rolls[])
    {
        this.calculateScore(previousFrame);
    }

    public void calculateScore(Frame previousFrame)
    {
        int frame_score = previousFrame.getScore();

        frame_score += this.getRoll(0) + this.getRoll(1); 

        if (this.isStrike() || this.isSpare()) frame_score += this.getRoll(2);


        this.setScore(frame_score);
    }

    public String toString()
    {
        StringBuilder buffer = new StringBuilder();
        if (this.isSpare())
        {
            buffer.append(super.toString());
            buffer.append(" ");
            buffer.append(this.printRoll(2));
        }
        else if (this.isStrike())
        {
            buffer.append(this.printRoll(0));
            buffer.append(" ");
            buffer.append(this.printRoll(1));
            buffer.append(" ");
            buffer.append(super.printRoll(2));
        }
        else
        {
            buffer.append(super.toString());
            buffer.append("  ");
        }

        return buffer.toString();
    }

    protected char printRoll(int idx)
    {
        if (this.getRoll(idx) == 0) return NO_POINTS_SYMBOL;
        else if (this.getRoll(idx) == PINS) return STRIKE_SYMBOL;
        else return (char)(this.getRoll(idx) + '0');
    }
}