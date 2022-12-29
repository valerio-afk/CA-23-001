<?php
define("PINS",10);
define("N_FRAMES",10);
define("STRIKE_SYMBOL" ,'X');
define("STRIKE_SYMBOL_LC" ,'x');
define("SPARE_SYMBOL",'/');
define("NO_POINTS_SYMBOL",'-');

// logical or operator is 'or' and not the double pipe || as we saw so far
function is_strike ($c) { return (($c == STRIKE_SYMBOL) or ($c==STRIKE_SYMBOL_LC)); }
function is_spare  ($c) { return ($c == SPARE_SYMBOL); }
function is_miss   ($c) { return ($c == NO_POINTS_SYMBOL); }

//we don't need this function, PHP has already is numeric
//function is_number ($c) { return is_numeric($c); }

//an interface is a trick used in java and 
class Frame implements ArrayAccess
{
    protected  $frameNumber;
    protected  $rolls;
    private    $score;

    //public is by default
    function __construct($frameNumber)
    {
        $this->frameNumber = $frameNumber;
        $this->rolls = array_fill(0,2,0);
        $this->score = 0;
    }

    public function getFrameNumber() {return $this->frameNumber;}

    //these are the methods needed to be implemented in the interface
    //this method is called when isset($obj[0]) is called
    public function offsetExists($idx){return isset($this->rolls[$idx]);}
    
    //this method is called in an expression like $obj[0]
    public function offsetGet($idx) {return $this->rolls[$idx];}

    //this method is called in an expression like $obj[0] = 1
    public function offsetSet($idx, $value) {$this->rolls[$idx] = $value; }

    //this method is called when unset($obj[0]) is invoked
    //In our case, we cannot lalow unset like that 
    //However, we can take advantage of this that, instead of removing the item
    //from the array, we set it to 0
    public function offsetUnset($idx) {$this->rolls[$idx] = 0;}  
    
    public function getScore() {return $this->score; }
    protected function setScore($score) {$this->score = $score;}
    
    public function isStrike() {return $this[0] == PINS;}
    public function isSpare() {return !$this->isStrike() && (($this[0]+$this[1])==PINS);}

    public function calculateScore($previous_frame, $next_rolls)
    {
        $frame_score = 0;

        if (isset($previous_frame)) $frame_score += $previous_frame->getScore();

        $frame_score += $this[0] + $this[1];

        if (count($next_rolls)>=2)
        {
            if ($this->isStrike()) $frame_score += $next_rolls[0] + $next_rolls[1]; 
            elseif ($this->isSpare()) $frame_score += $next_rolls[0]; 
        }

        $this->setScore($frame_score);
    }

    public function __toString()
    {
        //this little friend comes back :) - I love printf and its variations
        return sprintf("%s %s",$this->printRoll(0), $this->printRoll(1));
    }

    protected function printRoll($idx)
    {
        if (($idx != 1) and ($this[$idx] == PINS)) return STRIKE_SYMBOL;
        elseif (($idx==1) and ($this[$idx-1] ==PINS) ) return ' ';
        elseif (($idx>0) and ( ($this[$idx-1] + $this[$idx]) ==PINS) ) return SPARE_SYMBOL;
        elseif ($this[$idx] == 0) return NO_POINTS_SYMBOL;
        else return strval($this[$idx]);
    }
}

class LastFrame extends Frame
{
    //php here complains with this nonsense error:
    //PHP Fatal error:  Declaration of LastFrame::__constructor() must be compatible with Frame::__constructor($frameNumber)
    //I put a dummy parameter that it's never used
    //Finally, we are using parameters with default values
    //I need to do this because PHP is more pedantic that I remembered
    //PHP Fatal error:  Uncaught ArgumentCountError: Too few arguments to function LastFrame::__construct(), 0 passed
    public function __construct($_ = NULL)
    {
        //scope resolution operator ::
        //clearly borrowed from c++
        parent::__construct(N_FRAMES);
        $this->rolls = array_fill(0,3,0);
    }

    //same thing happens here - php not only does not overloading, it even dislike it!
    //Since I can call variables as I like, I simply call it _ (an underscore)
    public function calculateScore($previousFrame, $_)
    {
        $frame_score = $previousFrame->getScore();

        $frame_score += $this[0] + $this[1]; 

        if ($this->isStrike() or $this->isSpare()) $frame_score += $this[2];


        $this->setScore($frame_score);
    }

    public function __toString()
    {
        $buffer = "";
        if ($this->isSpare())
        {
            //this is the concatenation operator for string
            $buffer.=parent::__toString(); //here ToString must be called explicitily because base seems to be seen as a keyword rather than a regular object.
            $buffer.=" ";
            $buffer.=$this->printRoll(2);
        }
        elseif ($this->isStrike())
        {
            $buffer.=$this->printRoll(0);
            $buffer.=" ";
            $buffer.=$this->printRoll(1);
            $buffer.=" ";
            $buffer.=parent::printRoll(2);
        }
        else
        {
            $buffer.=parent::__toString(); 
            $buffer.="  ";
        }

        return $buffer;
    }

    protected function printRoll($idx)
    {
        if ($this[$idx] == 0) return NO_POINTS_SYMBOL;
        elseif ($this[$idx] == PINS) return STRIKE_SYMBOL;
        else return strval($this[$idx]);
    }
}

class Game implements ArrayAccess
{
    private $frames;
    public $player;
    
    public function __construct ($sequence,$player)
    {
        $this->frames = array();

        for($i = 0;$i<(N_FRAMES-1);$i++) $this->frames[$i] = new Frame($i+1);
        $this->frames[N_FRAMES-1] = new LastFrame();

        if (isset($sequence))
        {
            $this->readSequence($sequence);
            $this->calculateScore();
        }

        //wtf is this operator?!
        $this->player= $player??"Player";
    }

    //these are the methods needed to be implemented in the interface
    //this method is called when isset($obj[0]) is called
    public function offsetExists($idx){return isset($this->frames[$idx]);}
    
    //this method is called in an expression like $obj[0]
    public function offsetGet($idx) {return $this->frames[$idx];}

    //we cannot allow a set of a new frame externally. Here I prefer throwing an exception
    public function offsetSet($idx, $value) 
    {
        throw BadMethodCallException("This is a readonly property.");
    }

    //this method is called when unset($obj[0]) is invoked
    //In our case, we cannot lalow unset like that 
    //However, we can reset the frame using the offsetUnset method in Frame
    //I won't do this now because php has a limitation that python doesn't
    //The discussion will be moved then
    public function offsetUnset($idx) {throw BadMethodCallException("This is a readonly property.");}  

    public function readSequence($sequence)
    {
        $f = 0; 
        $r = 0;

        //php doesn't expose that a string is an array of characters.
        //therefore, to access each character in a string I need to split it to chunks of size 1
        $chars = str_split($sequence,1); 

        foreach($chars as $c)
        {
            if ($f < (N_FRAMES-1)) 
            {
                if (is_numeric($c) || is_miss($c)) 
                {
                    if (is_miss($c)) $this[$f][$r] = 0;
                    else $this[$f][$r] = intval($c);

                    $r++;
                    if ($r>1)
                    {
                        $f++;
                        $r=0;
                    }
                }
                else 
                {
                    if (($r==0) && is_strike($c)) 
                    {
                        $this[$f][0] = PINS;
                        $f++;
                    }
                    else
                    {
                        if (($r==1) && is_spare($c)) 
                        {
                            
                            $this[$f][1] = PINS - $this[$f][0];

                            $f++; 
                            $r=0;
                        }
                        else 
                        {
                            throw Exception(sprintf("I don't undernstanad character %c at position %d",$c,$i+1));
                        }
                    }
                }
            }
            else 
            {
                if (is_numeric($c) || is_miss($c)) 
                {
                    if (is_miss($c)) $this[$f][$r] = 0;
                    else $this[$f][$r] = intval($c);
                } 
                else if(is_strike($c)) $this[$f][$r] = PINS;
                else
                {

                    if (($r>0) && (is_spare($c))) $this[$f][$r] = PINS - $this[$f][$r-1];
                    else throw Exception(sprintf("I don't undernstanad character %c at position %d",$c,$i+1));
                    
                }

                $r++; 
            }
        }
    }

    public function calculateScore()
    {
        for ($i=0;$i<N_FRAMES;$i++)
        {
            $current_frame = $this[$i];
            $next_rolls = array_fill(0,2,0);

            if ($current_frame->getFrameNumber() < N_FRAMES) 
            {
                $next_rolls[0] = $this[$i+1][0];

                if ($this[$i+1]->getFrameNumber() < N_FRAMES)
                    $next_rolls[1] = $this[$i+1]->isStrike() ? $this[$i+2][0] : $this[$i+1][1];
                else $next_rolls[1] = $this[$i+1][1];                
            }
            
            
            if ($current_frame->getFrameNumber() > 1) $current_frame->calculateScore($this[$i-1],$next_rolls);
            else $current_frame->calculateScore(NULL,$next_rolls);
        }
    }

    public function printGame()
    {
        $frame_line1 = "";
        $frame_line2 = "";        

        foreach ($this->frames as $current_frame)
        {
            $frame_line1 .= strval($current_frame);
            $frame_line1 .= "|";

            switch($current_frame->getFrameNumber())
            {
                case N_FRAMES:
                    $frame_line2.= sprintf("%5d",$current_frame->getScore());
                    break;
                default:
                    $frame_line2.= sprintf("%3d",$current_frame->getScore());
            }

            $frame_line2 .= "|";

        }

        $dashed_line = str_repeat('-',strlen($frame_line1)+1)."\n";
        $player_line = $this->player;

        echo $dashed_line;
        
        echo "|". str_pad($this->player,strlen($frame_line1)-1," ", STR_PAD_BOTH) ."|\n";
               
        //print the top row of the scoring board - just the number of frames
        for($i=0;$i<N_FRAMES;$i++) echo "| ".strval($i+1)." ";
        echo " |\n";  
        
        echo $dashed_line;
        echo "|".$frame_line1."\n";

        echo $dashed_line;
        echo "|".$frame_line2."\n";
        
        echo $dashed_line;
    }
}

$my_game = "9-XX9-7/XX8-6/XX9";
$valerio = new Game($my_game,"Valerio");
$valerio->printGame();
?>