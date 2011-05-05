package raim;

import java.util.Vector;

/**
 * Chat session record between two users.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 * @version 0.1
 */
public class RAIMChatSession
{
    private boolean messageOutstanding;
    private String chattingWith;
    private Vector<RAIMChatMessage> allMessages;

    public RAIMChatSession(String chattingWith)
    {
        this.chattingWith = chattingWith;
        messageOutstanding = false;
        allMessages = new Vector<RAIMChatMessage>();
    }

    public boolean messageOutstanding()
    {
        return messageOutstanding;
    }

    public String chattingWith()
    {
        return chattingWith;
    }

    public void addMessage(RAIMChatMessage message)
    {
        messageOutstanding = true;

        allMessages.add(message);
    }

    public Vector<RAIMChatMessage> getAllMessages()
    {
        messageOutstanding = false;
        return allMessages;
    }

    public void resetNewMessages()
    {
        for (int i = 0 ; i < allMessages.size() ; i++)
        {
            allMessages.get(i).resetNewMessage();
        }
    }
}
