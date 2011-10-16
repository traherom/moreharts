
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLConnection;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Random;
import javax.swing.JOptionPane;
import javax.swing.Timer;

public class UserInterface extends javax.swing.JApplet
{
	private Recorder person;
	private Random rand;
	private boolean isTestRunning = false;
	private Light currentLight;
	private Timer delay;
	private int totalLights;
	private int lightNumber;
	private Recorder.Side coveredEye;
	private String saveURL = "http://www.traherom.com/reaction/adddata";

	@Override
	public void init()
	{
		try
		{
			// Create generator
			rand = new Random();
			rand.setSeed(System.nanoTime());

			java.awt.EventQueue.invokeAndWait(new Runnable()
			{
				public void run()
				{
					initComponents();
				}
			});
		}
		catch(Exception ex)
		{
			ex.printStackTrace();
		}
	}

	@SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        nameTextBox = new javax.swing.JTextField();
        genderCB = new javax.swing.JComboBox();
        jLabel1 = new javax.swing.JLabel();
        jLabel2 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        ageSpinner = new javax.swing.JSpinner();
        startButton = new javax.swing.JButton();
        lightDrawer = new LightDrawerPanel();
        testFeedback = new javax.swing.JLabel();
        jScrollPane = new javax.swing.JScrollPane();
        testResults = new javax.swing.JTextArea();
        jLabel5 = new javax.swing.JLabel();
        dominateHandCB = new javax.swing.JComboBox();
        jPanel1 = new javax.swing.JPanel();
        jPanel2 = new javax.swing.JPanel();
        jPanel3 = new javax.swing.JPanel();

        addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                formMouseClicked(evt);
            }
        });
        addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                formKeyPressed(evt);
            }
        });
        getContentPane().setLayout(new java.awt.GridBagLayout());

        nameTextBox.setPreferredSize(new java.awt.Dimension(150, 20));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(nameTextBox, gridBagConstraints);

        genderCB.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Female", "Male" }));
        genderCB.setActionCommand("genderChanged");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(genderCB, gridBagConstraints);

        jLabel1.setText("Name");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
        getContentPane().add(jLabel1, gridBagConstraints);

        jLabel2.setText("Gender");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
        getContentPane().add(jLabel2, gridBagConstraints);

        jLabel3.setText("Age");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
        getContentPane().add(jLabel3, gridBagConstraints);

        ageSpinner.setModel(new javax.swing.SpinnerNumberModel(19, 1, 100, 1));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        getContentPane().add(ageSpinner, gridBagConstraints);

        startButton.setText("Start Test");
        startButton.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                startButtonActionPerformed(evt);
            }
        });
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(startButton, gridBagConstraints);

        lightDrawer.setMaximumSize(new java.awt.Dimension(800, 375));
        lightDrawer.setMinimumSize(new java.awt.Dimension(800, 375));
        lightDrawer.setPreferredSize(new java.awt.Dimension(800, 375));
        lightDrawer.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                UserInterface.this.mouseClicked(evt);
            }
        });

        javax.swing.GroupLayout lightDrawerLayout = new javax.swing.GroupLayout(lightDrawer);
        lightDrawer.setLayout(lightDrawerLayout);
        lightDrawerLayout.setHorizontalGroup(
            lightDrawerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 800, Short.MAX_VALUE)
        );
        lightDrawerLayout.setVerticalGroup(
            lightDrawerLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 375, Short.MAX_VALUE)
        );

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 6;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTHWEST;
        gridBagConstraints.insets = new java.awt.Insets(2, 2, 2, 2);
        getContentPane().add(lightDrawer, gridBagConstraints);

        testFeedback.setText("Test Stopped");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.gridwidth = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.HORIZONTAL;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(testFeedback, gridBagConstraints);

        testResults.setEditable(false);
        testResults.setTabSize(4);
        jScrollPane.setViewportView(testResults);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 6;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.NORTH;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(jScrollPane, gridBagConstraints);

        jLabel5.setText("Dominate Hand");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.EAST;
        getContentPane().add(jLabel5, gridBagConstraints);

        dominateHandCB.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "Left", "Right" }));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 2;
        gridBagConstraints.gridy = 3;
        gridBagConstraints.anchor = java.awt.GridBagConstraints.WEST;
        gridBagConstraints.insets = new java.awt.Insets(1, 2, 1, 2);
        getContentPane().add(dominateHandCB, gridBagConstraints);

        javax.swing.GroupLayout jPanel1Layout = new javax.swing.GroupLayout(jPanel1);
        jPanel1.setLayout(jPanel1Layout);
        jPanel1Layout.setHorizontalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        jPanel1Layout.setVerticalGroup(
            jPanel1Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 8;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.5;
        getContentPane().add(jPanel1, gridBagConstraints);

        javax.swing.GroupLayout jPanel2Layout = new javax.swing.GroupLayout(jPanel2);
        jPanel2.setLayout(jPanel2Layout);
        jPanel2Layout.setHorizontalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );
        jPanel2Layout.setVerticalGroup(
            jPanel2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 0, Short.MAX_VALUE)
        );

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 4;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridheight = 8;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.5;
        getContentPane().add(jPanel2, gridBagConstraints);

        javax.swing.GroupLayout jPanel3Layout = new javax.swing.GroupLayout(jPanel3);
        jPanel3.setLayout(jPanel3Layout);
        jPanel3Layout.setHorizontalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 804, Short.MAX_VALUE)
        );
        jPanel3Layout.setVerticalGroup(
            jPanel3Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 220, Short.MAX_VALUE)
        );

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 7;
        gridBagConstraints.gridwidth = 3;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weighty = 1.0;
        getContentPane().add(jPanel3, gridBagConstraints);
    }// </editor-fold>//GEN-END:initComponents

	private void startButtonActionPerformed(java.awt.event.ActionEvent evt)//GEN-FIRST:event_startButtonActionPerformed
	{//GEN-HEADEREND:event_startButtonActionPerformed
		try
		{
			// Disable properties for test
			startButton.setEnabled(false);
			startButton.setText("Test running...");
			ageSpinner.setEnabled(false);
			genderCB.setEnabled(false);
			dominateHandCB.setEnabled(false);
			nameTextBox.setEnabled(false);

			// Initialize the person
			person = new Recorder();
			person.setName(nameTextBox.getText());
			person.setAge((Integer)ageSpinner.getValue());

			person.setDominateEye(Recorder.Side.Unspecified);

			String hand = dominateHandCB.getSelectedItem().toString();
			person.setDominateHand(Enum.valueOf(Recorder.Side.class, hand));

			String gender = genderCB.getSelectedItem().toString();
			person.setGender(Enum.valueOf(Recorder.Gender.class, gender));

			// Initialize test
			// Set starting eye covering to be none and tell the user to do it
			coveredEye = Recorder.Side.Unspecified;
			JOptionPane.showMessageDialog(this,
										  "For this part of the test, keep both eyes open.\nWhen you see a white light appear in the black, click anywhere in the black.",
										  "Test instructions",
										  JOptionPane.INFORMATION_MESSAGE);
			lightNumber = 1;
			totalLights = 30;

			// And start test
			isTestRunning = true;
			delay = new Timer(rand.nextInt(3000) + 1000,
							  new java.awt.event.ActionListener()
			{
				public void actionPerformed(java.awt.event.ActionEvent evt)
				{
					// Ready timer for next time
					delay.stop();
					delay.setDelay((rand.nextInt(3000) + 1000));
					System.out.println(delay.getDelay());

					// And give the user some feed back
					testFeedback.setText("Test running: " + lightNumber + "/" + totalLights + " lights");

					// Create new random light
					// Subtraction of size is to ensure it is fully visible on the screen
					currentLight = new Light();
					currentLight.setSize(20);
					currentLight.setScreenSize(lightDrawer.getHeight(),
											   lightDrawer.getWidth());
					currentLight.setPosition(rand.nextInt(lightDrawer.getWidth() - currentLight.getSize()),
											 rand.nextInt(lightDrawer.getHeight() - currentLight.getSize()));
					currentLight.setEye(coveredEye);

					// Draw on canvas
					lightDrawer.setLight(currentLight);
					lightDrawer.repaint();
				}
			});
			delay.setRepeats(false);
			delay.start();

			repaint();
		}
		catch(Exception ex)
		{
			System.err.println(ex);
		}
	}//GEN-LAST:event_startButtonActionPerformed

	private boolean saveResults() throws UnsupportedEncodingException, Exception
	{
		// Dump to box for person to view if they want
		testResults.setText(person.toString());

		// Create data string
		StringBuilder sb = new StringBuilder();
		sb.append("applet_id=slightfun");

		// Main person data
		sb.append("&name=").append(URLEncoder.encode(person.getName(), "UTF-8"));
		sb.append("&age=").append(URLEncoder.encode(((Integer)person.getAge()).toString(), "UTF-8"));
		sb.append("&gender=" + URLEncoder.encode(person.getGender().toString(), "UTF-8"));
		sb.append("&dominateEye=" + URLEncoder.encode(person.getDominateEye().toString(), "UTF-8"));
		sb.append("&dominateHand=" + URLEncoder.encode(person.getDominateHand().toString(), "UTF-8"));

		// Each light
		ArrayList<Light> lights = person.getLights();
		for(int i = 0; i < lights.size(); i++)
		{
			String lightNum = "&lights[" + i + "]";
			sb.append(lightNum).append("[eye]=").append(lights.get(i).getEye());
			sb.append(lightNum).append("[time]=").append(lights.get(i).getTime());
			sb.append(lightNum).append("[size]=").append(lights.get(i).getSize());
			sb.append(lightNum).append("[x]=").append(lights.get(i).getX());
			sb.append(lightNum).append("[y]=").append(lights.get(i).getY());
			sb.append(lightNum).append("[height]=").append(lights.get(i).getScreenHeight());
			sb.append(lightNum).append("[width]=").append(lights.get(i).getScreenWidth());
		}

		// Send to server
		boolean success = true;
		URL url = new URL(saveURL);
		URLConnection conn = url.openConnection();
		conn.setDoOutput(true);
		OutputStreamWriter wr = new OutputStreamWriter(conn.getOutputStream());
		wr.write(sb.toString());
		wr.flush();

		// Get response
		// Get the response
		// If the server just tells us "success," then it worked
		BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
		String line;
		while((line = rd.readLine()) != null)
		{
			System.out.println(line);
			if(!line.equals("success"))
				success = false;
		}
		wr.close();
		rd.close();
		
		return success;
	}

	private void respondToLight()
	{
		if(!isTestRunning)
			return;

		// Are we actually running?
		if(currentLight != null)
		{
			// Save light
			currentLight.stopTime();
			person.recordLight(currentLight);

			// Erase from display
			currentLight = null;
			lightDrawer.removeLight();
			lightDrawer.repaint();

			if(lightNumber < totalLights)
			{
				// Should we advance to the next part of the test?
				if(lightNumber % 10 == 0)
				{
					if(coveredEye == Recorder.Side.Unspecified)
					{
						coveredEye = Recorder.Side.Left;
						JOptionPane.showMessageDialog(this,
													  "Now close or cover your left eye.\nClick OK to continue the test.",
													  "Test instructions",
													  JOptionPane.INFORMATION_MESSAGE);
					}
					else
					{
						coveredEye = Recorder.Side.Right;
						JOptionPane.showMessageDialog(this,
													  "Finally, close your right eye and use your left to watch for the lights.\nClick OK to continue the test.",
													  "Test instructions",
													  JOptionPane.INFORMATION_MESSAGE);
					}
				}

				// Start timer for next light
				lightNumber++;
				delay.start();
			}
			else
			{
				// All done with test
				isTestRunning = false;

				try
				{
					// Save/show results
					testFeedback.setText("Test complete. Saving results...");
					if(saveResults())
					{
						testFeedback.setText("Test complete. Results shown.");
						JOptionPane.showMessageDialog(this,
													  "Test complete. Your results have been successfully submitted.",
													  "Test instructions",
													  JOptionPane.INFORMATION_MESSAGE);
					}
					else
					{
						JOptionPane.showMessageDialog(this,
													  "Test complete, but failed to save results to server.\nPlease email results in the pane to the right to morehart@gmail.com",
													  "Failed to save",
													  JOptionPane.ERROR_MESSAGE);
					}
				}
				catch(UnsupportedEncodingException ex)
				{
					System.err.println("Unsupported encoding? What?");
				}
				catch(Exception ex)
				{
					System.err.println(ex);
				}

				// Allow test to be run again
				startButton.setText("Start Test");
				startButton.setEnabled(true);
				ageSpinner.setEnabled(true);
				genderCB.setEnabled(true);
				dominateHandCB.setEnabled(true);
				nameTextBox.setEnabled(true);
			}
		}
		else
		{
			// They jumped the gun, make them wait more
			delay.restart();
		}
	}

	private void formKeyPressed(java.awt.event.KeyEvent evt)//GEN-FIRST:event_formKeyPressed
	{//GEN-HEADEREND:event_formKeyPressed
		respondToLight();
	}//GEN-LAST:event_formKeyPressed

	private void mouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_mouseClicked
	{//GEN-HEADEREND:event_mouseClicked
		respondToLight();
	}//GEN-LAST:event_mouseClicked

	private void formMouseClicked(java.awt.event.MouseEvent evt)//GEN-FIRST:event_formMouseClicked
	{//GEN-HEADEREND:event_formMouseClicked
		respondToLight();
	}//GEN-LAST:event_formMouseClicked
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JSpinner ageSpinner;
    private javax.swing.JComboBox dominateHandCB;
    private javax.swing.JComboBox genderCB;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JPanel jPanel3;
    private javax.swing.JScrollPane jScrollPane;
    private LightDrawerPanel lightDrawer;
    private javax.swing.JTextField nameTextBox;
    private javax.swing.JButton startButton;
    private javax.swing.JLabel testFeedback;
    private javax.swing.JTextArea testResults;
    // End of variables declaration//GEN-END:variables
}
