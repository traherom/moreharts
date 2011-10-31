package raim;

import java.awt.Color;
import java.awt.Rectangle;
import java.sql.SQLException;
import java.sql.Time;
import java.text.DateFormat;
import java.util.Collections;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.Vector;
import javax.swing.JPanel;
import javax.swing.JTextPane;
import javax.swing.text.AttributeSet;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.StyleConstants;
import javax.swing.text.StyleContext;

/**
 * A chat tab panel for the RAIM client.
 *
 * @author Alex Laird
 * @author Ryan Morehart
 * @version 0.1
 */
public class RAIMChatTabPanel extends JPanel
{
    RAIMClientAPI api;
    RAIMPanel mainPanel;
    String myUsername;
    String chattingWithUsername;

    /**
     * Create a new chat tab.
     * 
     * @param dbConnection A reference to the database connection.
     * @param mainPanel A reference to the main panel.
     * @param myUsername The local users username.
     * @param chattingWithUsername The username to chat with.
     */
    public RAIMChatTabPanel(RAIMClientAPI api,
            RAIMPanel mainPanel,
            String myUsername,
            String chattingWithUsername)
    {
        initComponents();
        this.mainPanel = mainPanel;
        this.api = api;
        this.myUsername = myUsername;
        this.chattingWithUsername = chattingWithUsername;
    }

    /**
     * If messages already exist for the user, add them to the chat tab.
     *
     * @param allMessages A list of all messages already needing to be displayed.
     */
    public void setPastMessagesText(Vector<RAIMChatMessage> allMessages)
    {
        messageHistoryTextPane.setEditable(true);
        messageHistoryTextPane.setText("");

        // Sort the messages first based on their timestamps
        Collections.sort(allMessages);

        for (int i = 0 ; i < allMessages.size() ; i++)
        {
            Date adjustedTime = adjustForTimezone(allMessages.get(i).timestamp());

            String timeString = DateFormat.getTimeInstance().format(adjustedTime);

            String messageText = timeString + " - " + allMessages.get(i).message() + "\n";

            if (allMessages.get(i).sender().equals(myUsername))
            {
                append(Color.blue, StyleConstants.ALIGN_LEFT, false, messageText, messageHistoryTextPane);
            }
            else
            {
                append(Color.red, StyleConstants.ALIGN_RIGHT, allMessages.get(i).brandNewMessage(),  messageText, messageHistoryTextPane);
            }
        }
        messageHistoryTextPane.setEditable(false);
        typingTextArea.requestFocus();
        messageHistoryTextPane.scrollRectToVisible(
            new Rectangle(0, messageHistoryTextPane.getHeight() - 2, 1, 1));

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        chatTabPanel = new javax.swing.JPanel();
        typingScrollArea = new javax.swing.JScrollPane();
        typingTextArea = new javax.swing.JTextArea();
        sendButton = new javax.swing.JButton();
        messageHistoryScrollArea = new javax.swing.JScrollPane();
        messageHistoryTextPane = new javax.swing.JTextPane();

        setOpaque(false);
        setLayout(null);

        chatTabPanel.setOpaque(false);
        chatTabPanel.setLayout(new org.netbeans.lib.awtextra.AbsoluteLayout());

        typingScrollArea.setHorizontalScrollBarPolicy(javax.swing.ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);

        typingTextArea.setColumns(20);
        typingTextArea.setLineWrap(true);
        typingTextArea.setRows(2);
        typingTextArea.setWrapStyleWord(true);
        typingTextArea.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                typingTextAreaKeyTyped(evt);
            }
        });
        typingScrollArea.setViewportView(typingTextArea);

        chatTabPanel.add(typingScrollArea, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 210, 350, 50));

        sendButton.setText("Send");
        sendButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                sendButtonActionPerformed(evt);
            }
        });
        chatTabPanel.add(sendButton, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 270, -1, -1));

        messageHistoryScrollArea.setHorizontalScrollBarPolicy(javax.swing.ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
        messageHistoryScrollArea.setAutoscrolls(true);

        messageHistoryTextPane.setEditable(false);
        messageHistoryTextPane.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                messageHistoryTextPaneMouseClicked(evt);
            }
        });
        messageHistoryScrollArea.setViewportView(messageHistoryTextPane);

        chatTabPanel.add(messageHistoryScrollArea, new org.netbeans.lib.awtextra.AbsoluteConstraints(10, 10, 350, 190));

        add(chatTabPanel);
        chatTabPanel.setBounds(0, 0, 370, 300);
    }// </editor-fold>//GEN-END:initComponents

    private Date adjustForTimezone(Date timestamp)
    {
        Date adjustedTimestamp = (Date)timestamp.clone();
        GregorianCalendar calendar = new GregorianCalendar();
        int offset = calendar.getTimeZone().getRawOffset();

        int adjustAmount = (offset / 3600000) + 5;

        adjustedTimestamp.setTime(timestamp.getTime() + (adjustAmount * 3600000));

        return adjustedTimestamp;
    }

    private void sendTextMessage()
    {
        if(typingTextArea.getText().replaceAll(" ", "").equals("\n"))
        {
            typingTextArea.setText("");
            return;
        }

		if(mainPanel.getChatters().contains(chattingWithUsername))
		{
			api.send(chattingWithUsername, typingTextArea.getText().trim());
			mainPanel.addMessageFromUser(myUsername, chattingWithUsername, typingTextArea.getText().trim());
		}
		else
		{
			mainPanel.addMessageFromUser(myUsername, chattingWithUsername, chattingWithUsername
					+ " has logged out - the last message was not sent");
		}
        typingTextArea.setText("");
    }

    /**
     * Append to the current message list.
     *
     * @param color The color of the text to append.
     * @param alignement The alignment of the new text.
     * @param boldMessage True if the text should be bold, false otherwise.
     * @param message The message to append.
     * @param pane The text pane to add the message to.
     */
    public void append(Color color,
            int alignement,
            boolean boldMessage,
            String message,
            JTextPane pane)
    {
        StyleContext sc = StyleContext.getDefaultStyleContext();
        AttributeSet aset = sc.addAttribute(SimpleAttributeSet.EMPTY,
                                            StyleConstants.Foreground,
                                            color);

        SimpleAttributeSet as = new SimpleAttributeSet();
        StyleConstants.setAlignment(as, alignement);
        StyleConstants.setBold(as, boldMessage);

        int len = pane.getDocument().getLength(); // same value as getText().length();
        pane.setCaretPosition(len);  // place caret at the end (with no selection)
        pane.setCharacterAttributes(aset, false);
        pane.setParagraphAttributes(as, false);
        pane.replaceSelection(message); // there is no selection, so inserts at caret
  }

    private void sendButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_sendButtonActionPerformed
        sendTextMessage();
        typingTextArea.requestFocus();
    }//GEN-LAST:event_sendButtonActionPerformed

    private void typingTextAreaKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_typingTextAreaKeyTyped
        if (evt.getKeyChar() == '\n')
        {
            sendTextMessage();
        }
    }//GEN-LAST:event_typingTextAreaKeyTyped

    private void messageHistoryTextPaneMouseClicked(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_messageHistoryTextPaneMouseClicked
        typingTextArea.requestFocus();
    }//GEN-LAST:event_messageHistoryTextPaneMouseClicked


    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JPanel chatTabPanel;
    private javax.swing.JScrollPane messageHistoryScrollArea;
    private javax.swing.JTextPane messageHistoryTextPane;
    private javax.swing.JButton sendButton;
    private javax.swing.JScrollPane typingScrollArea;
    protected javax.swing.JTextArea typingTextArea;
    // End of variables declaration//GEN-END:variables

}