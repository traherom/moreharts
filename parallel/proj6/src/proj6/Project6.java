package proj6;

import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Implements QuickSort three different ways:
 * 1. Perform the basic, sequential QuickSort algorithm with two-pointer
 *    partition.
 * 2. Perform QuickSort using four threads, explicitely created and invoked.
 * 3. Perform QuickSort on numThreads using Executor support from Java's
 *    concurrency libraries.
 *
 * The program can be executed with the following command:
 * Usage: java Project6 <part> [OPTIONS]
 * <part> - Valid parts are 1, 2, or 3
 *
 * Available options:
 * -s, --size    The size of the array to generate
 * -r, --random  True indicates a random array, false indicates a seeded array
 * -m, --max     Random numbers will generated between 0 (inclusive) and
 *               maxValue
 * -n, --num     Must be specified if you are desiring to run Part 3, otherwise
 *               it is ignored.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class Project6
{
    /** The largest partition size that a task will execute for Part 3.*/
    private final int LARGEST_SIZE = 1000;
    /** Our processor ID if we are running in parallel. If the user is not running Part 2 or Part 3, this is left at -1.*/
    private int id = -1;
    /** In parallel algorithms, the number of threads that have completed their tasks.*/
    private int finishedThreads = 0;
    /** In parallel algorithms, the number of numbers that have been sorted.*/
    private int finishedCount = 0;
    /** The current number of tasks in use.*/
    private int taskCount = 0;
    /** Executor service for use in Part 3.*/
    ExecutorService pool;
    /** The number of threads to use if performing Part 2 or Part 3.*/
    private int numThreads = -1;

    /**
     * Constructs the class object with the number of threads used, if running Part 2 or Part 3.
     *
     * @param numThreads Number of threads to use.
     */
    public Project6(int numThreads)
    {
        this.numThreads = numThreads;
    }

    /**
     * Increments the variable stating the number of threads finished.
     */
    protected synchronized void threadFinished(int id)
    {
        ++finishedThreads;
    }

    /**
     * Check if the number of slave threads reporting they have finished working
     * is equal to the number of slave threads spawned.
     *
     * @return True if all spawned threads have finished, false otherwise.
     */
    private synchronized boolean waitingForThreads()
    {
        if (finishedThreads == numThreads)
        {
            return true;
        }
        return false;
    }

    /**
     * Check if the number of sorted items equals the total number of elements
     * in the array.
     *
     * @param size The number of elements total in the array.
     * @return True if all elements have been sorted, false otherwise.
     */
    private synchronized boolean waitingForCount(int size)
    {
        if (getFinishedCount () != size)
        {
            return true;
        }
        return false;
    }

    /**
     * Increment the finished counter by the given number n.
     *
     * @param n The amount to increment the finished counter by.
     */
    protected synchronized void addToFinishedCount (int n)
    {
        finishedCount += n;
    }
    
    /**
     * Retrieve the number of finished threads.
     *
     * @return The number of finished threads.
     */
    private synchronized int getFinishedCount()
    {
        return finishedCount;
    }

    /**
     * Increment the current number of tasks in use by one.
     */
    protected synchronized void incrementTaskCount()
    {
        ++taskCount;
    }

    /**
     * Retrieve the current number of tasks running.
     *
     * @return The number of tasks running.
     */
    protected synchronized int getTaskCount()
    {
        return taskCount;
    }

    /**
     * Swaps the two indeces in the array given.
     *
     * @param array The array to perform the swap on.
     * @param first Swap this index with element at second.
     * @param second Swap this index with element at first.
     */
    public void swap(int[] array,
                     int first,
                     int second)
    {
        int temp = array[first];
        array[first] = array[second];
        array[second] = temp;
    }

    /**
     * Finds the most logical point to split the array in two parts and uses
     * that point as the partition. Uses a while loop to evaluate the partition.
     *
     * @param array The array to be sorted.
     * @param low The lowest index.
     * @param high The highest index.
     * @return The index of the partitioning point.
     */
    protected int partition(int[] array,
                            int low,
                            int high)
    {
        int pivot = array[high];
        int left = low;
        int right = high - 1;

        while (left <= right)
        {
            // Increment the low pointer until you meet the pivot
            while (left <= right &&
                   array[left] <= pivot)
            {
                ++left;
            }
            // Decrement the high pointer until you meet the pivot
            while (left <= right &&
                   array[right] >= pivot)
            {
                --right;
            }

            // If the pointers have crossed, swap the items
            if (left < right)
            {
                // Swap value at left with value at right
                swap(array, left, right);
            }
        }

        // Finally, swap value at left with value at high
        swap (array, left, high);

        return left;
    }

    /**
     * Performs a standard recursive QuickSort algorithm on an array from
     * indeces high to low.
     *
     * @param array The array to be sorted.
     * @param low The lowest index.
     * @param high The highest index.
     */
    protected void sort(int[] array,
                        int low,
                        int high)
    {
        if (low < high)
        {
            // Locate the most precise partition point
            int mid = partition (array, low, high);
            // Recursively sort the lower half
            sort (array, low, mid - 1);
            // Recursively sort the upper half
            sort (array, mid + 1, high);
        }
    }

    /**
     * Continue passing partitions out to threads until the partition size is small
     * enough for us to handle ourselves, backing out of recursion.
     *
     * @param low The lowest index.
     * @param high The highest index.
     * @param array The array to be sorted.
     */
    protected void part3(int low,
                         int high,
                         int[] array)
    {
        if (high - low + 1 < LARGEST_SIZE)
        {
            // The partition is small enough, so do the work ourselves
            sort (array, low, high);
            addToFinishedCount (high - low + 1);
        }
        else
        {
            // Identify partition point
            int mid = partition (array, low, high);
            addToFinishedCount (1);

            // Launch slave task
            pool.execute (new QuickSortTask (this, getTaskCount (), mid + 1, high, array));
            incrementTaskCount ();

            // Recurse to assign the lower half to its thread
            part3(low, mid - 1, array);
        }
    }

    /**
     * Generate the array, sort it using the correct method for the given part,
     * and benchmark.
     *
     * @param part The part of the problem to perform.  Value use be 1, 2, or 3
     * @param size The size of the array to sort.
     * @param random True if numbers should be generated randomly, false if they should follow a pattern
     * @param maxValue The maximum value a randomly generated number can be
     */
    private void go(int part,
                    int size,
                    boolean random,
                    int maxValue)
    {
        // Generate array, output if length is reasonable, and declare time variables before
        // starting the time count, so as to not effect performance
        int[] array = Utility.buildArray (size, random, maxValue);
        // Copy the array to an unsorted array location for verification after the fact
        int[] unsortedArray = Arrays.copyOf (array, array.length);
        System.out.println ("::Part " + part + "::");
        System.out.println ("Array size: " + Utility.NUM_FORMAT.format (size));
        if (array.length <= 25)
        {
            System.out.print ("Initial array: ");
            Utility.printArray (array);
        }
        
        long startTime = -1;
        long endTime = -1;

        // Mark beginning time for calculation later
        startTime = System.currentTimeMillis();

        // Perform the basic, sequential QuickSort algorithm with two-pointer partition
        if (part == 1)
        {
            sort (array, 0, array.length - 1);
        }
        // Perform QuickSort using four threads, explicitely created and invoked
        else if (part == 2)
        {
            // Set this processor's ID
            id = 0;
            // Set the number of slave threads
            numThreads = 3;
            
            int low = 0;
            int high = array.length - 1;
            // Identify partition points for master and slaves
            int mid = partition (array, low, high);
            int first = partition (array, low, mid - 1);
            int second = partition (array, mid + 1, high);

            // Instantiate slave threads and pass them their partition portions
            QuickSortThread thread2 = new QuickSortThread (this, 1, first + 1, mid - 1, array);
            QuickSortThread thread3 = new QuickSortThread (this, 2, mid + 1, second - 1, array);
            QuickSortThread thread4 = new QuickSortThread (this, 3, second + 1, high, array);

            // Launch slave threads
            thread2.start ();
            thread3.start ();
            thread4.start ();
            // Current thread is the first thread, so let it do the lowest part
            sort (array, low, first - 1);

            // Wait for all threads to finish
            while (!waitingForThreads ())
            {
                continue;
            }
        }
        // Perform QuickSort on numThreads using Executor support from Java's concurrency libraries
        else if (part == 3)
        {
            // Set this processor's ID and increment task count
            id = 0;
            incrementTaskCount ();

            // Launch slave task
            pool = Executors.newFixedThreadPool (numThreads);
            pool.execute (new QuickSortTask (this, getTaskCount (), 0, array.length - 1, array));
            incrementTaskCount ();

            while (waitingForCount (array.length))
            {
                continue;
            }
            pool.shutdown();
        }

        // Mark end time and calculate total runtime
        endTime = System.currentTimeMillis();
        long totalTime = endTime - startTime;

        // Output benchmark as well as results, if length is reasonable
        if (part == 2)
        {
            System.out.println ("Threads used: 4");
        }
        else if(part == 3)
        {
            System.out.println ("Tasks used: " + numThreads);
        }
        System.out.println ("Elapsed time: " + Utility.NUM_FORMAT.format (totalTime) + "ms");
        if (array.length <= 25)
        {
            System.out.print ("Sorted array: ");
            Utility.printArray (array);
        }
        if (Utility.verifyArray (array, unsortedArray))
        {
            System.out.println ("Verified: array sorted properly");
        }
        else
        {
            System.out.println ("Unverified: array did not sort properly");
        }
        System.out.println ();
    }

    /**
     * The main method executes first in the program, parsing command-line arguments
     * and then immediately leaving static-land for the actual algorithms and
     * computations.
     *
     * @param args The command-line arguments.  "Usage: java Project6 <part> [OPTIONS]
     * <part> - Valid parts are 1, 2, or 3
     *
     * Available options:
     * -s, --size    The size of the array to generate
     * -r, --random  True indicates a random array, false indicates a seeded array
     * -m, --max     Random numbers will generated between 0 (inclusive) and maxValue
     * -n, --num     Must be specified if you are desiring to run Part 3, otherwise it is ignored.
     */
    public static void main(String[] args)
    {
        // Output string for common errors
        final String USAGE_TEXT = "Usage: java Project6 <part> [OPTIONS]\n"
        + "<part> - Valid parts are 1, 2, or 3\n\n"
        + "Available options:\n"
        + "-s, --size      The size of the array to generate\n"
        + "-r, --random    True indicates a random array, false indicates a seeded array\n"
        + "-m, --max       Random numbers will generated between 0 (inclusive) and maxValue\n"
        + "-n, --num       Must be specified if you are desiring to run Part 3, otherwise it is ignored.";
        // The part of the problem to perform.  Value use be 1, 2, or 3
        int part = -1;
        // The size of the array to sort.*/
        int size = 10;
        // True if numbers should be generated randomly, false if they should follow a pattern
        boolean random = false;
        // The maximum value a randomly generated number can be
        int maxValue = 10;
        // The number of threads to use if performing Part 3
        int numThreads = -1;
        
        // Parse through arguments and set their respective variables
        if (args.length > 0)
        {
            part = Integer.parseInt (args[0]);

            try
            {
                for (int i = 1; i < args.length; i += 2)
                {
                    if (args[i].equals ("-s"))
                    {
                        size = Integer.parseInt (args[i + 1]);
                    }
                    else if(args[i].equals ("-r"))
                    {
                        if (args[i + 1].equals ("true"))
                        {
                            random = true;
                        }
                        else if (!args[i + 1].equals ("false"))
                        {
                            int intRand = Integer.parseInt (args[i + 1]);
                            if (intRand == 1)
                            {
                                random = true;
                            }
                            else if (intRand != 0)
                            {
                                // Since the given random value was not valid at this point, just break the given part so the program termiantes
                                part = -1;
                            }
                        }
                    }
                    else if(args[i].equals("-m"))
                    {
                        maxValue = Integer.parseInt(args[i + 1]);
                    }
                    else if(args[i].equals("-n"))
                    {
                        numThreads = Integer.parseInt(args[i + 1]);
                    }
                }
            }
            catch (NumberFormatException ex)
            {
                System.out.println (USAGE_TEXT);
                System.exit (2);
            }

            // Make sure the given part number was valid
            if (part > 3 || part < 1)
            {
                System.out.println (USAGE_TEXT);
                System.exit (3);
            }

            // If we are doing Part 3 and the number of desired threads was not given, we cannot continue
            if (part == 3 && numThreads == -1)
            {
                System.out.println (USAGE_TEXT);
                System.exit (4);
            }
        }
        else
        {
            System.out.println (USAGE_TEXT);
            System.exit (1);
        }

        // Get us out of static-land!
        Project6 proj6 = new Project6 (numThreads);
        proj6.go (part, size, random, maxValue);
    }
}
