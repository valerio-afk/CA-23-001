macro_rules!is_strike{ ($c:expr) => {$c == STRIKE} }
macro_rules!is_spare{ ($c:expr) => {$c == SPARE} }

macro_rules!get_score {
    ($c:expr, $prev_roll:expr) => {match $c{
        STRIKE => PINS,
        NO_POINTS => 0,
        SPARE => PINS - $prev_roll,
        _ => $c.to_digit(10).unwrap() as u8
    }}
}

#[warn(non_snake_case)]
mod Bowling
{
    use std::fmt;
    pub const STRIKE:char = 'X';
    pub const SPARE:char = '/';
    pub const NO_POINTS:char = '-';
    pub const PINS:u8 = 10;
    pub const FRAMES:u8 = 10;

    enum FrameType{
        OpenFrame,
        Spare,
        Strike,
        LastFrame
    }

    pub struct Game{
        pub game_sequence:String,
        pub player:String,
        iter_curr:u8,
        scores:Option<Vec<u16>>
    }

    impl Game{
        pub fn new(sequence:String, player:String) -> Game
        {
            Game{game_sequence:sequence,
                player:player,
                iter_curr:0,
                scores:None
            }
        }

        pub fn get_score(&self) -> Option<Vec<u16>>
        {
            if let Some(s) = &self.scores
            {
                Some(s.to_vec())
            }
            else { None }
        }

        pub fn get_frame(&self, idx:u8) -> Option<(Vec<u8>,Vec<u8>)>{
            let mut frame:Vec<u8> = Vec::new();
            let mut rolls_idx:Vec<u8> = Vec::new();
            let chars:Vec<char> = self.game_sequence.chars().collect();
            let mut frame_number = 0;
            let mut i = 0;

            if idx >= FRAMES { return None; }

            loop //while i < chars.len()
            {
                if frame_number < idx //we need to skip
                {
                    if is_strike!(chars[i]){i+=1}
                    else{i+=2} //in any other case, we skip two characters
                    frame_number+=1;
                }
                else
                {
                    if frame_number < (FRAMES-1)
                    {
                        if is_strike!(chars[i])
                        {
                            rolls_idx.push(i as u8);
                            frame.push(get_score!(chars[i],0));
                        }
                        else
                        {
                            for j  in 0..2
                            {
                                frame.push(get_score!(chars[i+j],frame[0]));
                                rolls_idx.push((i+j) as u8);
                            }
                        }
                    }
                    else //last frame
                    {
                        let expected_rolls = if is_strike!(chars[i]) || is_spare!(chars[i+1]) {3} else {2};

                        for j in 0..expected_rolls
                        {
                            println!(">>> {}",chars[i+j]);
                            frame.push(get_score!(chars[i+j],frame[j-1]));
                            rolls_idx.push((i+j) as u8);
                        }
                    }

                    break;
                }
            }

            Some ((frame,rolls_idx))
        }

        fn get_roll(&mut self, idx:u8) -> Option<u8>
        {
            for i in 0..FRAMES
            {
                if let Some((scores,roll_idx)) = self.get_frame(i)
                {
                    for j in 0..scores.len()
                    {
                        if roll_idx[j] == idx { return Some(scores[j]); }
                    }
                }
            }

            None
        }

        pub fn calculate_score(&mut self)
        {
            self.scores = Some({
                let mut calculated_scores:Vec<u16> = Vec::new();

                for i in 0..FRAMES
                {

                    if let Some(frame) = self.get_frame(i)
                    {
                        let mut frame_score:u16 = match i {
                            0 => 0,
                            _ => *calculated_scores.last().unwrap()
                        };

                        let (rolls, _) = frame;

                        frame_score += rolls.iter().sum::<u8>() as u16;
                        let next_rolls = self.get_next_rolls(i);

                        if next_rolls != None
                        {
                            frame_score += next_rolls.unwrap().iter().sum::<u8>() as u16;
                        }

                        calculated_scores.push(frame_score);
                    }
                }

                calculated_scores

            })
        }

        fn get_next_rolls(&mut self,idx:u8) -> Option<Vec<u8>>
        {
            if idx<(FRAMES-1)
            {
                let mut rolls:Vec<u8> = Vec::new();
                let roll_idxs = self.get_frame(idx).unwrap().1;

                return match self.get_frame_status(idx)
                {
                    Some(FrameType::Strike) => {
                        rolls.push(self.get_roll(roll_idxs.last().unwrap()+1).unwrap());
                        rolls.push(self.get_roll(roll_idxs.last().unwrap()+2).unwrap());
                        Some(rolls)
                    },
                    Some(FrameType::Spare) => {
                        rolls.push(self.get_roll(roll_idxs.last().unwrap()+1).unwrap());
                        Some(rolls)
                    },
                    _ => Some(rolls)
                };

            }
            None
        }

        fn get_frame_status(&mut self,idx:u8) -> Option<FrameType>
        {
            if idx<FRAMES
            {
                let frame = self.get_frame(idx);

                if frame != None
                {
                    let f = frame.unwrap().0;

                    if idx==(FRAMES-1) { return Some(FrameType::LastFrame); }
                    else if f[0] == PINS { return Some(FrameType::Strike); }
                    else if (f[0]+f[1]) == PINS { return Some(FrameType::Spare);}
                    else {return Some(FrameType::OpenFrame);}
                }
            }
            None
        }

        fn print_roll(roll:u8) -> char
        {
            match roll
            {
                10 => STRIKE,
                0 => NO_POINTS,
                _ => char::from_digit(roll as u32,10).unwrap()
            }
        }

        fn print_frame_from_rolls(rolls:&Vec<u8>) -> String
        {
            if rolls[0]==10 {return format!("{}",Game::print_roll(rolls[0])); }
            if (rolls[0]+rolls[1]) == 10 { return format!("{} {}",Game::print_roll(rolls[0]),SPARE); }
            else { return format!("{} {}",Game::print_roll(rolls[0]),Game::print_roll(rolls[1])); }

        }


        fn print_frame(&self, idx:u8) -> String
        {
            if let Some((rolls,_)) = self.get_frame(idx)
            {
                if idx<(FRAMES-1){ return Game::print_frame_from_rolls(&rolls); }
                else
                {
                    if rolls[0] == 10 { return format!("{} {} {}", Game::print_roll(rolls[0]), Game::print_roll(rolls[1]), if (rolls[1]+rolls[2])==PINS {SPARE} else {Game::print_roll(rolls[2])});}
                    else { 
                        if rolls.len()==2 { return format!("{}", Game::print_frame_from_rolls(&rolls));}
                        else {return format!("{} {}",Game::print_frame_from_rolls(&rolls),Game::print_roll(*rolls.last().unwrap()));}
                    }
                }
            }

            "".to_string()
        }
    }

    impl Iterator for Game
    {
        type Item = Vec<u8>;

        fn next(&mut self) -> Option<Self::Item>
        {
            let result = self.get_frame(self.iter_curr);

            self.iter_curr = match result
            {
                None => {0},
                _ => {self.iter_curr+1}
            };

            return match result
            {
                None => None,
                _ => Some(result.unwrap().0)
            };
        }
    }

    impl fmt::Display for Game
    {
        fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result
        {
            if let Some(scores) = self.get_score()
            {
                let mut line1 = String::new();
                let mut line2 = String::new();
                let mut line3 = String::new();

                line1.push('|');
                line2.push('|');
                line3.push('|');

                for i in 0..FRAMES
                {
                    let width = match i{
                        9 =>5,
                        _ =>3
                    };

                    line1.push_str(format!("{:^width$}|",i+1).as_str());
                    line2.push_str(format!("{:width$}|",self.print_frame(i)).as_str());
                    line3.push_str(format!("{:<width$}|",scores[i as usize]).as_str());
                }

                let n = line1.len()-2;
                let dashed_line = format!("+{}+","-".repeat(n));
                let player_name = format!("|{player:^length$}|",length=n,player=self.player);

                write!(f,"{dashed_line}\n{player_name}\n{dashed_line}\n{line1}\n{dashed_line}\n{line2}\n{dashed_line}\n{line3}\n{dashed_line}")
            }
            else {write!(f,"")}
        }
    }
}


fn main()
{
    use Bowling::*;
    let my_game:String = "7/9/7/XXX72X7/51".to_string(); //"9-XX9-7/XX8-6/XX9".to_string();

    let mut valerio:Game = Game::new(my_game,"Valerio".to_string());

    valerio.calculate_score();

    println!("{}",valerio);
}