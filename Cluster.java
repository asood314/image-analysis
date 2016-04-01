import java.awt.Point;
import java.util.Vector;

public class Cluster
{
    private Vector<Point> pixels;
    private Point centroid;
    
    public Cluster()
    {
        pixels = new Vector<Point>();
        centroid = null;
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

    public void removePixel(int index){ pixels.remove(index); }
    
    public boolean contains(Point p){ return pixels.contains(p); }
    
    public int size(){ return pixels.size(); }
    
    public Point computeCentroid()
    {
        int x = 0;
        int y = 0;
        for(int i = 0; i < pixels.size(); i++){
            x += pixels.elementAt(i).getX();
            y += pixels.elementAt(i).getY();
        }
        x = x / pixels.size();
        y = y / pixels.size();
        centroid = new Point(x,y);
        return centroid;
    }
    
    public Point getCentroid()
    {
        if(centroid == null) return computeCentroid();
        return centroid;
    }
    
    public double distanceTo(Point cent2)
    {
        if(centroid == null) computeCentroid();
        return Math.sqrt(Math.pow(centroid.getX() - cent2.getX(),2) + Math.pow(centroid.getY() - cent2.getY(),2));
    }
    
    public double distanceTo(Cluster c2){ return distanceTo(c2.getCentroid()); }
    
    public int getOverlap(Cluster c2)
    {
        int overlap = 0;
        for(int i = 0; i < pixels.size(); i++){
            Point p = pixels.elementAt(i);
            for(int j = 0; j < c2.size(); j++){
                if(p.equals(c2.getPixel(j))){
                    overlap++;
                    break;
                }
            }
        }
        return overlap;
    }
    
    public int getOverlap(Cluster[] clusters)
    {
        int overlap = 0;
        for(int i = 0; i < pixels.size(); i++){
            Point p = pixels.elementAt(i);
            boolean overlapping = true;
            for(int j = 0; j < clusters.length; j++) overlapping = overlapping && clusters[j].contains(p);
            if(overlapping) overlap++;
        }
        return overlap;
    }
}
