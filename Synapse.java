import java.awt.Point;
import java.util.Vector;

public class Synapse
{
    private int[] channels;
    private Vector<Cluster> puncta;
    private Vector<Integer> indices;
    private static int overlapThreshold;
    private static boolean useOverlapThreshold;
    private static double distanceThreshold;
    private static boolean useDistanceThreshold;
    private static boolean usePairwiseMeasure;
    
    public Synapse(int[] chan)
    {
        channels = chan;
        puncta = new Vector<Cluster>();
        indices = new Vector<Integer>();
        overlapThreshold = 0;
        useOverlapThreshold = true;
        distanceThreshold = 0;
        useDistanceThreshold = false;
        usePairwiseMeasure = false;
    }
    
    public int getNChannels(){ return channels.length; }
    
    public int getChannel(int index){ return channels[index]; }
    
    public void addPunctum(Cluster c, int index)
    {
        puncta.add(c);
        indices.add(index);
    }
    
    public Cluster getPunctum(int index){ return puncta.elementAt(index); }
    
    public int getPunctumIndex(int index){ return indices.elementAt(index); }
    
    public int getNPuncta(){ return puncta.size(); }
    
    public static void setOverlapThreshold(int t){ overlapThreshold = t; }
    
    public static int getOverlapThreshold(){ return overlapThreshold; }
    
    public static void setUseOverlapThreshold(boolean b){ useOverlapThreshold = b; }
    
    public static boolean getUseOverlapThreshold(){ return useOverlapThreshold; }
    
    public static void setDistanceThreshold(double t){ distanceThreshold = t; }
    
    public static double getDistanceThreshold(){ return distanceThreshold; }
    
    public static void setUseDistanceThreshold(boolean b){ useDistanceThreshold = b; }
    
    public static boolean getUseDistanceThreshold(){ return useDistanceThreshold; }
    
    public static void setUsePairwiseMeasure(boolean b){ usePairwiseMeasure = b; }
    
    public static boolean getUsePairwiseMeasure(){ return usePairwiseMeasure; }
    
    public int getOverlap(int index1, int index2)
    {
        if(index1 >= puncta.size() || index2 >= puncta.size()) return -1;
        return puncta.elementAt(index1).getOverlap(puncta.elementAt(index2));
    }

    public int getOverlap()
    {
        Cluster[] c = new Cluster[puncta.size()-1];
        for(int i = 1; i < puncta.size(); i++) c[i-1] = puncta.elementAt(i);
        return puncta.elementAt(0).getOverlap(c);
    }

    public int size(){
	int sum = 0;
	for(int i = 0; i < puncta.size(); i++) sum += puncta.elementAt(i).size();
	return sum;
    }

    public double getDistance(int index1, int index2)
    {
        if(index1 >= puncta.size() || index2 >= puncta.size()) return -1;
        return puncta.elementAt(index1).distanceTo(puncta.elementAt(index2));
    }
    
    public Point getCenter()
    {
        int centerX = 0;
        int centerY = 0;
        for(int i = 0; i < puncta.size(); i++){
            Point p = puncta.elementAt(i).getCentroid();
            centerX += p.getX();
            centerY += p.getY();
        }
        return new Point(centerX/puncta.size(), centerY/puncta.size());
    }
    
    public double distanceTo(Point p)
    {
        Point c = getCenter();
        return Math.sqrt(Math.pow(c.getX() - p.getX(),2) + Math.pow(c.getY() - p.getY(),2));
    }
    
    public double distanceTo(int x, int y)
    {
        Point c = getCenter();
        return Math.sqrt(Math.pow(c.getX() - x,2) + Math.pow(c.getY() - y,2));
    }
    
    public boolean isColocalized(int index1, int index2)
    {
        if(index1 >= puncta.size() || index2 >= puncta.size()) return false;
        if(useOverlapThreshold) return getOverlap(index1,index2) > overlapThreshold;
        if(useDistanceThreshold) return getDistance(index1,index2) < distanceThreshold;
        return false;
    }
    
    public boolean isColocalized()
    {
        if(usePairwiseMeasure){
            for(int i = 0; i < puncta.size(); i++){
                for(int j = i+1; j < puncta.size(); j++){
                    if(!(isColocalized(i,j))) return false;
                }
            }
            return true;
        }
        if(useOverlapThreshold){
            //System.out.println(getOverlap());
            return getOverlap() > overlapThreshold;
        }
        if(useDistanceThreshold){
            Point c = getCenter();
            for(int i = 0; i < puncta.size(); i++){
                if(puncta.elementAt(i).distanceTo(c) >= distanceThreshold) return false;
            }
            return true;
        }
        return false;
    }

    public int getColocalizationScore(int index1, int index2)
    {
        if(index1 >= puncta.size() || index2 >= puncta.size()) return -999;
        if(useOverlapThreshold){
	    int over = getOverlap(index1,index2);
	    if(over > overlapThreshold) return over;
	    return -99999;
	}
        if(useDistanceThreshold){
	    int dist = (int)getDistance(index1,index2);
	    if(dist >= distanceThreshold) return -999;
	    return dist;
	}
        return -99999;
    }

    public int getColocalizationScore()
    {
	if(usePairwiseMeasure){
	    int score = 0;
            for(int i = 0; i < puncta.size(); i++){
                for(int j = i+1; j < puncta.size(); j++){
                    score += getColocalizationScore(i,j);
                }
            }
            return score;
        }
        if(useOverlapThreshold){
            //System.out.println(getOverlap());
            int over = getOverlap();
	    if(over > overlapThreshold) return over;
	    return -999;
        }
        if(useDistanceThreshold){
            Point c = getCenter();
	    int score = 0;
            for(int i = 0; i < puncta.size(); i++){
                int dist = (int)puncta.elementAt(i).distanceTo(c);
		if(dist >= distanceThreshold) return -999;
		score -= dist;
            }
            return score;
        }
        return -999;
    }
}
