package raim;

import java.sql.SQLException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JApplet;

/**
 * The container for the applet-based application.
 *
 * @author Alex Laird
 * @autho Ryan Morehart
 * @version 0.1
 */
public class RAIMApplet extends JApplet
{
    /** The panel that is added to the frame.*/
    private RAIMPanel mainPanel;

    /**
     * Construct the applet prior actually starting it.
     */
    @Override
    public void init()
    {
        initComponents();
        mainPanel = new RAIMPanel(this);
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setName("mainApplet"); // NOI18N
        getContentPane().setLayout(new java.awt.GridLayout(1, 1));
    }// </editor-fold>//GEN-END:initComponents

    /**
     * Adds the panel to the applet and sets it visible.
     */
    @Override
    public void start()
    {
		try
		{
			getContentPane().add(mainPanel);
			mainPanel.setVisible(true);
			mainPanel.connectToServer();
		}
		catch(RAIMException ex)
		{
			System.out.println(ex.getMessage());
		}
    }

    /**
     * Logout and close connections before allowing stopping the applet.
     */
    @Override
    public void stop()
    {
        mainPanel.getAPI().logout();
    }

    /**
     * Unused by the applet.
     */
    @Override
    public void destroy() {}

    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables

}