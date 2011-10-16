public class Light
{
	// Light position
	private int x;
	private int y;

	// Light size
	private int size;

	// Screen size
	private int screenWidth;
	private int screenHeight;

	// Person info
	private Recorder.Side coveredEye = Recorder.Side.Unspecified;

	// How long it took to react
	private boolean isRunning = false;
	private boolean hasRun = false;
	private long startTime;
	private long endTime;

	// Recorder functions
	public void startTime()
	{
		if(isRunning)
			return;

		startTime = System.nanoTime();
		isRunning = true;
	}

	public void stopTime()
	{
		// Record time first for a little more accuracy, then check
		// for an invalid call
		endTime = System.nanoTime();

		// And we're done
		isRunning = false;
		hasRun = true;
	}

	public long getTime() throws Exception
	{
		if(isRunning)
			throw new Exception("Timer currently running, unable to get time");
		if(!hasRun)
			throw new Exception("Timer has not yet run for this light");

		return Math.abs(endTime - startTime);
	}

	// Getters and setters
	public Recorder.Side getEye()
	{
		return coveredEye;
	}
	public void setEye(Recorder.Side newEye)
	{
		coveredEye = newEye;
	}
	public boolean isLeftCovered()
	{
		return (coveredEye == Recorder.Side.Left);
	}
	public boolean isRightCovered()
	{
		return (coveredEye == Recorder.Side.Right);
	}

	public int getSize()
	{
		return size;
	}
	public void setSize(int newSize)
	{
		size = newSize;
	}

	public int getScreenWidth()
	{
		return screenWidth;
	}
	public void setScreenWidth(int newScreenWidth)
	{
		screenWidth = newScreenWidth;
	}

	public int getScreenHeight()
	{
		return screenHeight;
	}
	public void setScreenHeight(int newScreenHeight)
	{
		screenHeight = newScreenHeight;
	}

	public void setScreenSize(int height, int width)
	{
		setScreenHeight(height);
		setScreenWidth(width);
	}

	public int getY()
	{
		return y;
	}
	public void setY(int newY)
	{
		y = newY;
	}

	public int getX()
	{
		return x;
	}
	public void setX(int newX)
	{
		x = newX;
	}

	public void setPosition(int x, int y)
	{
		setX(x);
		setY(y);
	}

	@Override
	public String toString()
	{
		StringBuilder sb = new StringBuilder();
		sb.append(coveredEye).append(' ');
		try
		{
			long time = getTime();
			sb.append((double)time / 1000000.0).append("ms");
		}
		catch(Exception ex)
		{
			sb.append("not yet run");
		}

		return sb.toString();
	}
}
