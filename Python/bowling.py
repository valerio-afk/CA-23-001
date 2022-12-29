allowed_roll = lambda lst,x : (x in lst) or (0<=int(x)<Roll.PINS)
#Yes, a new class could have been used, but I prefer this to introduce you to dictionaries
new_frame = lambda : {'printable_rolls':[],'rolls':[],'score':0,'cum_score':0}

is_strike = lambda r : isinstance(r,Strike)

class Roll:
    PINS = 10

    def __init__(this, value, look_ahead = 0):
        this.value = value
        this.look_ahead = look_ahead
    
    def __str__(this):
        return str(this.value) if (this.value>0) else '-'
    
    def __repr__(this):
        return str(this)
    
    def __add__(this,obj):
        if isinstance(obj,Roll) or (type(obj) == int):
            return this.value + int(obj)
    
    __radd__ = __add__
    
    def __int__(this):
        return this.value

class Spare(Roll):

    def __init__(this,previous_roll):
        super().__init__(Roll.PINS - previous_roll.value,1)
    
    def __str__(this):
        return '/'

class Strike(Roll):

    def __init__(this):
        super().__init__(10,2)
    
    def __str__(this):
        return 'X'

def convertToRoll(char, prev_roll=None):
    if (char in ['X','x']): return Strike()
    elif (char == '/'): return Spare(prev_roll)
    elif (char == '-'): return Roll(0)
    else: return Roll(int(char))


class Game:
    FRAMES = 10

    def __init__(this, sequence=None, player="Player"):
        this.__rolls = [] #this is python way to make private stuff
        this.player = player
        this.score = 0

        if (sequence is not None):
            this.readSequence(sequence)
            #this.calculateScore()

    #another magic method (__init__ is one)
    #this allows to use an instance as it was an array
    def __getitem__(this, key):
        return this.__rolls[key]

    def getFrames(this):
        prev_frame = None
        frame = new_frame()
        n_frame=1

        for i,roll in enumerate(this.__rolls):
            frame["printable_rolls"].append(str(roll))
            frame["rolls"].append(int(roll))
            frame["score"] += roll.value

            if ( (n_frame < Game.FRAMES) and ((is_strike(roll)) or (len(frame['rolls'])==2)) ):
                frame["score"] += sum(this[i+1:(i+roll.look_ahead+1)])
                frame['cum_score'] = frame['score'] + prev_frame['cum_score'] if (prev_frame is not None) else frame['score']

                yield frame

                prev_frame = frame
                frame = new_frame()
                n_frame+=1
            else: 
                if ((len(frame['rolls'])==2) and (frame["score"]<Roll.PINS) or #yields 10th frame if we have two rolls and cannot have more
                    (len(frame['rolls'])==3)): #yields 10th frame when we have 3 rolls
                    
                    #here we could avoid copy n paste somehow (ex nested function, but let's not overdo)
                    frame['cum_score'] = frame['score'] + prev_frame['cum_score'] if (prev_frame is not None) else frame['score']

                    yield frame


    def readSequence(this,sequence):
        this.__rolls = []

        frame = 1
        current_roll=0
        previous_roll = None

        for i,c in enumerate(sequence):
            roll = None
            if (current_roll == 0):
                allowed_symbols = ['-','X','x']
            else:
                if ((frame < Game.FRAMES) or ((frame == Game.FRAMES) and not is_strike(previous_roll)) ):
                    allowed_symbols = ['-','/']
                else:
                    allowed_symbols = ['-','X','x']
            
            if allowed_roll(allowed_symbols, c):
                roll = convertToRoll(c,previous_roll)
            else: raise Exception(f"I don't understand character {c} at position {i+1}")

            if (frame == Game.FRAMES):
                roll.look_ahead = 0 #strikes and spares in the last frame are just 10

            this.__rolls.append(roll)
            previous_roll = roll

            if (frame < Game.FRAMES):
                if (((current_roll==0) and (roll.value == 10)) or #move to next frame if we scored a strike not in the last frame
                    (current_roll>0)):                            #or when we are in the 2nd roll in any non-last frame
                    frame+=1
                    current_roll = 0
                else:
                    current_roll+=1

            else: #this is the last frame
                if (current_roll<2):
                    current_roll+=1

                    if (sum(this[-2:])<Roll.PINS): #we are not entitled to the extra roll(s)
                        break #exit for prematurely
                else:
                    break #we reached the last roll
    
    def __str__(this):
        frame_line0 = []
        frame_line1 = []
        frame_line2 = []

        for i,frame in enumerate(this.getFrames()):
            size = 3 if i<(Game.FRAMES-1) else 5
            
            frame_line0.append(str(i+1).center(size))
            frame_line1.append(" ".join(frame['printable_rolls']).ljust(size))
            frame_line2.append(str(frame['cum_score']).rjust(size))
        
        line0= "|{}|".format("|".join(frame_line0))
        line1= "|{}|".format("|".join(frame_line1))
        line2= f"|{'|'.join(frame_line2)}|" #let's use a different string formatting approach

        dashed_line = "-"*len(line0)

        formatted_player = "|"+ this.player.center(len(line0)-2) +"|"

        return f"{dashed_line}\n{formatted_player}\n{line0}\n{dashed_line}\n{line1}\n{dashed_line}\n{line2}\n{dashed_line}\n"

    __repr__ = __str__

my_game = "9-XX9-7/XX8-6/XX9"
valerio = Game(my_game,"Valerio")
print(valerio)