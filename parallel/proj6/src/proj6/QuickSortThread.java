package proj6;

/**
 * A thread that will sort a set array upon execution.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class QuickSortThread extends Thread
{
    /** The back-reference to the class containing the sorting routine.*/
    Project6 proj6;
    /** The ID of this thread.*/
    int id;
    /** The lowest index this thread goes in the array.*/
    int low;
    /** The highest index this thread goes in the array.*/
    int high;
    /** The array to be sorted.*/
    int[] array;

    /**
     * Construct a new thread, giving it a class reference containing the sorting
     * routine as well as the array to be sorted.
     * 
     * @param proj6 Pass the reference to the class containing necessary sorting routine.
     * @param id The ID of this thread.
     * @param low The lowest index this thread goes in the array.
     * @param high The highest index this thread goes in the array.
     * @param array The array to be sorted.
     */
    public QuickSortThread (Project6 proj6,
                            int id,
                            int low,
                            int high,
                            int[] array)
    {
        this.proj6 = proj6;
        this.id = id;
        this.low = low;
        this.high = high;
        this.array = array;
    }

    /**
     * Instantiates the thread and performs the sort on the given array for the
     * specified portions.
     */
    @Override
    public void run()
    {
        proj6.sort (array, low, high);
        proj6.threadFinished (id);
    }
}
