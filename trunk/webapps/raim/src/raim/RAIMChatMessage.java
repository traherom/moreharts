package raim;

import java.sql.Time;

/**
 * Chat message record between two users.
 * 
 * @author Alex Laird
 * @author Ryan Morehart
 * @version 0.1
 */
public class RAIMChatMessage implements Comparable
{
    private String sender;
    private String message;
    private Time timestamp;
    private boolean brandNewMessage;

    public RAIMChatMessage(String sender,
            String message,
            boolean brandNewMessage)
    {
        this.sender = sender;
        this.message = message;
        this.timestamp = new Time(System.currentTimeMillis());
        this.brandNewMessage = brandNewMessage;
    }

    public String sender()
    {
        return sender;
    }

    public String message()
    {
        return message;
    }

    public Time timestamp()
    {
        return timestamp;
    }

    public boolean brandNewMessage()
    {
        return brandNewMessage;
    }

    public void resetNewMessage()
    {
        brandNewMessage = false;
    }

    public int compareTo(Object o)
    {
        int toReturn = -1;
        
        RAIMChatMessage chatMessage = (RAIMChatMessage) o;

        toReturn = this.timestamp.compareTo(chatMessage.timestamp);

        return toReturn;
    }
}
