import java.awt.BorderLayout;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.SwingConstants;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class StatsPage extends JFrame implements ChangeListener, ActionListener
{    
    private HistogramPanel histPanel;
    private ImagePanel imPanel;
    private int[] region;
    private boolean maskIn;
    private JSlider xslider,yslider;
    private double xmin,xmax;
    private int nbins;
    private JMenuBar menuBar;
    private JMenu menu;
    private JPanel figPanel,textPanel;

    public StatsPage(ImagePanel ip)
    {
	super();
	imPanel = ip;
	region = imPanel.getDisplayRegion();
	maskIn = true;
	menuBar = new JMenuBar();
	menu = new JMenu("Menu");
	menuBar.add(menu);
	JMenuItem menuItem = new JMenuItem("Histogram options");
	menuItem.addActionListener(this);
	menuItem.setActionCommand("histOpt");
	menu.add(menuItem);
	menuItem = new JMenuItem("Plot Function");
	menuItem.addActionListener(this);
	menuItem.setActionCommand("plotf");
	menu.add(menuItem);
	menuItem = new JMenuItem("Fit Function");
	menuItem.addActionListener(this);
	menuItem.setActionCommand("fitf");
	menu.add(menuItem);
	setJMenuBar(menuBar);
	getContentPane().setLayout(new BoxLayout(getContentPane(),BoxLayout.Y_AXIS));
	figPanel = new JPanel();
	textPanel = new JPanel();
	getContentPane().add(figPanel);
	getContentPane().add(textPanel);
	figPanel.setLayout(new BorderLayout());
	xslider = new JSlider(SwingConstants.HORIZONTAL,1,10,1);
	xslider.addChangeListener(this);
	yslider = new JSlider(SwingConstants.VERTICAL,1,10,1);
	yslider.addChangeListener(this);
	nbins = 100;
	if(region[2]*region[3] < 1000) nbins = region[2]*region[3] / 10;
	xmin = (double)imPanel.imMin(maskIn);
	xmax = (double)imPanel.imMax(maskIn);
	histPanel = new HistogramPanel(xmin,xmax,nbins);
	for(int i = region[0]; i < region[0]+region[2]; i++){
	    for(int j = region[1]; j < region[1]+region[3]; j++) histPanel.Fill(imPanel.getImPixel(i,j,maskIn));
	}
	figPanel.add(xslider,BorderLayout.SOUTH);
	figPanel.add(yslider,BorderLayout.WEST);
	figPanel.add(histPanel,BorderLayout.CENTER);
	textPanel.setLayout(new FlowLayout());
	JLabel lab = new JLabel("width: "+region[2]);
	textPanel.add(lab);
	lab = new JLabel("height: "+region[3]);
	textPanel.add(lab);
	lab = new JLabel("min: "+imPanel.imMin(maskIn));
	textPanel.add(lab);
	lab = new JLabel("max: "+imPanel.imMax(maskIn));
	textPanel.add(lab);
	lab = new JLabel("mean: "+(int)imPanel.imMean(maskIn));
	textPanel.add(lab);
	lab = new JLabel("median: "+(int)imPanel.imMedian(maskIn));
	textPanel.add(lab);
	lab = new JLabel("mode: "+(int)imPanel.imMode(maskIn));
	textPanel.add(lab);
	lab = new JLabel("std dev: "+(int)imPanel.imStd(maskIn));
	textPanel.add(lab);
	setDefaultCloseOperation(DISPOSE_ON_CLOSE);
	pack();
	setVisible(true);
    }

    public void stateChanged(ChangeEvent e)
    {
	if(e.getSource() == xslider) histPanel.setLogBaseX(xslider.getValue());
	else if(e.getSource() == yslider) histPanel.setLogBaseY(yslider.getValue());
    }

    public void actionPerformed(ActionEvent e)
    {
        String cmd = e.getActionCommand();
        if(cmd.equals("histOpt")){
            final JDialog jd = new JDialog(this,true);
            jd.getContentPane().setLayout(new BoxLayout(jd.getContentPane(),BoxLayout.Y_AXIS));
            JPanel pan = new JPanel();
            pan.setLayout(new FlowLayout());
            pan.add(new JLabel("x min:"));
            final JTextField minField = new JTextField(5);
            minField.setText(""+xmin);
            pan.add(minField);
            jd.getContentPane().add(pan);
            pan = new JPanel();
            pan.setLayout(new FlowLayout());
            pan.add(new JLabel("x max:"));
            final JTextField maxField = new JTextField(5);
            maxField.setText(""+xmax);
            pan.add(maxField);
            jd.getContentPane().add(pan);
            pan = new JPanel();
            pan.setLayout(new FlowLayout());
            pan.add(new JLabel("n bins:"));
            final JTextField binField = new JTextField(5);
            binField.setText(""+nbins);
            pan.add(binField);
            jd.getContentPane().add(pan);
            pan = new JPanel();
            pan.setLayout(new FlowLayout());
            pan.add(new JLabel("mask in/out:"));
            final JCheckBox maskBox = new JCheckBox();
            if(maskIn) maskBox.setSelected(true);
            pan.add(maskBox);
            jd.getContentPane().add(pan);
            JButton ok = new JButton("OK");
            ok.addActionListener(new ActionListener(){
                public void actionPerformed(ActionEvent e){
                    xmin = Double.parseDouble(minField.getText());
                    xmax = Double.parseDouble(maxField.getText());
                    nbins = Integer.parseInt(binField.getText());
                    if(maskBox.isSelected()) maskIn = true;
                    else maskIn = false;
                    jd.dispose();
                    refresh();
                }
            });
            jd.getContentPane().add(ok);
            jd.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
            jd.pack();
            jd.setVisible(true);
        }
        else if(cmd.equals("plotf")){
            double mean = imPanel.imMode(maskIn);
            double norm = 22618.0*Math.sqrt(2*Math.PI*mean);//(double)histPanel.getEntries();
            double[] par = {mean,Math.sqrt(mean)};
            Function f = Functions.gaussian();
            f.setParameters(par);
            histPanel.setFunctionNorm(norm);
            histPanel.setFunction(f);
        }
        else if(cmd.equals("fitf")){
            double mean = imPanel.imMode(maskIn);
            double par[] = {mean,mean};
            Function f = Functions.gamma();
            Fitter fitter = new Fitter(f,(int)xmin,(int)xmax,par);
            fitter.setDistribution(imPanel.getDistribution(maskIn));
            fitter.fit();
            histPanel.setFunctionNorm(histPanel.getIntegral()/f.integral(xmin,xmax));
            histPanel.setFunction(f);
            par = f.getParameters();
            System.out.println(""+par[0]+", "+par[1]);
        }
    }

    public void refresh()
    {
	histPanel.newHistogram(xmin,xmax,nbins);
	for(int i = region[0]; i < region[0]+region[2]; i++){
	    for(int j = region[1]; j < region[1]+region[3]; j++) histPanel.Fill(imPanel.getImPixel(i,j,maskIn));
	}
	textPanel.removeAll();
	JLabel lab = new JLabel("width: "+region[2]);
	textPanel.add(lab);
	lab = new JLabel("height: "+region[3]);
	textPanel.add(lab);
	lab = new JLabel("min: "+imPanel.imMin(maskIn));
	textPanel.add(lab);
	lab = new JLabel("max: "+imPanel.imMax(maskIn));
	textPanel.add(lab);
	lab = new JLabel("mean: "+(int)imPanel.imMean(maskIn));
	textPanel.add(lab);
	lab = new JLabel("median: "+(int)imPanel.imMedian(maskIn));
	textPanel.add(lab);
	lab = new JLabel("mode: "+(int)imPanel.imMode(maskIn));
	textPanel.add(lab);
	lab = new JLabel("std dev: "+(int)imPanel.imStd(maskIn));
	textPanel.add(lab);
	histPanel.repaint();
	textPanel.repaint();
	textPanel.revalidate();
    }

    public void updateRegion()
    {
	region = imPanel.getDisplayRegion();
	refresh();
    }
}
