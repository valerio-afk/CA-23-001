class Frame
{
    protected int frameNumber;
    protected int rolls[];
    private int score;

    public static final int PINS = 10;
    public static final int N_FRAMES = 10;
    public static final char STRIKE_SYMBOL = 'X';
    public static final char STRIKE_SYMBOL_LC = 'x';
    public static final char SPARE_SYMBOL = '/';
    public static final char NO_POINTS_SYMBOL = '-';

    public Frame()
    {
        this.frameNumber = 0;
        this.score = 0;
        this.rolls = new int[2];
    }

    public Frame(int n)
    {
        this();
        this.frameNumber = n;
    }

    public int getFrameNumber() {return this.frameNumber;}
    
    public void setRoll(int idx, int value) {this.rolls[idx] = value;}
    public int getRoll(int idx) {return this.rolls[idx];}

    public int getScore(){return this.score;}
    protected void setScore(int score) {this.score = score;}

    public boolean isStrike() {return this.getRoll(0) == PINS;}
    public boolean isSpare() {return !this.isStrike() && ((this.getRoll(0)+this.getRoll(1))==PINS);}

    public void calculateScore(int next_rolls[])
    {
        int frame_score = 0;

        frame_score += this.getRoll(0) + this.getRoll(1); 

        if (this.isStrike()) frame_score += next_rolls[0] + next_rolls[1]; 
        else if (this.isSpare()) frame_score += next_rolls[0]; 

        this.setScore(frame_score);
    }

    public void calculateScore(Frame previousFrame, int next_rolls[])
    {
        int score;

        this.calculateScore(next_rolls);
        
        score = previousFrame.getScore() + this.getScore();
        this.setScore(score);
    }

    public String toString()
    {
        return String.format("%c %c",this.printRoll(0), this.printRoll(1));
    }

    protected char printRoll(int idx)
    {
        if ((idx != 1) && (this.getRoll(idx) == PINS)) return STRIKE_SYMBOL;
        //if it is required to get the character of the 2nd roll and we have a strike, we just print an empty space
        else if ((idx==1) && (this.getRoll(idx-1) ==PINS) ) return ' ';
        //if we are not in the first roll and the sum of two neighbour rolls is 10, then it's spare
        else if ((idx>0) && ( (this.getRoll(idx-1) + this.getRoll(idx)) ==PINS) ) return SPARE_SYMBOL;
        //if we didn't knock out any pins, we didn't score!
        else if (this.getRoll(idx) == 0) return NO_POINTS_SYMBOL;
        //all the other cases are numbers in 1-9 -> convert it to the right ASCII digit
        else return (char) (this.getRoll(idx) + '0');
    }

}