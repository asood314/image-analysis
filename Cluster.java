import java.awt.Point;
import java.util.Vector;

public class Cluster extends LocalizedObject
{
    private Vector<Point> pixels;
    
    public Cluster()
    {
	super();
        pixels = new Vector<Point>();
    }
    
    public void addPixel(int x, int y){ pixels.add(new Point(x,y)); }
    
    public void addPixel(Point p){ pixels.add(p); }
    
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
    }
}
