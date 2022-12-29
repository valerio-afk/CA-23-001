public class Bowling 
{
    public static void main(String [] args)    
    {
        String my_game = "9-XX9-7/XX8-6/XX9"; //this is a real game I have scored a few months ago
        Game valerio = new Game(my_game,"Valerio"); //I initialise my game

        valerio.printGame();
    }
}
