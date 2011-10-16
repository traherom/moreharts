import java.util.ArrayList;

public class Recorder
{
	public enum Gender { Male, Female, Unspecified };
	public enum Side { Left, Right, Unspecified };

	// Info about person being recorded
	private String name = "";
	private int age = 0;
	private Gender gender;
	private Side dominateEye;
	private Side dominateHand;

	// How they do
	private ArrayList<Light> lights = new ArrayList<Light>();

	// Save a new light trial to this person
	public void recordLight(Light newLight)
	{
		lights.add(newLight);
	}
	
	// Getters and setters
	public ArrayList<Light> getLights()
	{
		return lights;
	}

	public void setName(String newName)
	{
		name = newName;
	}

	public String getName()
	{
		return name;
	}

	public void setAge(int newAge) throws Exception
	{
		if(newAge < 1)
			throw new Exception("Age must be at least 1 year old");

		age = newAge;
	}
	public int getAge()
	{
		return age;
	}

	public void setGender(Gender newGender)
	{
		gender = newGender;
	}
	public Gender getGender()
	{
		return gender;
	}
	public boolean isMale()
	{
		return (gender == Gender.Male);
	}
	public boolean isFemale()
	{
		return (gender == Gender.Female);
	}

	public void setDominateHand(Side side)
	{
		dominateHand = side;
	}
	public Side getDominateHand()
	{
		return dominateHand;
	}

	public void setDominateEye(Side side)
	{
		dominateEye = side;
	}
	public Side getDominateEye()
	{
		return dominateEye;
	}

	public long getAverageTime() throws Exception
	{
		long total = 0;
		for(Light light : lights)
		{
			total += light.getTime();
		}
		return total / lights.size();
	}

	// Obligatory string creator
	@Override
	public String toString()
	{
		StringBuilder sb = new StringBuilder();
		sb.append("Name: ").append(name);
		sb.append("\nAge: ").append(age);
		sb.append("\nDominate Eye: ").append(dominateEye);
		sb.append("\nDominate Hand: ").append(dominateHand);
		sb.append("\nGender: ").append(gender).append('\n');

		for(Light light : lights)
		{
			sb.append(light).append('\n');
		}

		return sb.toString();
	}
}
