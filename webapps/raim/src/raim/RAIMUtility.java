package raim;

/**
 * Contains static utility methods that can be used at any time by any class.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 * @version 0.1
 */
public class RAIMUtility
{
    /**
     * Takes a byte array and converts each byte to a hex value and stringifies
     * it.
     * 
     * @param b The byte array
     * @return A string representation of the hex values of the byte array.
     */
    public static String byteArrayToHexString(byte[] b)
    {
        StringBuffer sb = new StringBuffer(b.length * 2);

        for (int i = 0; i < b.length; i++)
        {
            int v = b[i] & 0xff;

            if (v < 16)
            {
                sb.append('0');
            }

            sb.append(Integer.toHexString(v));
        }

        return sb.toString().toUpperCase();
    }
}
