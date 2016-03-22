import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.StringTokenizer;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class ScaleAdjuster extends JFrame implements ActionListener
{
    private JPanel minPanel,maxPanel,chanPanel;
    private JLabel minLabel,maxLabel,chanHeader;
    private JLabel[] channelLabels;
    private JTextField[] minFields, maxFields;
    private JButton updateButton,autoscaleButton,closeButton;
    private ImagePanel imPanel;
    
    public ScaleAdjuster(ImagePanel ip)
    {
        super();
        //setBounds(100,100,400,400);
        imPanel = ip;
        chanPanel = new JPanel();
        minPanel = new JPanel();
        maxPanel = new JPanel();
        chanHeader = new JLabel("Channel");
        minLabel = new JLabel("Min Value");
        maxLabel = new JLabel("Max Value");
        chanPanel.setLayout(new BoxLayout(chanPanel,BoxLayout.Y_AXIS));
        chanPanel.add(chanHeader);
        minPanel.setLayout(new BoxLayout(minPanel,BoxLayout.Y_AXIS));
        minPanel.add(minLabel);
        maxPanel.setLayout(new BoxLayout(maxPanel,BoxLayout.Y_AXIS));
        maxPanel.add(maxLabel);
        if(imPanel.getMode() == ImagePanel.GRAY_8){
            channelLabels = new JLabel[1];
            minFields = new JTextField[1];
            maxFields = new JTextField[1];
            channelLabels[0] = new JLabel("Channel "+imPanel.getWavelength()+":");
            minFields[0] = new JTextField(10);
            minFields[0].setText(""+imPanel.getWPixMin());
            minFields[0].addActionListener(this);
            minFields[0].setActionCommand("min -1");
            maxFields[0] = new JTextField(10);
            maxFields[0].setText(""+imPanel.getWPixMax());
            maxFields[0].addActionListener(this);
            maxFields[0].setActionCommand("max -1");
            chanPanel.add(channelLabels[0]);
            minPanel.add(minFields[0]);
            maxPanel.add(maxFields[0]);
        }
        else{
            int nchan = imPanel.getNChannels();
            channelLabels = new JLabel[nchan];
            minFields = new JTextField[nchan];
            maxFields = new JTextField[nchan];
            for(int i = 0; i < nchan; i++){
                channelLabels[i] = new JLabel("Channel "+imPanel.getChannel(i)+":");
                minFields[i] = new JTextField(10);
                minFields[i].setText(""+imPanel.getCPixMin(i));
                minFields[i].addActionListener(this);
                minFields[i].setActionCommand("min " + i);
                maxFields[i] = new JTextField(10);
                maxFields[i].setText(""+imPanel.getCPixMax(i));
                maxFields[i].addActionListener(this);
                maxFields[i].setActionCommand("max " + i);
                chanPanel.add(channelLabels[i]);
                minPanel.add(minFields[i]);
                maxPanel.add(maxFields[i]);
            }
            channelLabels[0].setText("(R) "+channelLabels[0].getText());
            channelLabels[1].setText("(G) "+channelLabels[1].getText());
            channelLabels[2].setText("(B) "+channelLabels[2].getText());
        }
        updateButton = new JButton("Update");
        updateButton.addActionListener(this);
        updateButton.setActionCommand("update");
        chanPanel.add(updateButton);
        autoscaleButton = new JButton("Auto Scale");
        autoscaleButton.addActionListener(this);
        autoscaleButton.setActionCommand("auto");
        minPanel.add(autoscaleButton);
        closeButton = new JButton("Close");
        closeButton.addActionListener(this);
        closeButton.setActionCommand("close");
        maxPanel.add(closeButton);
        getContentPane().setLayout(new BoxLayout(getContentPane(),BoxLayout.X_AXIS));
        getContentPane().add(chanPanel);
        getContentPane().add(minPanel);
        getContentPane().add(maxPanel);
        setDefaultCloseOperation(DISPOSE_ON_CLOSE);
        pack();
        setVisible(true);
    }
    
    public void actionPerformed(ActionEvent e)
    {
        StringTokenizer st = new StringTokenizer(e.getActionCommand());
        String cmd = st.nextToken();
        if(cmd.equals("min")){
            int chan = Integer.parseInt(st.nextToken());
            if(chan < 0) imPanel.setWPixMin(Integer.parseInt(minFields[0].getText()));
            else imPanel.setCPixMin(chan,Integer.parseInt(minFields[chan].getText()));
        }
        else if(cmd.equals("max")){
            int chan = Integer.parseInt(st.nextToken());
            if(chan < 0) imPanel.setWPixMax(Integer.parseInt(maxFields[0].getText()));
            else imPanel.setCPixMax(chan,Integer.parseInt(maxFields[chan].getText()));
        }
        else if(cmd.equals("update")){
            if(minFields.length > 1){
                for(int i = 0; i < minFields.length; i++){
                    imPanel.setCPixMin(i,Integer.parseInt(minFields[i].getText()));
                    imPanel.setCPixMax(i,Integer.parseInt(maxFields[i].getText()));
                }
            }
            else{
                imPanel.setWPixMin(Integer.parseInt(minFields[0].getText()));
                imPanel.setWPixMax(Integer.parseInt(maxFields[0].getText()));
            }
        }
        else if(cmd.equals("auto")){
            imPanel.autoScale();
            if(minFields.length > 1){
                for(int i = 0; i < minFields.length; i++){
                    minFields[i].setText(""+imPanel.getCPixMin(i));
                    maxFields[i].setText(""+imPanel.getCPixMax(i));
                }
            }
            else{
                minFields[0].setText(""+imPanel.getWPixMin());
                maxFields[0].setText(""+imPanel.getWPixMax());
            }
        }
        else if(cmd.equals("close")) dispose();
    }
}