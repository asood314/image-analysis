import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Point;
import java.awt.Polygon;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;
import java.util.Vector;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSlider;
import javax.swing.JTextArea;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

public class ImageAnalyzer extends JFrame implements ActionListener, MouseListener, ChangeListener
{
    
    private ImagePanel imPanel;
    private JScrollPane imPane;
    private JPanel topPanel;
    private JTextArea codeArea;
    private JPanel codePanel;
    private Jint jintPanel;
    private JScrollPane codePane;
    private JPanel bottomPanel;
    private JMenuBar menuBar;
    private JMenu fileMenu;
    private JMenu viewMenu;
    private JMenu channelMenu;
    private JMenu redMenu;
    private JMenu greenMenu;
    private JMenu blueMenu;
    private JMenu analyzeMenu;
    private JMenu toolMenu;
    private JMenuItem menuItem;
    private JSlider zslide, tslide, pslide;
    private JButton znext,zback,tnext,tback,pnext,pback;
    private JDialog jd;
    private JTextField loadField;
    private JTextField wField;
    private JTextField zField;
    private JTextField tField;
    private JTextField pField;
    private NDImage ndim;
    private String imName;
    private ImageReport[] reports;
    private boolean punctaSelectorTool;
    private boolean synapseSelectorTool;
    private boolean regionSelectorTool;
    private boolean pixelSelectorTool;
    private boolean roiSelectorTool;
    private int mousePressX,mousePressY,prevX,prevY;
    private Vector<Integer> xpoints,ypoints;
    private ImageAnalysisToolkit analysisTools;
    private StatsPage stats;
    
    public ImageAnalyzer(NDImage im, String name)
    {
        super();
        ndim = im;
        imName = name;
        reports = new ImageReport[im.getNZ()*im.getNT()*im.getNPos()];
        for(int i = 0; i < im.getNZ()*im.getNT()*im.getNPos();i++) reports[i] = new ImageReport(ndim.getNWavelengths());
        //analysisTools = new FixedCellAnalyzer(ndim,reports);
        stats = null;
        punctaSelectorTool = false;
        synapseSelectorTool = false;
        regionSelectorTool = false;
        pixelSelectorTool = true;
	roiSelectorTool = false;
	prevX = -1;
	prevY = -1;
	xpoints = new Vector<Integer>();
	ypoints = new Vector<Integer>();
        setBounds(120,20,1220,1020);
        int[] chan = {1,0};
        imPanel = new ImagePanel(im);
        imPanel.addMouseListener(this);
	analysisTools = new TestAnalyzer(ndim,reports,imPanel);
        imPane = new JScrollPane(imPanel);
        imPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        imPane.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        imPane.getVerticalScrollBar().setUnitIncrement(20);
        imPane.getHorizontalScrollBar().setUnitIncrement(20);
        topPanel = new JPanel();
        topPanel.setLayout(new BoxLayout(topPanel, BoxLayout.Y_AXIS));
        topPanel.add(imPane);
        //topPanel.setPreferredSize(new Dimension(1000,700));
        //codePanel = new JPanel();
        //codeArea = new JTextArea(20,100);
        //codeArea.setEditable(true);
        //codePanel.setLayout(new FlowLayout());
        //codePanel.add(codeArea);
        jintPanel = new Jint();
        jintPanel.setVar(imName,ndim);
        jintPanel.setVar(imName+"_reports",reports);
        //codePane = new JScrollPane(jintPanel);
        bottomPanel = new JPanel();
        bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.Y_AXIS));
        //bottomPanel.add(codePane);
        //bottomPanel.setPreferredSize(new Dimension(1000,200));
        JPanel zPanel = new JPanel();
        zPanel.setLayout(new BoxLayout(zPanel,BoxLayout.X_AXIS));
        zslide = new JSlider(javax.swing.SwingConstants.HORIZONTAL,0,ndim.getNZ()-1,imPanel.getZSlice());
        zslide.addChangeListener(this);
        zback = new NextButton(false);
        zback.addActionListener(this);
        zback.setActionCommand("zback");
        znext = new NextButton(true);
        znext.addActionListener(this);
        znext.setActionCommand("znext");
        zPanel.add(new JLabel("z slice:"));
        zPanel.add(zback);
        zPanel.add(zslide);
        zPanel.add(znext);
        bottomPanel.add(zPanel);
        JPanel tPanel = new JPanel();
        tPanel.setLayout(new BoxLayout(tPanel,BoxLayout.X_AXIS));
        tslide = new JSlider(javax.swing.SwingConstants.HORIZONTAL,0,ndim.getNT()-1,imPanel.getTimepoint());
        tslide.addChangeListener(this);
        tback = new NextButton(false);
        tback.addActionListener(this);
        tback.setActionCommand("tback");
        tnext = new NextButton(true);
        tnext.addActionListener(this);
        tnext.setActionCommand("tnext");
        tPanel.add(new JLabel("time point:"));
        tPanel.add(tback);
        tPanel.add(tslide);
        tPanel.add(tnext);
        bottomPanel.add(tPanel);
        JPanel pPanel = new JPanel();
        pPanel.setLayout(new BoxLayout(pPanel,BoxLayout.X_AXIS));
        pslide = new JSlider(javax.swing.SwingConstants.HORIZONTAL,0,ndim.getNPos()-1,imPanel.getPosition());
        pslide.addChangeListener(this);
        pback = new NextButton(false);
        pback.addActionListener(this);
        pback.setActionCommand("pback");
        pnext = new NextButton(true);
        pnext.addActionListener(this);
        pnext.setActionCommand("pnext");
        pPanel.add(new JLabel("position:"));
        pPanel.add(pback);
        pPanel.add(pslide);
        pPanel.add(pnext);
        bottomPanel.add(pPanel);
        loadField = new JTextField(50);
        loadField.addActionListener(this);
        wField = new JTextField(3);
        zField = new JTextField(3);
        tField = new JTextField(3);
        pField = new JTextField(3);
        menuBar = new JMenuBar();
        fileMenu = new JMenu("Image");
        menuBar.add(fileMenu);
        menuItem = new JMenuItem("Load Image");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("lim");
        fileMenu.add(menuItem);
        menuItem = new JMenuItem("Save Analysis Data");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("saveReps");
        fileMenu.add(menuItem);
        menuItem = new JMenuItem("Load Analysis Data");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("loadReps");
        fileMenu.add(menuItem);
        menuItem = new JMenuItem("Z-project");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("zproj");
        fileMenu.add(menuItem);
        viewMenu = new JMenu("View");
        menuBar.add(viewMenu);
        channelMenu = new JMenu("Channel");
        viewMenu.add(channelMenu);
        for(int i = 0; i < ndim.getNWavelengths();  i++){
            menuItem = new JMenuItem("Channel " + i);
            menuItem.addActionListener(this);
            menuItem.setActionCommand("vch " + i);
            channelMenu.add(menuItem);
        }
        menuItem = new JMenuItem("Composite");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vcom");
        viewMenu.add(menuItem);
        menuItem = new JMenuItem("Split Composite");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vsplit");
        viewMenu.add(menuItem);
        redMenu = new JMenu("Red Channel");
        viewMenu.add(redMenu);
        menuItem = new JMenuItem("None");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("rch -1");
        redMenu.add(menuItem);
        for(int i = 0; i < ndim.getNWavelengths();  i++){
            menuItem = new JMenuItem("Channel " + i);
            menuItem.addActionListener(this);
            menuItem.setActionCommand("rch " + i);
            redMenu.add(menuItem);
        }
        greenMenu = new JMenu("Green Channel");
        viewMenu.add(greenMenu);
        menuItem = new JMenuItem("None");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("gch -1");
        greenMenu.add(menuItem);
        for(int i = 0; i < ndim.getNWavelengths();  i++){
            menuItem = new JMenuItem("Channel " + i);
            menuItem.addActionListener(this);
            menuItem.setActionCommand("gch " + i);
            greenMenu.add(menuItem);
        }
        blueMenu = new JMenu("Blue Channel");
        viewMenu.add(blueMenu);
        menuItem = new JMenuItem("None");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("bch -1");
        blueMenu.add(menuItem);
        for(int i = 0; i < ndim.getNWavelengths();  i++){
            menuItem = new JMenuItem("Channel " + i);
            menuItem.addActionListener(this);
            menuItem.setActionCommand("bch " + i);
            blueMenu.add(menuItem);
        }
        menuItem = new JMenuItem("Adjust Scale");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("scale");
        viewMenu.add(menuItem);
        JMenu maskMenu = new JMenu("Mask");
        viewMenu.add(maskMenu);
        menuItem = new JMenuItem("None");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm none");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Outlier Mask");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm outlier");
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Background Mask");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm bkgd");
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Signal Mask");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm signal");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Puncta");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm puncta");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Synapses");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("vm synapses");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        maskMenu.add(menuItem);
        menuItem = new JMenuItem("Stats");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("stats");
        viewMenu.add(menuItem);
        menuItem = new JMenuItem("Zoom In");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("zin");
        menuItem.setAccelerator(KeyStroke.getKeyStroke('=', Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        viewMenu.add(menuItem);
        menuItem = new JMenuItem("Zoom Out");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("zout");
        menuItem.setAccelerator(KeyStroke.getKeyStroke('-', Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        viewMenu.add(menuItem);
        menuItem = new JMenuItem("Selected Region");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("zreg");
        viewMenu.add(menuItem);
        menuItem = new JMenuItem("Unzoom");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("unzoom");
        menuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_U, Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()));
        viewMenu.add(menuItem);
        analyzeMenu = new JMenu("Analyze");
        menuBar.add(analyzeMenu);
        menuItem = new JMenuItem("Standard Analysis");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("Standard Analysis");
        analyzeMenu.add(menuItem);
	menuItem = new JMenuItem("Find Background Mask");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("findbm");
        analyzeMenu.add(menuItem);
        menuItem = new JMenuItem("Find Signal Mask");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("findsm");
        analyzeMenu.add(menuItem);
        menuItem = new JMenuItem("Find Puncta");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("findpunc");
        analyzeMenu.add(menuItem);
        menuItem = new JMenuItem("Find Synapses");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("findsyn");
        analyzeMenu.add(menuItem);
        menuItem = new JMenuItem("Synapse Density");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("syndens");
        analyzeMenu.add(menuItem);
        toolMenu = new JMenu("Tools");
        menuBar.add(toolMenu);
        menuItem = new JMenuItem("Region Selector");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("rsel");
        toolMenu.add(menuItem);
        menuItem = new JMenuItem("Puncta Selector");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("psel");
        toolMenu.add(menuItem);
        menuItem = new JMenuItem("Synapse Selector");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("ssel");
        toolMenu.add(menuItem);
	menuItem = new JMenuItem("Pixel Selector");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("pxsel");
        toolMenu.add(menuItem);
	menuItem = new JMenuItem("ROI Selector");
        menuItem.addActionListener(this);
        menuItem.setActionCommand("roisel");
        toolMenu.add(menuItem);
        setJMenuBar(menuBar);
        getContentPane().setLayout(new BoxLayout(getContentPane(), BoxLayout.Y_AXIS));
        getContentPane().add(topPanel);
        getContentPane().add(bottomPanel);
        setDefaultCloseOperation(EXIT_ON_CLOSE);
        //pack();
        setVisible(true);
    }
    
    public void actionPerformed(ActionEvent e)
    {
        String cmd = e.getActionCommand();
        //System.out.println(Runtime.getRuntime().freeMemory());
        if(cmd.equals("lim")){
            jd = new JDialog(this);
            jd.setBounds(300,300,900,600);
            jd.getContentPane().setLayout(new BoxLayout(jd.getContentPane(),BoxLayout.Y_AXIS));
            JPanel jp = new JPanel();
            jp.setLayout(new FlowLayout());
            loadField.setText("Name");
            loadField.setActionCommand("");
            jp.add(loadField);
            wField.setText("#w");
            jp.add(wField);
            zField.setText("#z");
            jp.add(zField);
            tField.setText("#t");
            jp.add(tField);
            pField.setText("pos");
            jp.add(pField);
            jd.getContentPane().add(jp);
            jp = new JPanel();
            jp.setLayout(new FlowLayout());
            JButton button = new JButton("Load From File");
            button.addActionListener(this);
            button.setActionCommand("limff");
            jp.add(button);
            button = new JButton("Load S. Data");
            button.addActionListener(this);
            button.setActionCommand("lsd");
            jp.add(button);
            button = new JButton("Load From Memory");
            button.addActionListener(this);
            button.setActionCommand("limfm");
            jp.add(button);
            jd.getContentPane().add(jp);
            jd.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            jd.setVisible(true);
        }
        else if(cmd.equals("limff")){
            String fileName = loadField.getText();
            StringTokenizer st = new StringTokenizer(fileName,"\\/");
            String varName = "";
            while(st.hasMoreTokens()) varName = st.nextToken();
            int nw = Integer.parseInt(wField.getText());
            int nz = Integer.parseInt(zField.getText());
            int np = Integer.parseInt(pField.getText());
            try{
                ndim = new NDImage(nw,nz,np,fileName);
                reports = new ImageReport[ndim.getNZ()*ndim.getNT()*ndim.getNPos()];
                analysisTools.setImage(ndim);
                analysisTools.setImageReports(reports);
                imName = varName;
                jintPanel.setVar(varName,ndim);
                jintPanel.setVar(varName+"_reports",reports);
                imPanel.setImage(ndim);
            }
            catch(IOException ex){ ex.printStackTrace(); }
            jd.dispose();
        }
        else if(cmd.equals("lsd")){
            String fileName = loadField.getText();
            StringTokenizer st = new StringTokenizer(fileName,"\\/",true);
            String dir = "";
            String varName = st.nextToken();
            while(st.hasMoreTokens()){
                dir += varName;
                varName = st.nextToken();
            }
            int nt = Integer.parseInt(tField.getText());
            int nz = Integer.parseInt(zField.getText());
            int pos = Integer.parseInt(pField.getText());
            try{
                ndim = NDImage.loadSamanthaData(dir,varName,pos,nz,nt);
                reports = new ImageReport[ndim.getNZ()*ndim.getNT()];
                analysisTools.setImage(ndim);
                analysisTools.setImageReports(reports);
                imName = varName;
                jintPanel.setVar(varName,ndim);
                jintPanel.setVar(varName+"_reports",reports);
                imPanel.setImage(ndim);
            }
            catch(IOException ex){ ex.printStackTrace(); }
            jd.dispose();
        }
        else if(cmd.equals("limfm")){
            String varName = loadField.getText();
            ndim = (NDImage)(jintPanel.getVar(varName));
            reports = (ImageReport[])(jintPanel.getVar(varName+"_reports"));
            analysisTools.setImage(ndim);
            analysisTools.setImageReports(reports);
            imName = varName;
            imPanel.setImage(ndim);
            jd.dispose();
        }
        else if(cmd.equals("saveReps")){
            jd = new JDialog(this);
            jd.getContentPane().setLayout(new BoxLayout(jd.getContentPane(),BoxLayout.X_AXIS));
            loadField.setText("Enter file name");
            loadField.setActionCommand("simrep");
            JButton button = new JButton("Load");
            button.addActionListener(this);
            button.setActionCommand("simrep");
            jd.getContentPane().add(loadField);
            jd.getContentPane().add(button);
            jd.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            jd.pack();
            jd.setVisible(true);
        }
        else if(cmd.equals("simrep")){
            analysisTools.saveImageReports(loadField.getText());
            jd.dispose();
        }
        else if(cmd.equals("loadReps")){
            jd = new JDialog(this);
            jd.getContentPane().setLayout(new BoxLayout(jd.getContentPane(),BoxLayout.X_AXIS));
            loadField.setText("Enter file name");
            loadField.setActionCommand("limrep");
            JButton button = new JButton("Load");
            button.addActionListener(this);
            button.setActionCommand("limrep");
            jd.getContentPane().add(loadField);
            jd.getContentPane().add(button);
            jd.setDefaultCloseOperation(DISPOSE_ON_CLOSE);
            jd.pack();
            jd.setVisible(true);
        }
        else if(cmd.equals("limrep")){
            analysisTools.loadImageReports(loadField.getText());
            jd.dispose();
        }
        else if(cmd.equals("zproj")){
            ndim = ndim.zprojection();
            reports = new ImageReport[ndim.getNPos()*ndim.getNT()];
            analysisTools.setImage(ndim);
            analysisTools.setImageReports(reports);
            String varName = imName+"_zproj";
            jintPanel.setVar(varName,ndim);
            jintPanel.setVar(varName+"_reports",reports);
        }
        else if(cmd.contains("vch")){
            StringTokenizer st = new StringTokenizer(cmd);
            st.nextToken();
            int wl = Integer.parseInt(st.nextToken());
            imPanel.setWavelength(wl);
            imPanel.setMode(ImagePanel.GRAY_8);
            //imPanel.autoScale();
            imPanel.setMaskColor(0xffff00ff);
            imPanel.repaint();
        }
        else if(cmd.equals("vcom")){
            imPanel.setMode(ImagePanel.RGB_8);
            //imPanel.autoScale();
            imPanel.setMaskColor(0xffffffff);
            imPanel.repaint();
        }
        else if(cmd.equals("vsplit")){
            imPanel.setMode(ImagePanel.SPLIT);
            //imPanel.autoScale();
            imPanel.setMaskColor(0xffffffff);
            imPanel.repaint();
        }
        else if(cmd.contains("rch")){
            StringTokenizer st = new StringTokenizer(cmd);
            st.nextToken();
            int wl = Integer.parseInt(st.nextToken());
            imPanel.setChannel(0,wl);
            imPanel.repaint();
        }
        else if(cmd.contains("gch")){
            StringTokenizer st = new StringTokenizer(cmd);
            st.nextToken();
            int wl = Integer.parseInt(st.nextToken());
            imPanel.setChannel(1,wl);
            imPanel.repaint();
        }
        else if(cmd.contains("bch")){
            StringTokenizer st = new StringTokenizer(cmd);
            st.nextToken();
            int wl = Integer.parseInt(st.nextToken());
            imPanel.setChannel(2,wl);
            imPanel.repaint();
        }
        else if(cmd.equals("scale")){
            ScaleAdjuster sa = new ScaleAdjuster(imPanel);
        }
        else if(cmd.equals("zin")){
            imPanel.setZoom(imPanel.getZoom() + 0.1);
            imPanel.repaint();
        }
        else if(cmd.equals("zout")){
            imPanel.setZoom(imPanel.getZoom() - 0.1);
            imPanel.repaint();
        }
	else if(cmd.equals("zreg")){
	    imPanel.setZoomToRegion(true);
	    imPanel.repaint();
	}
        else if(cmd.equals("unzoom")){
            imPanel.unzoom();
            imPanel.repaint();
        }
        else if(cmd.contains("vm")){
            if(cmd.equals("vm signal")){
                ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
                if(r != null){
                    //imPanel.setMask(r.getSignalMask(imPanel.getWavelength()));
		    imPanel.toggleMask(r.getSignalMask(imPanel.getWavelength()));
                    imPanel.repaint();
                }
            }
	    else if(cmd.equals("vm bkgd")){
                ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
                if(r != null){
                    //imPanel.setMask(r.getUtilityMask(imPanel.getWavelength()));
		    imPanel.toggleMask(r.getUtilityMask(imPanel.getWavelength()));
                    imPanel.repaint();
                }
            }
            else if(cmd.equals("vm outlier")){
                ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
                if(r != null){
                    //imPanel.setMask(r.getOutlierMask(imPanel.getWavelength()).getInverse());
		    imPanel.toggleMask(r.getOutlierMask(imPanel.getWavelength()).getInverse());
                    imPanel.repaint();
                }
            }
            else if(cmd.equals("vm puncta")){
                ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
                if(imPanel.getMode() == ImagePanel.SPLIT){
                    Mask m = new Mask(ndim.getWidth()*2,ndim.getHeight()*2);
                    for(int w = 0; w < ndim.getNWavelengths(); w++){
                        int xOffset = ((w+1) % 2) * ndim.getWidth();
                        int yOffset = ((w+1) / 2) * ndim.getHeight();
                        m.add(r.getPunctaMask(w),xOffset,yOffset);
                    }
                    //imPanel.setMask(m);
		    imPanel.toggleMask(m);
                    repaint();
                }
                else{
                    //imPanel.setMask(circlePuncta(imPanel.getWavelength(),imPanel.getZSlice(),imPanel.getTimepoint()));
                    imPanel.toggleMask(r.getPunctaMask(imPanel.getWavelength()));
                    repaint();
                }
            }
            else if(cmd.equals("vm synapses")){
                if(imPanel.getMode() == ImagePanel.SPLIT){
                    ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
                    Mask m = new Mask(ndim.getWidth()*2,ndim.getHeight()*2);
                    m.add(circleSynapses(imPanel.getZSlice(),imPanel.getTimepoint(),imPanel.getPosition()));
                    for(int w = 0; w < ndim.getNWavelengths(); w++){
                        int xOffset = ((w+1) % 2) * ndim.getWidth();
                        int yOffset = ((w+1) / 2) * ndim.getHeight();
                        m.add(r.getPunctaMask(w),xOffset,yOffset);
                    }
                    //imPanel.setMask(m);
		    imPanel.toggleMask(m);
                    repaint();
                }
                else{
                    //imPanel.setMask(circleSynapses(imPanel.getZSlice(),imPanel.getTimepoint(),imPanel.getPosition()));
		    imPanel.toggleMask(circleSynapses(imPanel.getZSlice(),imPanel.getTimepoint(),imPanel.getPosition()));
                    repaint();
                }
            }
            else if(cmd.equals("vm none")){
                //imPanel.setMask((Mask)null);
		imPanel.clearMasks();
                imPanel.repaint();
            }
        }
	else if(cmd.equals("stats")){
	    stats = new StatsPage(imPanel);
	}
        else if(cmd.equals("rsel")){
            regionSelectorTool = true;
            punctaSelectorTool = false;
            synapseSelectorTool = false;
	    pixelSelectorTool = false;
	    roiSelectorTool = false;
        }
        else if(cmd.equals("psel")){
            regionSelectorTool = false;
            punctaSelectorTool = true;
            synapseSelectorTool = false;
	    pixelSelectorTool = false;
	    roiSelectorTool = false;
        }
        else if(cmd.equals("ssel")){
            regionSelectorTool = false;
            punctaSelectorTool = false;
            synapseSelectorTool = true;
	    pixelSelectorTool = false;
	    roiSelectorTool = false;
        }
	else if(cmd.equals("pxsel")){
            regionSelectorTool = false;
            punctaSelectorTool = false;
            synapseSelectorTool = false;
	    pixelSelectorTool = true;
	    roiSelectorTool = false;
        }
	else if(cmd.equals("roisel")){
            regionSelectorTool = false;
            punctaSelectorTool = false;
            synapseSelectorTool = false;
	    pixelSelectorTool = false;
	    roiSelectorTool = true;
        }
        else if(cmd.equals("Standard Analysis")){
            analysisTools.standardAnalysis(imPanel.getZSlice(),imPanel.getTimepoint(),imPanel.getPosition());
            System.out.println("Done.");
        }
        else if(cmd.equals("findsm")){
            int z = imPanel.getZSlice();
            int t = imPanel.getTimepoint();
            int p = imPanel.getPosition();
            int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
            for(int w = 0; w < ndim.getNWavelengths(); w++){
                //Mask m = analysisTools.findOutlierMask(w,z,t,p);
                //reports[index].setOutlierMask(w,m);
                reports[index].setSignalMask(w,analysisTools.findSignalMask(w,z,t,p,reports[index].getOutlierMask(w).getInverse()));
            }
            System.out.println("Done.");
        }
	else if(cmd.equals("findbm")){
            int z = imPanel.getZSlice();
            int t = imPanel.getTimepoint();
            int p = imPanel.getPosition();
            int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
            for(int w = 0; w < ndim.getNWavelengths(); w++){
                reports[index].setUtilityMask(w,analysisTools.findBackgroundMask(w,z,t,p));
            }
            System.out.println("Done.");
        }
        else if(cmd.equals("findpunc")){
            int z = imPanel.getZSlice();
            int t = imPanel.getTimepoint();
            int p = imPanel.getPosition();
            for(int w = 0; w < ndim.getNWavelengths(); w++) analysisTools.findPuncta(w,z,t,p);
            System.out.println("Done.");
        }
        else if(cmd.equals("findsyn")){
            int z = imPanel.getZSlice();
            int t = imPanel.getTimepoint();
            int p = imPanel.getPosition();
            analysisTools.findSynapses(z,t,p);
            System.out.println("Done.");
        }
        else if(cmd.equals("syndens")){
            int z = imPanel.getZSlice();
            int t = imPanel.getTimepoint();
            int p = imPanel.getPosition();
	    ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
	    r.getSynapseDensity(1);
            System.out.println("Done.");
        }
        else if(cmd.equals("zback")){
            int z = zslide.getValue();
            if(z > 0) zslide.setValue(z-1);
        }
        else if(cmd.equals("znext")){
            int z = zslide.getValue()+1;
            if(z < ndim.getNZ()) zslide.setValue(z);
        }
        else if(cmd.equals("tback")){
            int t = tslide.getValue();
            if(t > 0) tslide.setValue(t-1);
        }
        else if(cmd.equals("tnext")){
            int t = tslide.getValue()+1;
            if(t < ndim.getNT()) tslide.setValue(t);
        }
        else if(cmd.equals("pback")){
            int p = pslide.getValue();
            if(p > 0) pslide.setValue(p-1);
        }
        else if(cmd.equals("pnext")){
            int p = pslide.getValue()+1;
            if(p < ndim.getNPos()) pslide.setValue(p);
        }
        Runtime.getRuntime().gc();
    }
    
    public void mouseClicked(MouseEvent e)
    {
        //System.out.println("Click X: " + e.getX() + " Y: " + e.getY());
        int trueX = (int)(e.getX() / imPanel.getZoom());
        int trueY = (int)(e.getY() / imPanel.getZoom());
        ImageReport r = reports[imPanel.getPosition()*ndim.getNT()*ndim.getNZ() + imPanel.getTimepoint()*ndim.getNZ() + imPanel.getZSlice()];
        if(punctaSelectorTool){
            Point p = new Point(trueX,trueY);
            Cluster c = null;
	    if(imPanel.getMode() != ImagePanel.GRAY_8) c = r.selectPunctum(p);
	    else c = r.selectPunctum(p,imPanel.getWavelength());
	    if(c == null) return;
	    p = c.getPixel(0);
            System.out.println("Center: " + c.getCentroid().toString() + ", Size: " + c.size() + ", Peak Location: " + p.toString() + ", Peak Intensity: " + ndim.getPixel(imPanel.getWavelength(),imPanel.getZSlice(),imPanel.getTimepoint(),p.x,p.y,imPanel.getPosition()));
	    Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
	    for(int i = 0; i < c.size(); i++){
		p = c.getPixel(i);
		m.setValue(p.x,p.y,1);
	    }
	    imPanel.toggleMask(m);
        }
        else if(synapseSelectorTool){
            Point p = new Point(trueX,trueY);
            Synapse s = r.selectSynapse(p);
            System.out.println("Center: " + s.getCenter().toString() + ", Overlap: " + s.getOverlap());
	    for(int j = 0; j < s.getNPuncta(); j++){
		Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
		Cluster c = s.getPunctum(j);
		for(int i = 0; i < c.size(); i++){
		    p = c.getPixel(i);
		    m.setValue(p.x,p.y,1);
		}
		imPanel.toggleMask(m);
	    }
        }
	else if(pixelSelectorTool){
	    System.out.println("Location: (" + trueX + "," + trueY + ") Intensity: " + ndim.getPixel(imPanel.getWavelength(),imPanel.getZSlice(),imPanel.getTimepoint(),trueX,trueY,imPanel.getPosition()));
	}
	else if(roiSelectorTool){
	    if(e.getX() != prevX && e.getY() != prevY){
		xpoints.addElement(trueX);
		ypoints.addElement(trueY);
		prevX = e.getX();
		prevY = e.getY();
	    }
	    else{
		int npoints = xpoints.size();
		int[] xpArr = new int[npoints];
		int[] ypArr = new int[npoints];
		for(int i = 0; i < npoints; i++){
		    xpArr[i] = xpoints.elementAt(i);
		    ypArr[i] = ypoints.elementAt(i);
		}
		Polygon p = new Polygon(xpArr,ypArr,npoints);
		Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
		for(int i = 0; i < m.getWidth(); i++){
		    for(int j = 0; j < m.getHeight(); j++){
			if(p.contains(i,j)) m.setValue(i,j,1);
		    }
		}
		r.setROI(m);
		r.setUtilityMask(imPanel.getWavelength(),m);
		imPanel.toggleMask(m);
		xpoints.clear();
		ypoints.clear();
		prevX = -1;
		prevY = -1;
	    }
	}
    }
    
    public void mouseEntered(MouseEvent e){}
    
    public void mouseExited(MouseEvent e){}
    
    public void mousePressed(MouseEvent e)
    {
        //System.out.println("Press X: " + e.getX() + " Y: " + e.getY());
        mousePressX = e.getX();
        mousePressY = e.getY();
    }
    
    public void mouseReleased(MouseEvent e)
    {
        //System.out.println("Release X: " + e.getX() + " Y: " + e.getY());
        if(regionSelectorTool){
            if(e.getX() == mousePressX && e.getY() == mousePressY) return;
            int trueX1 = (int)(mousePressX / imPanel.getZoom());
            int trueY1 = (int)(mousePressY / imPanel.getZoom());
            int trueX2 = (int)(e.getX() / imPanel.getZoom());
            int trueY2 = (int)(e.getY() / imPanel.getZoom());
            imPanel.setDisplayRegion(Math.min(trueX1,trueX2),Math.min(trueY1,trueY2),Math.abs(trueX1-trueX2),Math.abs(trueY1-trueY2));
            imPanel.repaint();
	    if(stats != null) stats.updateRegion();
        }
    }
    
    public void stateChanged(ChangeEvent e)
    {
        if(e.getSource() == zslide){
            imPanel.setZSlice(zslide.getValue());
            imPanel.repaint();
        }
        if(e.getSource() == tslide){
            imPanel.setTimepoint(tslide.getValue());
            imPanel.repaint();
        }
        if(e.getSource() == pslide){
            imPanel.setPosition(pslide.getValue());
            imPanel.repaint();
        }
    }
    
    public void setImage(NDImage im)
    {
        ndim = im;
        reports = new ImageReport[im.getNZ()*im.getNT()*im.getNPos()];
    }
    
    public void setImage(NDImage im, ImageReport[] irs)
    {
        ndim = im;
        reports = irs;
    }
    
    public Mask circleSynapses(int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        Mask m;
        if(imPanel.getMode() == ImagePanel.SPLIT) m = new Mask(ndim.getWidth()*2, ndim.getHeight()*2);
        else m = new Mask(ndim.getWidth(),ndim.getHeight());
        Vector<Point> centers = new Vector<Point>();
        for(int s = 0; s < r.getNSynapses(); s++){
            centers.add(r.getSynapse(s).getCenter());
        }
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                for(int k = 0; k < centers.size(); k++){
                    double dist = Math.sqrt(Math.pow(centers.elementAt(k).getX() - i,2) + Math.pow(centers.elementAt(k).getY()-j,2));
                    if(dist > 12 && dist < 16){
                        m.setValue(i,j,1);
                        break;
                    }
                }
            }
        }
        return m;
    }
    
    public Mask circlePuncta(int w, int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        int chan = imPanel.findChannel(w);
        if(chan < 0) return null;
        int offsetX = 0;
        int offsetY = 0;
        Mask m;
        if(imPanel.getMode() == ImagePanel.SPLIT){
            m = new Mask(ndim.getWidth()*2, ndim.getHeight()*2);
            if(chan == 0) offsetX = ndim.getWidth();
            else if(chan == 1) offsetY = ndim.getHeight();
            else if(chan == 2){
                offsetX = ndim.getWidth();
                offsetY = ndim.getHeight();
            }
        }
        else m = new Mask(ndim.getWidth(),ndim.getHeight());
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                for(int k = 0; k < r.getNPuncta(w); k++){
                    Cluster c = r.getPunctum(w,k);
                    double dist = Math.sqrt(Math.pow(c.getCentroid().getX() - i,2) + Math.pow(c.getCentroid().getY()-j,2));
                    if(dist > 12 && dist < 16){
                        m.setValue(offsetX+i,offsetY+j,1);
                        break;
                    }
                }
            }
        }
        return m;
    }
    
    //9668390 --> 3,340,771
    public static void main(String[] args)
    {
        try
        {
            //DataInputStream fin = new DataInputStream(new FileInputStream("test1001.tiff"));
            //fin.skipBytes(2903920);
            /*
            boolean found = false;
            long pos = 0;
            int b0 = fin.readUnsignedByte();
            while(!found)
            {
                int b1 = fin.readUnsignedByte();
                int b = ((b1 & 0xff) << 8) | (b0 & 0xff);
                if(b == 16383) found = true;
                pos++;
                b0 = b1;
                //if(pos % 100 == 0)System.out.println(b);
            }
            System.out.println(pos);
            */
            
            //byte[] b = new byte[100];
            //fin.read(b,0,12);
            //int val = ((b[3] & 0xff) << 24) | ((b[2] & 0xff) << 16) | ((b[1] & 0xff) << 8) | (b[0] & 0xff);
            //System.out.println(val);
            /*for(int i = 0; i < 2; i++){
                int val = ((b[2*i+1] & 0xff) << 8) | (b[2*i] & 0xff);
                System.out.println(val);
            }
            int val = ((b[7] & 0xff) << 24) | ((b[6] & 0xff) << 16) | ((b[5] & 0xff) << 8) | (b[4] & 0xff);
            System.out.println(val);
            val = ((b[11] & 0xff) << 24) | ((b[10] & 0xff) << 16) | ((b[9] & 0xff) << 8) | (b[8] & 0xff);
            System.out.println(val);*/
            //NDImage ndi = new NDImage(2,5,"tf8_test.tiff");
            NDImage ndi = NDImage.loadSamanthaData("Z:\\Samantha Processed Images\\New Nikon\\Long-term Live Imaging\\SHANK\\20130113\\20130113-2\\Files to Average\\xy4/","20130113-2",4,2,6);
	    //NDImage ndi = NDImage.loadSamanthaData("/cygdrive/z/Samantha Processed Images/New Nikon/Long-term Live Imaging/SHANK/20130113/20130113-2/Files to Average/xy4/","20130113-2",4,2,6);
            ImageAnalyzer ia = new ImageAnalyzer(ndi,"20130113-2");
            
        }
        catch(IOException e)
        {
            System.out.println("Exception!");
            e.printStackTrace();
        }
    }
}
