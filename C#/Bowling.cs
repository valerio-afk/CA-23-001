public class Bowling 
{
    //C# calls the entrypoint Main instead of main (de gustibus)
    public static void Main(string [] args)
    {
        if (args.Length>=1)
        {
            string my_game = args[0];
            Game valerio = new Game(my_game,"Valerio");

            Console.WriteLine(valerio);

            
        }else Console.WriteLine("Devi passare una partita da linea di comando");
    }
}
