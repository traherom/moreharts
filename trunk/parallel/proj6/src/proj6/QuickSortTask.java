package proj6;

/**
 * A task that will sort a set array upon execution.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class QuickSortTask implements Runnable
{
    /** The back-reference to the class containing the sorting routine.*/
    Project6 proj6;
    /** The ID of this task.*/
    int id;
    /** The lowest index this task goes in the array.*/
    int low;
    /** The highest index this task goes in the array.*/
    int high;
    /** The array to be sorted.*/
    int[] array;

    /**
     * Construct a new task, giving it a class reference containing the sorting
     * routine as well as the array to be sorted.
     *
     * @param proj6 Pass the reference to the class containing necessary sorting routine.
     * @param id The ID of this task.
     * @param low The lowest index this task goes in the array.
     * @param high The highest index this task goes in the array.
     * @param array The array to be sorted.
     */
    public QuickSortTask (Project6 proj6,
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
     * Instantiates the task and performs the sort on the given array for the
     * specified portions.
     */
    @Override
    public void run()
    {
        proj6.part3 (low, high, array);
    }
}
