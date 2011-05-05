package summer;
import java.util.Scanner;

public class Main
{
    public static void main(String[] args)
	{
		try
		{
			Scanner in = new Scanner(System.in);

			while(true)
			{
				// Get number to go to
				System.out.print("Number to sum to: ");
				int sumTo = in.nextInt();

				// Stop when user asks for 0
				if(sumTo == 0)
				{
					break;
				}

				int sum = 0;
				for(int odd = 1; odd <= sumTo; odd += 2)
				{
					sum += odd;
				}

				System.out.println(sum);
			}

			in.close();
		}
		catch(Exception e)
		{

		}
    }

}
