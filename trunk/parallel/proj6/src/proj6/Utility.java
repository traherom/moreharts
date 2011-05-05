package proj6;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Arrays;
import java.util.Random;

/**
 * This class contains common, simple helper functions that are globally
 * available to any class that needs them.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 */
public class Utility
{
    /** The number formatter for pretty output.*/
    public static final NumberFormat NUM_FORMAT = new DecimalFormat ("###,###");
    
    /**
     * Builds an array of given size with random values (if random is true) between
     * 0 (inclusive) and MAX_VALUE.  If random is set to false, values are seeded to
     * the indeces of the array.
     * 
     * @param size The size to build the array.
     * @param random True if values should be random, false if values should be seeded.
     * @param MAX_VALUE The maximum value of a randomly generated number.
     * @return The newly built array.
     */
    public static int[] buildArray (int size,
                                    boolean random,
                                    final int MAX_VALUE)
    {
        int[] array = new int[size];
        Random randomizer = new Random ();

        // Loop through and set all array values
        for (int i = 0; i < size; ++i)
        {
            // If random, set value between 0 (inclusive) and MAX_VALUE
            if (random)
            {
                array[i] = randomizer.nextInt(MAX_VALUE);
            }
            // Not random, so set value to the index of the array
            else
            {
                array[i] = i;
            }
        }

        return array;
    }

    /**
     * Verify that the two arrays are equal by sorting the unsorted array, using
     * verified Java sorting libraries, and comparing the two arrays.
     *
     * @param sortedArray The sorted array.
     * @param unsortedArray The unsorted array to be sorted and compared to the sorted array.
     * @return True if the arrays are equal after sorting using Java sorting libraries, false if there was an inconsistency.
     */
    public static boolean verifyArray(int[] sortedArray,
                                      int[] unsortedArray)
    {
        Arrays.sort (unsortedArray);
        for (int i = 0; i < sortedArray.length ;++i)
        {
            if (sortedArray[i] != unsortedArray[i])
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Print the given array.
     *
     * @param array The array to be printed.
     */
    public static void printArray(int[] array)
    {
        for (int i = 0; i < array.length; ++i)
        {
            System.out.print (array[i]);
            if (i < array.length - 1)
            {
                System.out.print (", ");
            }
        }
        System.out.println ();
    }
}
