import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.util.Vector;
import javax.swing.JPanel;

public class ImagePanel extends JPanel
{
    public static final int GRAY_16 = 0;
    public static final int GRAY_8 = 1;
    public static final int RGB_8 = 2;
    public static final int SPLIT = 3;
    
    private NDImage ndim;
    private BufferedImage displayImage;
    private int mode;
    private int wavelength;
    private int zslice;
    private int timepoint;
    private int position;
    private int[] channels;
    private Mask imMask;
    private int maskColor;
    private Vector<Mask> imMasks;
    private Vector<Integer> maskColors;
    private double zoom;
    private int wpixMin,wpixMax;
    private int[] cpixMin;
    private int[] cpixMax;
    private int[] displayRegion;
    private boolean zoomToRegion,displayRegionSet;
    
    public ImagePanel(NDImage n)
    {
        super();
        ndim = n;
	displayImage = null;
        mode = GRAY_8;
        channels = new int[3];
        cpixMin = new int[3];
        cpixMax = new int[3];
        for(int i = 0; i < 3; i++){
            channels[i] = -1;
            cpixMin[i] = 0;
            cpixMax[i] = 65535;
        }
        wavelength = 0;
        zslice = 0;
        timepoint = 0;
        position = 0;
        imMask = null;
        maskColor = 0xffff00ff;
	imMasks = new Vector<Mask>();
	maskColors = new Vector<Integer>();
	maskColors.addElement(0xffff0000);
	maskColors.addElement(0xff00ff00);
	maskColors.addElement(0xff0000ff);
        zoom = 1.0;
        displayRegion = new int[4];
        displayRegion[0] = 0;
        displayRegion[1] = 0;
        displayRegion[2] = ndim.getWidth();
        displayRegion[3] = ndim.getHeight();
	zoomToRegion = false;
	displayRegionSet = false;
        int[] minMax = ndim.getMinMax(wavelength,zslice,timepoint,position,displayRegion);
        wpixMin = minMax[0];
        wpixMax = minMax[1];
        setPreferredSize(new Dimension(ndim.getWidth(),ndim.getHeight()));
    }
    
    public ImagePanel(NDImage n, int[] chan)
    {
        super();
        ndim = n;
	displayImage = null;
        mode = RGB_8;
        if(chan.length < 3){
            channels = new int[3];
            for(int i = 0; i < chan.length; i++) channels[i] = chan[i];
            for(int i = chan.length; i < 3; i++) channels[i] = -1;
        }
        else channels = chan;
        cpixMin = new int[channels.length];
        cpixMax = new int[channels.length];
        wavelength = 0;
        zslice = 0;
        timepoint = 0;
        position = 0;
        imMask = null;
        maskColor = 0xffffffff;
	imMasks = new Vector<Mask>();
	maskColors = new Vector<Integer>();
	maskColors.addElement(0xffffffff);
	maskColors.addElement(0xffff00ff);
	maskColors.addElement(0xff00ffff);
        zoom = 1.0;
        displayRegion = new int[4];
        displayRegion[0] = 0;
        displayRegion[1] = 0;
        displayRegion[2] = ndim.getWidth();
        displayRegion[3] = ndim.getHeight();
	zoomToRegion = false;
	displayRegionSet = false;
        int[] minMax = ndim.getMinMax(wavelength,zslice,timepoint,position,displayRegion);
        wpixMin = minMax[0];
        wpixMax = minMax[1];
        minMax = ndim.getMinMax(channels,zslice,timepoint,position,displayRegion);
        for(int i = 0; i < cpixMin.length; i++){
            cpixMin[i] = minMax[2*i];
            cpixMax[i] = minMax[2*i+1];
        }
        setPreferredSize(new Dimension(ndim.getWidth(),ndim.getHeight()));
    }
    
    protected void paintComponent(Graphics g)
    {
        if(ndim == null) return;
	if(displayImage == null) updateImage();
	double aspectRatio = ((double)ndim.getWidth()) / ndim.getHeight();
        if(zoomToRegion) aspectRatio = ((double)displayRegion[2]) / displayRegion[3];
        int imWidth = (int)(ndim.getHeight() * aspectRatio * zoom);
        int imHeight = (int)(ndim.getHeight() * zoom);
	try{
	    if(mode == SPLIT) setPreferredSize(new Dimension(2*imWidth,2*imHeight));
	    else setPreferredSize(new Dimension(imWidth,imHeight));
	    g.drawImage(displayImage.getScaledInstance(imWidth,imHeight,Image.SCALE_SMOOTH),0,0,this);
	    if(!zoomToRegion && displayRegionSet){
		g.setColor(Color.white);
		g.drawRect((int)(displayRegion[0]*zoom),(int)(displayRegion[1]*zoom),(int)(displayRegion[2]*zoom),(int)(displayRegion[3]*zoom));
	    }
	    revalidate();
	}
	catch(Exception e){ e.printStackTrace(); }
    }

    private void updateImage()
    {
	if(zoomToRegion){
	    if(mode == GRAY_8) displayImage = ndim.getGrayImage(wavelength,zslice,timepoint,position,wpixMin,wpixMax,displayRegion);
	    else if(mode == RGB_8) displayImage = ndim.getRGBImage(channels,zslice,timepoint,position,cpixMin,cpixMax,displayRegion);
	    else if(mode == SPLIT) displayImage = ndim.getSplitImage(channels,zslice,timepoint,position,cpixMin,cpixMax,displayRegion);
	    else return;
	}
	else{
	    if(mode == GRAY_8) displayImage = ndim.getGrayImage(wavelength,zslice,timepoint,position,wpixMin,wpixMax);
	    else if(mode == RGB_8) displayImage = ndim.getRGBImage(channels,zslice,timepoint,position,cpixMin,cpixMax);
	    else if(mode == SPLIT) displayImage = ndim.getSplitImage(channels,zslice,timepoint,position,cpixMin,cpixMax);
	    else return;
	}
	if(imMasks.size() > 0){
	    for(int i = 0; i < displayImage.getWidth(); i++){
		for(int j = 0; j < displayImage.getHeight(); j++){
		    int color = 0x00000000;
		    for(int k = 0; k < imMasks.size(); k++){
			if(imMasks.elementAt(k).getValue(i,j) > 0) color = color | maskColors.elementAt(k % maskColors.size());
		    }
		    if(color != 0) displayImage.setRGB(i,j,color);
		}
	    }
	}
    }
    
    public void setImage(NDImage im){
        ndim = im;
        updateImage();
    }

    public void setDisplayImage(BufferedImage bim)
    {
	displayImage = bim;
	repaint();
    }
    
    public void setMode(int m){
	mode = m;
	updateImage();
    }
    
    public int getMode(){ return mode; }
    
    public void setChannels(int[] chan)
    {
        if(chan.length < 3){
            channels = new int[3];
            for(int i = 0; i < chan.length; i++) channels[i] = chan[i];
            for(int i = chan.length; i < 3; i++) channels[i] = -1;
        }
        channels = chan;
    }
    
    public void setChannel(int rgb, int wl){ channels[rgb] = wl; }
    
    public int getChannel(int chan){ return channels[chan]; }
    
    public int findChannel(int wl){
        for(int i = 0; i < channels.length; i++){
            if(channels[i] == wl) return i;
        }
        return -1;
    }
    
    public int getNChannels(){ return channels.length; }
    
    public void setMask(Mask m){ imMask = m; }
    
    public void setMaskColor(int color){ maskColor = color; }

    public void addMask(Mask m){
	imMasks.addElement(m);
	updateImage();
    }

    public void toggleMask(Mask m)
    {
	for(int i = 0; i < imMasks.size(); i++){
	    if(imMasks.elementAt(i).equals(m)){
		imMasks.remove(i);
		updateImage();
		return;
	    }
	}
	imMasks.addElement(m);
	updateImage();
    }

    public void clearMasks(){
	imMasks.clear();
	updateImage();
    }

    public void setMaskColor(int index, int color){
	maskColors.set(index,color);
	updateImage();
    }

    public void addMaskColor(int color){ maskColors.addElement(color); }

    public void clearMaskColors(){ maskColors.clear(); }

    public void removeMask(Mask m){
	imMasks.remove(m);
	updateImage();
    }
    
    public void setZoom(double z){
	zoom = z;
	updateImage();
    }
    
    public double getZoom(){ return zoom; }

    public void setZoomToRegion(boolean tf){
	zoomToRegion = tf;
	updateImage();
    }
    
    public void unzoom()
    {
        displayRegion[0] = 0;
        displayRegion[1] = 0;
        displayRegion[2] = ndim.getWidth();
        displayRegion[3] = ndim.getHeight();
        zoom = 1.0;
	zoomToRegion = false;
	displayRegionSet = false;
	updateImage();
    }
    
    public int getWavelength(){ return wavelength; }
    
    public int getZSlice(){ return zslice; }
    
    public int getTimepoint(){ return timepoint; }
    
    public int getPosition(){ return position; }
    
    public void setWavelength(int w)
    {
        wavelength = w;
        int chan = findChannel(w);
        if(chan >= 0){
            wpixMin = cpixMin[chan];
            wpixMax = cpixMax[chan];
        }
	updateImage();
    }
    
    public void setZSlice(int z){
	zslice = z;
	updateImage();
    }
    
    public void setTimepoint(int t){
	timepoint = t;
	updateImage();
    }
    
    public void setPosition(int pos){
	position = pos;
	updateImage();
    }
    
    public void autoScale()
    {
        if(mode == GRAY_8){
            int[] minMax = ndim.getMinMax(wavelength,zslice,timepoint,position,displayRegion);
            wpixMin = minMax[0];
            wpixMax = minMax[1];
        }
        else{
            int[] minMax = ndim.getMinMax(channels,zslice,timepoint,position,displayRegion);
            for(int i = 0; i < cpixMin.length; i++){
                cpixMin[i] = minMax[2*i];
                cpixMax[i] = minMax[2*i+1];
            }
        }
	updateImage();
    }
    
    public void setWPixMin(int min){
	wpixMin = min;
	updateImage();
    }
    
    public void setWPixMax(int max){
	wpixMax = max;
	updateImage();
    }
    
    public void setCPixMin(int chan, int min){
	cpixMin[chan] = min;
	updateImage();
    }
    
    public void setCPixMax(int chan, int max){
	cpixMax[chan] = max;
	updateImage();
    }
    
    public int getWPixMin(){ return wpixMin; }
    
    public int getWPixMax(){ return wpixMax; }
    
    public int getCPixMin(int chan){ return cpixMin[chan]; }
    
    public int getCPixMax(int chan){ return cpixMax[chan]; }
    
    public void setDisplayRegion(int left, int top, int width, int height)
    {
        displayRegion[0] = left;
        displayRegion[1] = top;
        displayRegion[2] = width;
        displayRegion[3] = height;
	displayRegionSet = true;
	updateImage();
    }

    public int[] getDisplayRegion(){ return displayRegion; }

    public int getImPixel(int i, int j, boolean maskIn)
    {
	if(imMasks.size() > 0){
	    if(maskIn) return imMasks.elementAt(0).getValue(i,j) * ndim.getPixel(wavelength,zslice,timepoint,i,j,position);
	    else return (1-imMasks.elementAt(0).getValue(i,j)) * ndim.getPixel(wavelength,zslice,timepoint,i,j,position);
	}
	return ndim.getPixel(wavelength,zslice,timepoint,i,j,position);
    }

    public int imMin(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.min(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.min(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.min(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public int imMax(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.max(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.max(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.max(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public int[] imArgMax(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.argmax(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.argmax(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.argmax(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public double imMean(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.mean(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.mean(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.mean(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public double imMedian(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.median(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.median(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.median(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public double imPercentile(double frac, boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.percentile(frac,wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.percentile(frac,wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.percentile(frac,wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public double imStd(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.std(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.std(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.std(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public double imMode(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.mode(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.mode(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.mode(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }

    public int[] getDistribution(boolean maskIn)
    {
	int x1 = displayRegion[0];
	int x2 = displayRegion[0]+displayRegion[2];
	int y1 = displayRegion[1];
	int y2 = displayRegion[1]+displayRegion[3];
	if(imMasks.size() > 0){
	    if(maskIn) return ndim.getDistribution(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0));
	    else return ndim.getDistribution(wavelength,zslice,timepoint,position,x1,x2,y1,y2,imMasks.elementAt(0).getInverse());
	}
	return ndim.getDistribution(wavelength,zslice,timepoint,position,x1,x2,y1,y2);
    }
}
