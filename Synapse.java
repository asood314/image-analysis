import java.awt.Point;
import java.util.Vector;

public class Synapse extends LocalizedObject
{
    private Vector<Cluster> puncta;
    private Vector<Integer> indices;
    private double colocalizationScore;
    
    public Synapse()
    {
	super();
        puncta = new Vector<Cluster>();
        indices = new Vector<Integer>();
	colocalizationScore = 0.0;
    }
    
    public void addPunctum(Cluster c, int index)
    {
        puncta.add(c);
        indices.add(index);
    }
    
    public Cluster getPunctum(int index){ return puncta.elementAt(index); }
    
    public int getPunctumIndex(int index){ return indices.elementAt(index); }
    
    public int getNPuncta(){ return puncta.size(); }

    public Vector<Point> getPoints()
    {
	Vector<Point> retVal = new Vector<Point>();
	for(int i = 0; i < puncta.size(); i++){
	    Cluster c = puncta.elementAt(i);
	    for(int j = 0; j < c.size(); j++) retVal.add(c.getPixel(j));
	}
	return retVal;
    }

    public void setColocalizationScore(double score){ colocalizationScore = score; }

    public double getColocalizationScore(){ return colocalizationScore; }
    
    public int getClusterOverlap()
    {
        Cluster[] c = new Cluster[puncta.size()];
        for(int i = 0; i < c.length; i++) c[i] = puncta.elementAt(i);
        return LocalizedObject.findOverlap(c);
    }

    public int getClusterOverlap(int[] indices)
    {
        Cluster[] c = new Cluster[indices.length];
        for(int i = 0; i < indices.length; i++) c[i] = puncta.elementAt(indices[i]);
        return LocalizedObject.findOverlap(c);
    }

    public int size(){
	int sum = 0;
	for(int i = 0; i < puncta.size(); i++) sum += puncta.elementAt(i).size();
	return sum;
    }

    public double getMaxClusterDistance(int[] indices)
    {
        Cluster[] c = new Cluster[indices.length];
        for(int i = 0; i < indices.length; i++) c[i] = puncta.elementAt(indices[i]);
        return LocalizedObject.findMaxDistance(c);
    }
    
    public void computeCentroid()
    {
        int centerX = 0;
        int centerY = 0;
        for(int i = 0; i < puncta.size(); i++){
            Point p = puncta.elementAt(i).getCentroid();
            centerX += p.getX();
            centerY += p.getY();
        }
        center = new Point(centerX/puncta.size(), centerY/puncta.size());
    }
    
    public double distanceTo(int x, int y)
    {
        return Math.sqrt(Math.pow(center.x - x,2) + Math.pow(center.y - y,2));
    }
    
    public boolean isColocalized()
    {
	if(getColocalizationScore() > 0.0) return true;
	return false;
    }
}
