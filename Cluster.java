import java.awt.Point;
import java.util.Vector;

public class Cluster extends LocalizedObject
{
    private Vector<Point> pixels;
    private int peakIntensity;
    private int totalIntensity;
    
    public Cluster()
    {
	super();
        pixels = new Vector<Point>();
	peakIntensity = 0;
	totalIntensity = 0;
    }
    
    public void addPixel(int x, int y){ pixels.add(new Point(x,y)); }
    
    public void addPixel(Point p){ pixels.add(p); }

    public void addPixel(int x, int y, int i)
    {
	pixels.add(new Point(x,y));
	totalIntensity += i;
    }
    
    public void addPixel(Point p, int i)
    {
	pixels.add(p);
	totalIntensity += i;
    }
    
    public void addPixelSafe(int x, int y)
    {
        Point p = new Point(x,y);
        if(!pixels.contains(p)) pixels.add(p);
    }
    
    public void addPixelSafe(Point p)
    {
        if(!pixels.contains(p)) pixels.add(p);
    }
    
    public Point getPixel(int index){ return pixels.elementAt(index); }

    public void setPixel(int index, Point p){ pixels.setElementAt(p,index); }

    public void removePixel(int index){ pixels.remove(index); }

    public void removePixel(Point p)
    {
	for(int i = 0; i < pixels.size(); i++){
	    Point p2 = pixels.elementAt(i);
	    if(p2.x == p.x && p2.y == p.y){
		pixels.remove(i);
		return;
	    }
	}
    }

    public void setPeakIntensity(int i){ peakIntensity = i; }

    public int getPeakIntensity(){ return peakIntensity; }

    public void setIntegratedIntensity(int i){ totalIntensity = i; }

    public int getIntegratedIntensity(){ return totalIntensity; }

    public Vector<Point> getPoints(){ return pixels; }
    
    public boolean contains(Point p)
    {
	for(int i = 0; i < pixels.size(); i++){
	    Point p2 = pixels.elementAt(i);
	    if(p2.x == p.x && p2.y == p.y){
		return true;
	    }
	}
	return false;
    }

    public int indexOf(Point p)
    {
	for(int i = 0; i < pixels.size(); i++){
	    Point p2 = pixels.elementAt(i);
	    if(p2.x == p.x && p2.y == p.y){
		return i;
	    }
	}
	return -1;
    }
    
    public int size(){ return pixels.size(); }
    
    public void computeCentroid()
    {
        int x = 0;
        int y = 0;
        for(int i = 0; i < pixels.size(); i++){
            x += pixels.elementAt(i).getX();
            y += pixels.elementAt(i).getY();
        }
        x = x / pixels.size();
        y = y / pixels.size();
        center = new Point(x,y);
    }
    
    public int getBorderLength(Cluster c)
    {
	int border = 0;
	for(int i = 0; i < pixels.size(); i++){
	    Point p = pixels.elementAt(i);
	    for(int j = 0; j < c.size(); j++){
		Point p2 = c.getPixel(j);
		if(Math.abs(p.x - p2.x) < 2 && Math.abs(p.y - p2.y) < 2){
		    border++;
		    break;
		}
	    }
	}
	return border;
    }

    public void add(Cluster c)
    {
	for(int i = 0; i < c.size(); i++) pixels.addElement(c.getPixel(i));
	if(c.getPeakIntensity() > peakIntensity) peakIntensity = c.getPeakIntensity();
	totalIntensity += c.getIntegratedIntensity();
    }

    public double peakToPeakDistance(Cluster c)
    {
	return Math.sqrt(peakToPeakDistance2(c));
    }

    public double peakToPeakDistance2(Cluster c)
    {
	Point pt = c.getPixel(0);
	Point pt2 = pixels.elementAt(0);
	return Math.pow(pt.x - pt2.x,2) + Math.pow(pt.y - pt2.y,2);
    }
}
