import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import javax.swing.JPanel;

public class HistogramPanel extends JPanel
{
    private int[] histogram;
    private double xmin,xmax,binWidth;
    private int nbins,logBaseX,logBaseY,ymin,ymax,entries;
    private int preferredWidth;
    private int width,height;
    private Color histColor;
    private Function funk;
    private Color funkColor;

    public HistogramPanel(double min, double max, int nb)
    {
	super();
	histogram = new int[nb+2];
	for(int i = 0; i < nb+2; i++) histogram[i] = 0;
	xmin = min;
	xmax = max;
	nbins = nb;
	binWidth = (xmax - xmin) / nbins;
	ymin = 0;
	ymax = -1;
	logBaseX = 1;
	logBaseY = 1;
	entries = 0;
	preferredWidth = 860;
	width = ((preferredWidth-60)/nbins) * nbins + 60;
	height = 660;
	histColor = Color.gray;
	funk = null;
	funkColor = Color.black;
	setPreferredSize(new Dimension(width,height));
	setBackground(Color.white);
    }

    public void setDimensions(int w, int h)
    {
	preferredWidth = w;
	width = ((preferredWidth-60)/nbins) * nbins + 60;
	height = h;
	setPreferredSize(new Dimension(width,height));
	revalidate();
    }

    public void setLogBaseX(int base)
    {
	logBaseX = base;
	repaint();
    }

    public void setLogBaseY(int base)
    {
	logBaseY = base;
	repaint();
    }

    public void setFunction(Function f)
    {
	funk = f;
	repaint();
    }

    public void newHistogram(double min, double max, int nb)
    {
	histogram = new int[nb+2];
	for(int i = 0; i < nb+2; i++) histogram[i] = 0;
	xmin = min;
	xmax = max;
	nbins = nb;
	binWidth = (xmax - xmin) / nbins;
	entries = 0;
	width = ((preferredWidth-60)/nbins) * nbins + 60;
	setPreferredSize(new Dimension(width,height));
	revalidate();
    }

    public void Fill(double value)
    {
	if(value < xmin){
	    histogram[nbins+1]++;
	    entries++;
	    return;
	}
	int b = (int)((value - xmin) / binWidth);
	if(b > nbins) histogram[nbins]++;
	else histogram[b]++;
	entries++;
    }

    public int max()
    {
	int m = 0;
	for(int i = 0; i < nbins; i++){
	    if(histogram[i] > m) m = histogram[i];
	}
	return m;
    }

    public int getEntries(){ return entries; }

    public int getIntegral()
    {
	int sum = 0;
	for(int i = 0; i < nbins; i++) sum += histogram[i];
	return sum;
    }

    private double log(int base, int value)
    {
	if(value < 1) return -1;
	return Math.log((double)value)/Math.log((double)base);
    }

    public void paintComponent(Graphics g)
    {
	g.setColor(Color.white);
	g.fillRect(0,0,width,height);
	g.setColor(Color.black);
	g.drawLine(40,height-40,40,20);
	g.drawLine(40,height-40,width-20,height-40);
	int maxValue = ymax;
	int minValue = ymin;
	if(maxValue < 0) maxValue = max();
	if(logBaseY > 1){
	    if(minValue == 0) minValue = -1;
	    int nTicks = (int)(log(logBaseY,maxValue) - minValue);
	    int startHeight = (int)(((double)nTicks)*(height-60)/(log(logBaseY,maxValue)-minValue));
	    int yTickSpacing = startHeight / nTicks;
	    int spacer = height - 60 - startHeight;
	    //System.out.println(""+nTicks+", "+startHeight+", "+yTickSpacing+", "+spacer);
	    if(nTicks < 10){
		for(int i = 0; i < nTicks; i++){
		    g.drawLine(30,20+spacer+(i*yTickSpacing),40,20+spacer+(i*yTickSpacing));
		    g.drawString(""+logBaseY+"^"+(nTicks-i-1),5,20+spacer+(i*yTickSpacing));
		}
	    }
	    else{
		for(int i = 0; i < nTicks; i+=2){
		    g.drawLine(30,20+spacer+(i*yTickSpacing),40,20+spacer+(i*yTickSpacing));
		    g.drawString(""+logBaseY+"^"+(nTicks-i-1),5,20+spacer+(i*yTickSpacing));
		}
	    }
	}
	else{
	    int yTickSpacing = (height-60) / 10;
	    for(int i = 0; i < 10; i++){
		g.drawLine(30,20+(i*yTickSpacing),40,20+(i*yTickSpacing));
		g.drawLine(35,20+((2*i+1)*yTickSpacing/2),40,20+((2*i+1)*yTickSpacing/2));
		g.drawString(""+(maxValue-(i*maxValue/10)),2,18+(i*yTickSpacing));
	    }
	}
	//System.out.println(maxValue);
	//System.out.println(minValue);
	int xTickSpacing = (width-60) / 10;
	for(int i = 0; i < 10; i++){
	    g.drawLine(width-20-(i*xTickSpacing),height-30,width-20-(i*xTickSpacing),height-40);
	    g.drawLine(width-20-((2*i+1)*xTickSpacing/2),height-35,width-20-((2*i+1)*xTickSpacing/2),height-40);
	    g.drawString(""+((int)(xmax-(i*(xmax-xmin)/10))),width-30-(i*xTickSpacing),height-20);
	}
	int rectWidth = (int)(((double)(width-60)) / nbins);
	//System.out.println(rectWidth);
	g.setColor(histColor);
	for(int i = 0; i < nbins; i++){
	    int rectHeight = 0;
	    if(logBaseY > 1) rectHeight = (int)((height-60) * (log(logBaseY,histogram[i])-minValue)/(log(logBaseY,maxValue)-minValue));
	    else rectHeight = (int)((height-60) * (((double)histogram[i] - minValue)/(maxValue-minValue)));
	    //System.out.println(rectHeight);
	    if(rectHeight > 0) g.fillRect(40+(i*rectWidth),height-40-rectHeight,rectWidth,rectHeight);
	}
	if(funk != null){
	    //System.out.println(funk.integral(xmin,xmax) / (45236.0*Math.sqrt(2*Math.PI*106)));
	    //double xconv = (xmax-xmin)/(width-60);
	    int stepSize = (width-60)/nbins;
	    int startW = 40 + stepSize/2;
	    double startX = xmin;
	    double startY = funk.integral(xmin,xmin+binWidth);
	    //double startY = funk.calculate(xmin);
	    g.setColor(funkColor);
	    if(logBaseY > 1){
		double yconvLog = (log(logBaseY,maxValue) - minValue)/(height-60);
		for(int i = 1; i < nbins; i++){
		    double endY = funk.integral(startX + binWidth,startX + 2*binWidth);
		    //double endY = funk.calculate(startX + binWidth);
		    int startH = height-40-(int)((log(logBaseY,(int)startY) - minValue)/yconvLog);
		    int endH = height-40-(int)((log(logBaseY,(int)endY) - minValue)/yconvLog);
		    g.drawLine(startW,startH,startW+stepSize,endH);
		    startX += binWidth;
		    startY = endY;
		    startW += stepSize;
		}
	    }
	    else{
		double yconv = ((double)(maxValue-minValue))/(height-60);
		for(int i = 1; i < nbins; i++){
		    double endY = funk.integral(startX + binWidth,startX + 2*binWidth);
		    //double endY = funk.calculate(startX + binWidth);
		    int startH = height-40-(int)((startY-minValue)/yconv);
		    int endH = height-40-(int)((endY-minValue)/yconv);
		    g.drawLine(startW,startH,startW+stepSize,endH);
		    startX += binWidth;
		    startY = endY;
		    startW += stepSize;
		}
	    }
	}
    }
}
