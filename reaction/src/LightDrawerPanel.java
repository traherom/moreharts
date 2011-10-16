
import java.awt.Graphics;

public class LightDrawerPanel extends javax.swing.JPanel
{
	Light currLight;

	public LightDrawerPanel()
	{
		initComponents();
	}

	public void setLight(Light newLight)
	{
		currLight = newLight;
	}

	public void removeLight()
	{
		currLight = null;
	}

	public Light getLight()
	{
		return currLight;
	}

	@Override
	public void paintComponent(Graphics g)
	{
		super.paintComponent(g);

		if(currLight != null)
		{
			currLight.startTime();
			g.fillOval(currLight.getX(), currLight.getY(),
					   currLight.getSize(), currLight.getSize());
		}
	}

	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        setBackground(new java.awt.Color(0, 0, 0));
        setForeground(new java.awt.Color(255, 255, 255));

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 400, Short.MAX_VALUE)
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 300, Short.MAX_VALUE)
        );
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
}
