import java.awt.Point;
import java.util.Vector;

public class LocalizedObject
{
    protected Point center;

    public LocalizedObject()
    {
	center = null;
    }

    public Vector<Point> getPoints(){ return null; }

    public void computeCentroid(){}

    public Point getCentroid(){ return null; }

    public boolean contains(Point pt){ return false; }

    public int overlapWith(LocalizedObject obj)
    {
	int overlap = 0;
	Vector<Point> points = obj.getPoints();
	for(int i = 0; i < points.size(); i++){
	    if(contains(points.elementAt(i))) overlap++;
	}
	return overlap;
    }

    public static int findOverlap(LocalizedObject[] objs)
    {
	int overlap = 0;
	Vector<Point> points = objs[0].getPoints();
	for(int j = 0; j < points.size(); j++){
	    Point pt = points.elementAt(j);
	    boolean overlapping = true;
	    for(int i = 1; i < objs.length; i++) overlapping = overlapping && objs[i].contains(pt);
	    if(overlapping) overlap++;
	}
	return overlap;
    }

    public double distanceTo(LocalizedObject obj)
    {
	if(center == null) computeCentroid();
	Point p2 = obj.getCentroid();
	return Math.sqrt(Math.pow(center.x-p2.x,2) + Math.pow(center.y-p2.y,2));
    }

    public double distanceTo(Point pt)
    {
	if(center == null) computeCentroid();
	return Math.sqrt(Math.pow(center.x-pt.x,2) + Math.pow(center.y-pt.y,2));
    }

    public static double findMaxDistance(LocalizedObject[] objs)
    {
	double max = 0.0;
	for(int i = 0; i < objs.length; i++){
	    for(int j = i+1; j < objs.length; j++){
		double dist = objs[i].distanceTo(objs[j]);
		if(dist > max) max = dist;
	    }
	}
	return max;
    }
}