import java.util.Arrays;

public class Game
{
    private Frame frames[];
    private String player;
    
    public Game ()
    {
        this.frames = new Frame[Frame.N_FRAMES];

        for(byte i = 0;i<(Frame.N_FRAMES-1);i++) this.frames[i] = new Frame(i+1);
        this.frames[Frame.N_FRAMES-1] = new LastFrame();

        this.player="Player";
    }

    public Game (String sequence)
    {
        this();
        this.readSequence(sequence);
        this.calculateScore();
    }

    public Game (String sequence,String player) 
    {
        this(sequence);
        this.player = player;
    }

    public Frame getFrame(int idx)
    {
        return this.frames[idx];
    }

    protected void readSequence(String sequence)
    {
        int i = 0; 
        int f = 0; 
        int r = 0;

        for (char c : sequence.toCharArray()) 
        {
            if (f < (Frame.N_FRAMES-1)) 
            {
                if (Game.is_number(c) || Game.is_miss(c)) 
                {
                    if (Game.is_miss(c)) this.getFrame(f).setRoll(r,0);
                    else this.getFrame(f).setRoll(r, c - '0');

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
                        this.getFrame(f).setRoll(0,Frame.PINS);
                        f++;
                    }
                    else
                    {
                        if ((r==1) && Game.is_spare(c)) 
                        {
                            
                            this.getFrame(f).setRoll(1, Frame.PINS - this.getFrame(f).getRoll(0));

                            f++; 
                            r=0;
                        }
                        else 
                        {
                            throw new RuntimeException(String.format("I don't undernstanad character %c at position %d",c,i+1));
                        }
                    }
                }
            }
            else 
            {
                if (Game.is_number(c) || Game.is_miss(c)) 
                {
                    if (Game.is_miss(c)) this.getFrame(f).setRoll(r,0);
                    else this.getFrame(f).setRoll(r, c - '0');
                } 
                else if(Game.is_strike(c)) this.getFrame(f).setRoll(r,Frame.PINS);
                else
                {

                    if ((r>0) && (is_spare(c))) this.getFrame(f).setRoll(r, Frame.PINS - this.getFrame(f).getRoll(r-1));
                    else throw new RuntimeException(String.format("I don't undernstanad character %c at position %d",c,i+1));
                    
                }

                r++; 
            }
        }
    }

    public String getPlayer() {return this.player;}
    public void setPlayer(String player) {this.player = player;}

    public void calculateScore()
    {
        int next_rolls[] = new int[2]; 

        for (int i=0;i<Frame.N_FRAMES;i++)
        {
            Frame current_frame = this.getFrame(i);
            Arrays.fill(next_rolls,0); //similar to memset in C/C++

            if (current_frame.getFrameNumber() < Frame.N_FRAMES) 
            {
                next_rolls[0] = this.getFrame(i+1).getRoll(0);

                if (this.getFrame(i+1).getFrameNumber() < Frame.N_FRAMES)
                    next_rolls[1] = this.getFrame(i+1).isStrike() ? this.getFrame(i+2).getRoll(0) : this.getFrame(i+1).getRoll(1);
                else next_rolls[1] = this.getFrame(i+1).getRoll(1);                
            }
            
            if (current_frame.getFrameNumber() > 1) current_frame.calculateScore(this.getFrame(i-1),next_rolls);
            else current_frame.calculateScore(next_rolls);
        }
    }

    public void printGame()
    {
        StringBuilder frame_line1 = new StringBuilder();
        StringBuilder frame_line2 = new StringBuilder();
        

        for (Frame current_frame : this.frames)
        {
            frame_line1.append(current_frame);
            frame_line1.append("|");

            if (current_frame.getFrameNumber() == Frame.N_FRAMES) frame_line2.append(String.format("%5d",current_frame.getScore()));
            else frame_line2.append(String.format("%3d",current_frame.getScore()));

            frame_line2.append("|");

        }

        String frames = frame_line1.toString();
        String dashed_line = "+"+"-".repeat(frames.length()-1)+"+";
        String player_line = this.getPlayer();

        System.out.println(dashed_line);
        
        int l_padding = (frames.length() / 2) - (this.getPlayer().length() / 2);
        int r_padding = frames.length() - this.getPlayer().length() - l_padding-1;
       

        System.out.println("|"+" ".repeat(l_padding)+player_line+" ".repeat(r_padding)+"|");
        
        
        //print the top row of the scoring board - just the number of frames
        for(int i=0;i<Frame.N_FRAMES;i++) System.out.print("| "+(i+1)+" ");
        System.out.println(" |");
        
        
        System.out.println(dashed_line);
        System.out.println("|"+frames);


        System.out.println(dashed_line);
        System.out.println("|"+frame_line2);
        
        System.out.println(dashed_line);
    }

    public static boolean is_strike(char c) {return ((c == Frame.STRIKE_SYMBOL) || (c==Frame.STRIKE_SYMBOL_LC)); }
    public static boolean is_spare(char c)  {return (c == Frame.SPARE_SYMBOL); }
    public static boolean is_miss(char c)   {return (c == Frame.NO_POINTS_SYMBOL); }
    public static boolean is_number(char c) {return (c>='0') && (c<='9'); }
};