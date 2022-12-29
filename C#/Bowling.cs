public class Bowling 
{
    //C# calls the entrypoint Main instead of main (degustibus)
    public static void Main(string [] args)
    {
        string my_game = "9-XX9-7/XX8-6/XX9"; 
        Game valerio = new Game(my_game,"Valerio"); 

        valerio.printGame();
    }
}
