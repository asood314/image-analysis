import java.awt.Point;
import java.util.Vector;

public class TestAnalyzer extends ImageAnalysisToolkit
{
    private ImagePanel imPanel;

    public TestAnalyzer(NDImage im, ImageReport[] r, ImagePanel ip)
    {
	init(im,r);
	imPanel = ip;
    }

    public void standardAnalysis(int z, int t, int p){}
    
    public Mask findOutlierMask(int w, int z, int t, int p){ return null; }

    public Mask findBackgroundMask(int w, int z, int t, int p){ return null; }
    
    public Mask findSignalMask(int w, int z, int t, int p, Mask outMask){ return null; }

    public int findPuncta(int w, int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        Mask m = new Mask(r.getSignalMask(w));
        Mask m2 = r.getPunctaMask(w);
        m.add(m2,-1);
	imPanel.setMask(m);
	int[] seed = imPanel.imArgMax(true);
	Point lm = new Point(seed[0],seed[1]);
	int punctaDetectionWindow = 10;
	if(m.getValue(lm.x,lm.y) < 1){
	    System.out.println("Seed point not in signal region.");
	    return 0;
	}
	int x1 = Math.max(lm.x-punctaDetectionWindow,0);
	int x2 = Math.min(ndim.getWidth(),lm.x+punctaDetectionWindow);
	int y1 = Math.max(lm.y-punctaDetectionWindow,0);
	int y2 = Math.min(ndim.getHeight(),lm.y+punctaDetectionWindow);
	int nSignal = m.sum(x1,x2,y1,y2);
	while(nSignal < 10000){
	    x1 = Math.max(x1-punctaDetectionWindow,0);
	    x2 = Math.min(ndim.getWidth(),x2+punctaDetectionWindow);
	    y1 = Math.max(y1-punctaDetectionWindow,0);
	    y2 = Math.min(ndim.getHeight(),y2+punctaDetectionWindow);
	    nSignal = m.sum(x1,x2,y1,y2);
	}
	Point ul = new Point(x1,y1);
	Point lr = new Point(x2,y2);
	System.out.println("Clustering around point ("+lm.x+","+lm.y+") using local window from ("+ul.x+","+ul.y+") to ("+lr.x+","+lr.y+").");
        int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
        Mask used = new Mask(ndim.getWidth(),ndim.getHeight(),false);
        used.multiply(m);
	int Imax = ndim.getPixel(w,z,t,lm.x,lm.y,p);
	//double localMean = ndim.mean(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	double localMedian = ndim.median(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	double localStd = ndim.std(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	double minThreshold = localMedian + (Imax - localMedian)/2;
	double localThreshold = localMedian + 3*localStd;
	System.out.println("Imax: "+Imax+" Median: "+localMedian+" Threshold: "+localThreshold);
	if(Imax < localThreshold){
	    System.out.println("Not bright enough");
	    return 0;
	}
	localThreshold = Imax;
	Cluster c = new Cluster();
	Vector<Integer> borderX = new Vector<Integer>();
	Vector<Integer> borderY = new Vector<Integer>();
	borderX.addElement(lm.x);
	borderY.addElement(lm.y);
	used.setValue(lm.x,lm.y,0);
	double sumI = Imax;
	int npix = 1;
	int count = 0;
	Vector<Double> removalQueueI = new Vector<Double>();
	Vector<Integer> removalQueueN = new Vector<Integer>();
	for(int k = 0; k < 2; k++){
	    removalQueueI.addElement(0.0);
	    removalQueueN.addElement(0);
	}
	while(borderX.size() > 0){
	    sumI -= removalQueueI.elementAt(0);
	    npix -= removalQueueN.elementAt(0);
	    removalQueueI.remove(0);
	    removalQueueN.remove(0);
	    double upperLimit = sumI / npix;
	    if(count < 5) upperLimit = Imax;
	    int nborder = borderX.size();
	    double sumNew = 0.0;
	    int numNew = 0;
	    for(int b = 0; b < nborder; b++){
		int bi = borderX.elementAt(0);
		int bj = borderY.elementAt(0);
		c.addPixel(bi,bj);
		for(int k = 0; k < 8; k++){
		    int i = bi+diArr[k];
		    int j = bj+djArr[k];
		    localThreshold = minThreshold;// + Math.pow(0.95,(int)(Math.sqrt(i*i + j*j)))*(localThreshold - minThreshold);
		    try{
			//double val = (1-used.getValue(i,j))*((double)ndim.getPixel(w,z,t,i,j,p)) / Imax;
			//double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localMean) / (Imax - localMean);
			double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localThreshold) / (upperLimit - localThreshold);
			if(val < 0.001 || val > 1.0) continue;
			used.setValue(i,j,0);
			borderX.addElement(i);
			borderY.addElement(j);
			sumNew += ndim.getPixel(w,z,t,i,j,p);
			numNew++;
		    }
		    catch(ArrayIndexOutOfBoundsException e){ continue; }
		}
		//sumI -= ndim.getPixel(w,z,t,bi,bj,p);
		//npix--;
		borderX.remove(0);
		borderY.remove(0);
	    }
	    sumI += sumNew;
	    npix += numNew;
	    removalQueueI.addElement(sumNew);
	    removalQueueN.addElement(numNew);
	    count++;
	}
	if(c.size() < 10){
	    System.out.println("Cluster too small.");
	    return 0;
	}
	if(c.contains(c.getCentroid())){
	    if(c.size() > 5000){
		System.out.println("Found ridiculously large punctum of size " + c.size() + " seeded at (" + lm.x + "," + lm.y + ")");
		return 0;
	    }
	    r.addPunctum(w,c);
	    return 1;
	}
	System.out.println("Weird shape.");
	return 0;
    }
    
    public void findSynapses(int z, int t, int p)
    {
	int[] chan = {1,0};
	findSynapses(z,t,p,chan);
    }
    
    public void findSynapses(int z, int t, int p, int[] chan)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        int[] np = new int[chan.length];
        int maxIndex = 1;
	int nSynapses = 0;
	//int[] testX = {320,460};
	//int[] testY = {380,530};
	boolean verbose = false;
        for(int i = 0; i < chan.length; i++) np[i] = r.getNPuncta(chan[i]);
	for(int i = 1; i < chan.length; i++) maxIndex *= np[i];
        for(int j = 0; j < np[0]; j++){
	    Synapse best = null;
	    int bestScore = -999;
	    Cluster c = r.getPunctum(chan[0],j);
	    Point pt = c.getPixel(0);
	    verbose = false;
	    if(j == 125){//pt.x > 320 && pt.x < 460 && pt.y > 380 && pt.y < 530){
		verbose = true;
		System.out.println("PSD95 cluster " + j);
		System.out.println("Size: " + c.size() + ", Peak Location: " + pt.toString() + ", Peak Intensity: " + ndim.getPixel(chan[0],z,t,pt.x,pt.y,p));
	    }
	    for(int l = 0; l < maxIndex; l++){
		Synapse s = new Synapse(chan);
		s.addPunctum(r.getPunctum(chan[0],j),j);
		for(int i = 1; i < np.length; i++){
		    int index = l;
		    for(int k = 1; k < i; k++) index = index / np[k];
		    index = index % np[i];
		    c = r.getPunctum(chan[i],index);
		    s.addPunctum(r.getPunctum(chan[i],index),index);
		}
		int score = s.getColocalizationScore();
		pt = c.getPixel(0);
		if(verbose && l == 509){//verbose && pt.x > 320 && pt.x < 460 && pt.y > 380 && pt.y < 530){
		    System.out.println("VAMP cluster " + l);
		    System.out.println("Size: " + c.size() + ", Peak Location: " + pt.toString() + ", Peak Intensity: " + ndim.getPixel(chan[1],z,t,pt.x,pt.y,p));
		    System.out.println("Colocalization score: " + score);
		    System.out.println(s.getNPuncta());
		    System.out.println(s.getOverlap());
		    System.out.println(s.isColocalized());
		}
		if(score > bestScore){
		    bestScore = score;
		    best = s;
		}
	    }
	    if(best != null){
		if(verbose) System.out.println("Adding synapse");
		r.addSynapse(best);
		nSynapses++;
	    }
        }
	System.out.println("Found " + nSynapses + " synapses.");
    }    
}
