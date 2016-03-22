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
    private JPanel minPanel,maxPanel;
    private JLabel minLabel,maxLabel;
    //private JLabel[] channelLabels;
    private JTextField[] minFields, maxFields;
    private JButton autoscaleButton,closeButton;
    private ImagePanel imPanel;
    
    public ScaleAdjuster(ImagePanel ip)
    {
        super();
        //setBounds(100,100,400,400);
        imPanel = ip;
        minPanel = new JPanel();
        maxPanel = new JPanel();
        minLabel = new JLabel("Min Value");
        maxLabel = new JLabel("Max Value");
        minPanel.setLayout(new BoxLayout(minPanel,BoxLayout.Y_AXIS));
        minPanel.add(minLabel);
        maxPanel.setLayout(new BoxLayout(maxPanel,BoxLayout.Y_AXIS));
        maxPanel.add(maxLabel);
        if(imPanel.getMode() == ImagePanel.GRAY_8){
            minFields = new JTextField[1];
            maxFields = new JTextField[1];
            minFields[0] = new JTextField(10);
            minFields[0].setText(""+imPanel.getWPixMin());
            minFields[0].addActionListener(this);
            minFields[0].setActionCommand("min -1");
            maxFields[0] = new JTextField(10);
            maxFields[0].setText(""+imPanel.getWPixMax());
            maxFields[0].addActionListener(this);
            maxFields[0].setActionCommand("max -1");
            minPanel.add(minFields[0]);
            maxPanel.add(maxFields[0]);
        }
        else{
            int nchan = imPanel.getNChannels();
            minFields = new JTextField[nchan];
            maxFields = new JTextField[nchan];
            for(int i = 0; i < nchan; i++){
                minFields[i] = new JTextField(10);
                minFields[i].setText(""+imPanel.getCPixMin(i));
                minFields[i].addActionListener(this);
                minFields[i].setActionCommand("min " + i);
                maxFields[i] = new JTextField(10);
                maxFields[i].setText(""+imPanel.getCPixMax(i));
                maxFields[i].addActionListener(this);
                maxFields[i].setActionCommand("max " + i);
                minPanel.add(minFields[i]);
                maxPanel.add(maxFields[i]);
            }
        }
        autoscaleButton = new JButton("Auto Scale");
        autoscaleButton.addActionListener(this);
        autoscaleButton.setActionCommand("auto");
        minPanel.add(autoscaleButton);
        closeButton = new JButton("Close");
        closeButton.addActionListener(this);
        closeButton.setActionCommand("close");
        maxPanel.add(closeButton);
        getContentPane().setLayout(new BoxLayout(getContentPane(),BoxLayout.X_AXIS));
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
        else if(cmd.equals("auto")) imPanel.autoScale();
        else if(cmd.equals("close")) dispose();
    }
}