package hello;

public class HelloWorld
{
	public static void main(String [] args)
	{
		if(args.length > 0)
		{
			System.out.print("Hello world from " + args[0] + "\n");
		}
		else
		{
			System.out.print("Hello world\n");
		}
		System.out.flush();
	}
}
