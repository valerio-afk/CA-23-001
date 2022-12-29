const PINS = 10
const N_FRAMES = 10
const STRIKE_SYMBOL = 'X'
const STRIKE_SYMBOL_LC = 'x'
const SPARE_SYMBOL = '/'
const NO_POINTS_SYMBOL = '-'

is_strike = (c) => { return ((c == STRIKE_SYMBOL) || (c==STRIKE_SYMBOL_LC)) }
is_spare =  (c) => { return (c == SPARE_SYMBOL) }
is_miss =   (c) => { return (c == NO_POINTS_SYMBOL) }
is_number = (c) => { return !isNaN(parseInt(c)) }

class Frame
{
    constructor(frameNumber)
    {
        this.frameNumber = frameNumber
        this.rolls = Array(2).fill(0)
        this.score = 0 
    }

    get FrameNumber () {return this.frameNumber;}
    
    isStrike() {return this.rolls[0]==PINS; }
    isSpare () {return !this.isStrike() && ((this.rolls[0]+this.rolls[1])==PINS); }
    toString() {return this.printRoll(0) + " " +this.printRoll(1);}

    calculateScore(previous_frame, next_rolls)
    {
        let frame_score = 0

        if (typeof previous_frame !== 'undefined') frame_score += previous_frame.score

        frame_score += this.rolls[0] + this.rolls[1]; 

        
        if (this.isSpare()) frame_score += next_rolls[0]; 
        else if (this.isStrike()) frame_score += next_rolls[0] + next_rolls[1]; 

        this.score = frame_score; 
    }
    
    printRoll(idx)
    {
        if ((idx != 1) && (this.rolls[idx] == PINS)) return STRIKE_SYMBOL;
        else if ((idx==1) && (this.rolls[idx-1] ==PINS) ) return ' ';
        else if ((idx>0) && ( (this.rolls[idx-1] + this.rolls[idx]) ==PINS) ) return SPARE_SYMBOL;
        else if (this.rolls[idx] == 0) return NO_POINTS_SYMBOL;
        else return this.rolls[idx];
    }
}

class LastFrame extends Frame
{
    constructor()
    {
        super(N_FRAMES);
        this.rolls = Array(3).fill(0)
    }

    calculateScore (previous_frame)
    {
        let frame_score = previous_frame.score;
        frame_score += this.rolls[0] + this.rolls[1]; 
        if (this.isStrike() || this.isSpare()) frame_score += this.rolls[2];
        
        this.score = frame_score; 
    }

    printRoll(idx)
    {
        if (this.rolls[idx] == 0) return NO_POINTS_SYMBOL;
        else if (this.rolls[idx] == PINS) return STRIKE_SYMBOL;
        else return parseInt(this.rolls[idx]);
    }
    
    toString()
    {
        let frame = "";
        
        if (this.isSpare())
        {
            frame = super.toString() //this is like super OR base
            frame +=" " + this.printRoll(2);
        }
        else if (this.isStrike())
        {
            frame=this.printRoll(0)+" ";
            frame+=this.printRoll(1)+" ";
            frame+=super.printRoll(2);
        }
        else frame = super.toString(this) + "  ";

        return frame;
    }
}


class Game
{
    constructor(sequence, player_name)
    {
        let i;
        this.frames = []

        for(i = 0;i<(N_FRAMES-1);i++) this.frames[i] = new Frame(i+1);
        this.frames[N_FRAMES-1] = new LastFrame();
    
        this.player = typeof player_name == 'undefined' ? "Player" : player_name ;

        if (typeof sequence != 'undefined')
        {
            this.readSequence(sequence)
            this.calculateScore()
        }
    }

    get Player(){ return this.player }
    set Player(val) {this.player = val }

    readSequence(sequence)
    {
        let len = sequence.length;
        let i = 0; 
        let f = 0; 
        let r = 0;

        for (i = 0; i<len;i++) 
        {
            if (f < (N_FRAMES-1)) 
            {
                if (is_number(sequence[i]) || is_miss(sequence[i])) 
                {
                    if (is_miss(sequence[i])) this.frames[f].rolls[r] = 0;
                    else this.frames[f].rolls[r] =  parseInt(sequence[i]);

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
                        this.frames[f].rolls[0] = PINS;
                        f++;
                    }
                    else
                    {
                        if ((r==1) && is_spare(sequence[i])) 
                        {
                            
                            this.frames[f].rolls[1] =  PINS - this.frames[f].rolls[0]

                            f++; 
                            r=0;
                        }
                        else 
                        {
                            throw `I don't understand character ${sequence[i]} at position ${i+1}`;
                        }
                    }
                }
            }
            else 
            {
                if (is_number(sequence[i]) || is_miss(sequence[i])) 
                {
                    if (is_miss(sequence[i])) this.frames[f].rolls[r] = 0
                    else this.frames[f].rolls[r] =  parseInt(sequence [i])
                } 
                else if(is_strike(sequence[i])) this.frames[f].rolls[r] = PINS
                else
                {

                    if ((r>0) && (is_spare(sequence[i]))) this.frames[f].rolls[r] = PINS - this.frames[f].rolls[r-1]
                    else
                    {
                        throw `I don't understand character ${sequence[i]} at position ${i+1}`;
                    }
                }

                r++; 
            }
        }
    }

    calculateScore()
    {
        let next_rolls = undefined; 
        let i = 0;

        for (i=0;i<N_FRAMES;i++)
        {
            let current_frame = this.frames[i]
            next_rolls = Array(2).fill(0)

            if (current_frame.FrameNumber < N_FRAMES) 
            {
                next_rolls[0] = this.frames[i+1].rolls[0]

                if (this.frames[i+1].FrameNumber < N_FRAMES)
                    next_rolls[1] = this.frames[i+1].isStrike() ? this.frames[i+2].rolls[0] : this.frames[i+1].rolls[1]
                else next_rolls[1] = this.frames[i+1].rolls[1]
            }

            current_frame.calculateScore(this.frames[i-1],next_rolls);
        }
    }

    printGame()
    {
        let frame_line1 = "";
        let frame_line2 = "";
        
        for (let current_frame of this.frames)
        {
           
            frame_line1 += current_frame + "|";

            //print 2nd line with totals
            //the only difference in case it is the last frame is the number of paddind characters used
            //not a big deal but I like nice ASCII graphics
            let score_str = "    "+current_frame.score.toString()
            if (current_frame.FrameNumber == N_FRAMES) score_str = score_str.slice(-5)
            else score_str = score_str.slice(-3)

            frame_line2+=score_str+"|";

        }

        

        let dashed_line = "-".repeat(frame_line1.length+1)

        console.log(dashed_line)

        //this works differently than seen before
        let l_padding = (frame_line1.length / 2) + (this.player.length / 2);
        let padded_player_name = this.Player.padStart(l_padding," ").padEnd(frame_line1.length-1," ")

        console.log("|"+padded_player_name+"|")
        
        
        //print the top row of the scoring board - just the number of frames
        let frame_numbers = ""
        for(let i=0;i<N_FRAMES;i++) frame_numbers+="| "+(i+1)+" ";
        frame_numbers += " |"

        console.log(frame_numbers)
        console.log(dashed_line)

        console.log("|"+frame_line1)
        console.log(dashed_line)
        
        console.log("|"+frame_line2)
        console.log(dashed_line)
    }

}


my_game = "9-XX9-7/XX8-6/XX9";
valerio = new Game(my_game,"Valerio")
valerio.printGame();
