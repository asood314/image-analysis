import java.awt.Point;
import java.util.Vector;

public class FixedCellAnalyzer extends ImageAnalysisToolkit
{
    private int signalDetectionWindow;
    private int punctaDetectionWindow;
    private int punctaFindingIterations;
    private double punctaAreaThreshold;
    private int masterChannel;
    
    public FixedCellAnalyzer(NDImage im, ImageReport[] r)
    {
	init(im,r);
        signalDetectionWindow = 50*(ndim.getWidth()/1000);
        punctaDetectionWindow = 5*(ndim.getWidth()/1000);
	punctaFindingIterations = 10;
	masterChannel = -1;
    }
    
    public void setSignalDetectionWindow(int sdt){ signalDetectionWindow = sdt; }
    
    public void setPunctaDetectionWindow(int pdt){ punctaDetectionWindow = pdt; }

    public void setMasterChannel(int chan){ masterChannel = chan; }

    public void standardAnalysis(int z, int t, int p)
    {
	//System.out.println("Start.");
        int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
        if(reports[index] == null){
	    reports[index] = new ImageReport(ndim.getNWavelengths(),ndim.getWidth(),ndim.getHeight());
	    reports[index].setResolutionXY(resolutionXY);
	}
	long t1 = System.currentTimeMillis();
	double meanPost = 0.0;
	double modePost = 0.0;
	int npost = 0;
	if(masterChannel < 0){
	    for(int w = 0; w < ndim.getNWavelengths(); w++){
		Mask m = findBackgroundMask(w,z,t,p);
		reports[index].setOutlierMask(w,m.getInverse());
		Mask m2 = findSignalMask(w,z,t,p,m);
		reports[index].setSignalMask(w,m2);
		if(prePost[w] > 0){
		    meanPost += ndim.mean(w,z,t,p,m2);
		    modePost += ndim.mode(w,z,t,p,m2);
		    npost++;
		}
	    }
	}
	else{
	    Mask m = findBackgroundMask(masterChannel,z,t,p);
	    Mask m2 = findSignalMask(masterChannel,z,t,p,m);
	    m = m.getInverse();
	    for(int w = 0; w < ndim.getNWavelengths(); w++){
		double mean = ndim.mean(w,z,t,p,m2);
		double std = ndim.std(w,z,t,p,m2);
		double threshold = mean + std;
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			if(ndim.getPixel(w,z,t,i,j,p) > threshold) m2.setValue(i,j,1);
		    }
		}
	    }
	    for(int w = 0; w < ndim.getNWavelengths(); w++){
		reports[index].setOutlierMask(w,m);
		reports[index].setSignalMask(w,m2.getCopy());
		if(prePost[w] > 0){
		    meanPost += ndim.mean(w,z,t,p,m2);
		    modePost += ndim.mode(w,z,t,p,m2);
		    npost++;
		}
	    }
	}
	double thresholdPost = (meanPost+modePost)/(2*npost);
	Mask[] masks = reports[index].getSignalMasks();
	for(int i = 0; i < ndim.getWidth(); i++){
	    for(int j = 0; j < ndim.getHeight(); j++){
		int isSignal = 1;
		for(int w = 0; w < ndim.getNWavelengths(); w++){
		    isSignal *= masks[w].getValue(i,j);
		}
		if(isSignal < 1) continue;
		int maxPost = 0;
		int maxPre = 0;
		for(int w = 0; w < ndim.getNWavelengths(); w++){
		    int value = ndim.getPixel(w,z,t,i,j,p);
		    if(prePost[w] > 0){
			if(value > maxPost) maxPost = value;
		    }
		    else if(prePost[w] < 0){
			if(value > maxPre) maxPre = value;
		    }
		}
		if(maxPost > thresholdPost) continue;
		if(maxPre > maxPost){
		    for(int w = 0; w < ndim.getNWavelengths(); w++){
			if(prePost[w] > 0) masks[w].setValue(i,j,0);
		    }
		}
	    }
	}
	long t2 = System.currentTimeMillis();
	long diff = (t2 - t1) / 1000;
	System.out.println("Done signal finding in "+(diff/60)+" minutes and "+(diff%60)+" seconds.");
	t1 = t2;
	punctaAreaThreshold = 0.07/Math.pow(resolutionXY,2);
	for(int w = 0; w < ndim.getNWavelengths(); w++){
	    //System.out.println("Finding puncta for channel "+w);
	    findSaturatedPuncta(w,z,t,p);
	    //System.out.println("Done saturated puncta.");
	    for(int i = 0; i < punctaFindingIterations; i++){
		for(int j = reports[index].getNPuncta(w)-1; j >= 0; j--){
		    Cluster c = reports[index].getPunctum(w,j);
		    Point pt = c.getPixel(0);
		    if(ndim.getPixel(w,z,t,pt.x,pt.y,p) < saturationThreshold && c.size() < 1.5*punctaAreaThreshold) reports[index].removePunctum(w,j);
		}
		int n = findPuncta(w,z,t,p);
		//System.out.println("Iteration " + i + " found " + n + " puncta in channel " + w + ".");
		if(n == 0) break;
	    }
	    //System.out.println("Resolving overlaps");
	    resolveOverlaps(w,z,t,p);
	    t2 = System.currentTimeMillis();
	    diff = (t2 - t1) / 1000;
	    System.out.println("Found " + reports[index].getNPuncta(w) + " puncta in channel " + w + " in "+(diff/60)+" minutes and "+(diff%60)+" seconds.");
	    t1 = t2;
	}
	//System.out.println("Done puncta finding.");
	int nSynapses = findSynapses(z,t,p);
	t2 = System.currentTimeMillis();
	diff = (t2 - t1) / 1000;
	System.out.println("Found " + nSynapses + " synapses in "+(diff/60)+" minutes and "+(diff%60)+" seconds.");
    }
    
    public Mask findOutlierMask(int w, int z, int t, int p)
    {
        double mean = ndim.median(w,z,t,p);
        double std = ndim.std(w,z,t,p);
        Mask m = new Mask(ndim.getWidth(),ndim.getHeight(),false);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                if(ndim.getPixel(w,z,t,i,j,p) > mean + 10*std) m.setValue(i,j,0);
            }
        }
        double std2 = ndim.std(w,z,t,p,m);
        while(std2/std < 0.9){
            std = std2;
            for(int i = 0; i < ndim.getWidth(); i++){
                for(int j = 0; j < ndim.getHeight(); j++){
                    if(ndim.getPixel(w,z,t,i,j,p) > mean + 10*std) m.setValue(i,j,0);
                }
            }
            std2 = ndim.std(w,z,t,p,m);
        }
        return m;
    }

    public Mask findBackgroundMask(int w, int z, int t, int p)
    {
	double mode = ndim.mode(w,z,t,p);
	double median = ndim.median(w,z,t,p);
	double mean = ndim.mean(w,z,t,p);
	double threshold = Math.min(2.0 * mode,(mode + median) / 2) + 1.0;
	Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
	int[] dist = new int[65536];
	for(int i = 20; i < ndim.getWidth()-20; i++){
	    int x1 = i-20;
	    int x2 = i+20;
	    for(int k = 0; k < 65536; k++) dist[k] = 0;
	    int n = 0;
	    for(int k = x1; k < x2; k++){
		for(int j = 0; j < 40; j++){
		    int a = ndim.getPixel(w,z,t,k,j,p);
		    dist[a]++;
		    n++;
		}
	    }
	    int sum = 0;
	    double target = n/2.0;
	    int index;
	    for(index = 0; sum < target; index++) sum += dist[index];
	    int lmedian = index-1;
	    if(lmedian < threshold) m.setValue(i,19,1);
	    for(int j = 20; j < ndim.getHeight()-20; j++){
		int y1 = j-20;
		int y2 = j+20;
		for(int k = x1; k < x2; k++){
		    int val1 = ndim.getPixel(w,z,t,k,y1,p);
		    int val2 = ndim.getPixel(w,z,t,k,y2,p);
		    dist[val1]--;
		    dist[val2]++;
		    if(val1 <= lmedian) sum--;
		    if(val2 <= lmedian) sum++;
		}
		while(sum > target){
		    sum -= dist[lmedian];
		    lmedian--;
		}
		while(sum < target){
		    lmedian++;
		    sum += dist[lmedian];
		}
		if(lmedian < threshold) m.setValue(i,j,1);
		//double lmode = ndim.mode(w,z,t,p,i-20,i+20,j-20,j+20);
		//if(lmode/mode > 2) continue;
		//double lmedian = ndim.median(w,z,t,p,i-20,i+20,j-20,j+20);
		//if(lmedian/lmode < 1.5) m.setValue(i,j,1);
	    }
	}
	return m;
    }

    public Mask findSignalMask(int w, int z, int t, int p, Mask bkgdMask)
    {
	Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
	double lowerLimit = ndim.mode(w,z,t,p);
	double best = ndim.mean(w,z,t,p);
	double upperLimit = best + 6*ndim.std(w,z,t,p);
	double step = (upperLimit - lowerLimit) / 10;
	int nsteps = 10;
	if(step < 1.0){
	    step = 1.0;
	    nsteps = (int)(upperLimit - lowerLimit);
	}
	double fom = 0;
	boolean finished = false;
	while(!finished){
	    finished = true;
	    //System.out.println(""+lowerLimit+", "+upperLimit);
	    for(int istep = 0; istep < nsteps; istep++){
		double globalThreshold = lowerLimit + istep*step;
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			int value = ndim.getPixel(w,z,t,i,j,p);
			if(value > globalThreshold){
			    m.setValue(i,j,1);
			}
			else m.setValue(i,j,0);
		    }
		}
		//int maxSize = 0;
		int avgSigSize = 0;
		int nSigClusters = 0;
		Vector<Integer> borderX = new Vector<Integer>(10);
		Vector<Integer> borderY = new Vector<Integer>(10);
		Mask subMask = new Mask(m);
		//Cluster maxCluster = null;
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			if(subMask.getValue(i,j) != 1) continue;
			borderX.addElement(i);
			borderY.addElement(j);
			subMask.setValue(i,j,0);
			//Cluster c = new Cluster();
			//c.addPixel(i,j);
			int size = 1;
			while(borderX.size() > 0){
			    int bi = borderX.elementAt(0);
			    int bj = borderY.elementAt(0);
			    for(int di = -1; di < 2; di++){
				for(int dj = -1; dj < 2; dj++){
				    try{
					int val = m.getValue(bi+di,bj+dj);
					if(val > 0 && subMask.getValue(bi+di,bj+dj) == 1){
					    subMask.setValue(bi+di,bj+dj,0);
					    borderX.addElement(bi+di);
					    borderY.addElement(bj+dj);
					    //c.addPixel(bi+di,bj+dj);
					    size++;
					}
				    }
				    catch(ArrayIndexOutOfBoundsException e){ continue; }
				}
			    }
			    borderX.remove(0);
			    borderY.remove(0);
			}
			//if(size > maxSize){
			//  maxSize = size;
			//  maxCluster = c;
			//}
			avgSigSize += size;
			nSigClusters++;
		    }
		}
		/*
		int perimeter = 0;
		for(int i = 0; i < maxCluster.size(); i++){
		    Point pt = maxCluster.getPixel(i);
		    int localSum = 0;
		    for(int di = -1; di < 2; di++){
			for(int dj = -1; dj < 2; dj++){
			    try{ localSum += m.getValue(pt.x+di,pt.y+dj); }
			    catch(ArrayIndexOutOfBoundsException e){ continue; }
			}
		    }
		    if(localSum < 9) perimeter++;
		}
		*/
		int avgSize = 0;
		int nBkgClusters = 0;
		subMask = new Mask(m);
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			if(subMask.getValue(i,j) != 0) continue;
			borderX.addElement(i);
			borderY.addElement(j);
			subMask.setValue(i,j,1);
			int size = 1;
			while(borderX.size() > 0){
			    int bi = borderX.elementAt(0);
			    int bj = borderY.elementAt(0);
			    for(int di = -1; di < 2; di++){
				for(int dj = -1; dj < 2; dj++){
				    try{
					int val = m.getValue(bi+di,bj+dj);
					if(val < 1 && subMask.getValue(bi+di,bj+dj) == 0){
					    subMask.setValue(bi+di,bj+dj,1);
					    borderX.addElement(bi+di);
					    borderY.addElement(bj+dj);
					    size++;
					}
				    }
				    catch(ArrayIndexOutOfBoundsException e){ continue; }
				}
			    }
			    borderX.remove(0);
			    borderY.remove(0);
			}
			avgSize += size;
			nBkgClusters++;
		    }
		}
		double ifom = ((double)avgSigSize)*avgSize;
		if(nBkgClusters > 0) ifom = ifom/nBkgClusters;
		else ifom = 0;
		if(nSigClusters > 0) ifom = ifom/nSigClusters;
		else ifom = 0;
		if(ifom > fom){
		    fom = ifom;
		    best = globalThreshold;
		    finished = false;
		}
	    }
	    lowerLimit = best - step;
	    upperLimit = best + step;
	    step = (upperLimit - lowerLimit) / 10;
	    if(step < 1.0){
		step = 1.0;
		nsteps = (int)(upperLimit - lowerLimit);
	    }
	}
	//System.out.println("Best threshold: "+best);
	return findMaskWithThreshold(w,z,t,p,best);
    }
    
    //public Mask findSignalMask(int w, int z, int t, int p, Mask bkgdMask)
    public Mask findMaskWithThreshold(int w, int z, int t, int p, double globalThreshold)
    {
	Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
	//double stdBkg = ndim.std(w,z,t,p,bkgdMask);
	//double globalThreshold = ndim.mean(w,z,t,p,bkgdMask) + 5*stdBkg;
	for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
		int value = ndim.getPixel(w,z,t,i,j,p);
		if(value > globalThreshold){
		    m.setValue(i,j,1);
		}
	    }
	}
	double sizeThreshold = 0.25/Math.pow(resolutionXY,2);
	Vector<Integer> borderX = new Vector<Integer>(10);
        Vector<Integer> borderY = new Vector<Integer>(10);
	int[] clusterX = new int[4200000];
	int[] clusterY = new int[4200000];
	Mask used = new Mask(m);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                if(used.getValue(i,j) != 1) continue;
		clusterX[0] = i;
		clusterY[0] = j;
                borderX.addElement(i);
                borderY.addElement(j);
                //Mask subMask = new Mask(ndim.getWidth(),ndim.getHeight());
                //subMask.setValue(i,j,1);
		used.setValue(i,j,0);
                int size = 1;
                while(borderX.size() > 0){
                    int bi = borderX.elementAt(0);
                    int bj = borderY.elementAt(0);
                    for(int di = -1; di < 2; di++){
                        for(int dj = -1; dj < 2; dj++){
                            try{
                                int val = used.getValue(bi+di,bj+dj);
                                if(val > 0){// && subMask.getValue(bi+di,bj+dj) == 0){
                                    //subMask.setValue(bi+di,bj+dj,1);
				    used.setValue(bi+di,bj+dj,0);
				    clusterX[size] = bi+di;
				    clusterY[size] = bj+dj;
                                    borderX.addElement(bi+di);
                                    borderY.addElement(bj+dj);
                                    size++;
                                }
                            }
                            catch(ArrayIndexOutOfBoundsException e){ continue; }
                        }
                    }
                    borderX.remove(0);
                    borderY.remove(0);
                }
                //subMask.multiply(size);
                //m.add(subMask);
		if(size < sizeThreshold){
		    for(int k = 0; k < size; k++){
			//m.setValue(clusterX.elementAt(k),clusterY.elementAt(k),size);
			m.setValue(clusterX[k],clusterY[k],0);
		    }
		}
		//System.out.println("Added cluster: " + i + "," + j+", " + size);
            }
        }
	//System.out.println("Thresholding");
        //m.threshold((int)(0.25/Math.pow(resolutionXY,2)));
	//System.out.println("Filling in blanks");
        for(int i = 1; i < ndim.getWidth()-1; i++){
            for(int j = 1; j < ndim.getHeight()-1; j++){
                int sum = m.getValue(i-1,j-1) + m.getValue(i,j-1) + m.getValue(i+1,j-1) + m.getValue(i-1,j) + m.getValue(i+1,j) + m.getValue(i-1,j+1) + m.getValue(i,j+1) + m.getValue(i+1,j+1);
                if(sum > 4) m.setValue(i,j,1);
            }
        }
	return m;
    }
    
    public int findPuncta(int w, int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        Mask m = new Mask(r.getSignalMask(w));
        Mask m2 = r.getPunctaMask(w,false);
        m.add(m2,-1);
        Vector<Point> localMaxima = new Vector<Point>();
        Vector<Point> upperLeft = new Vector<Point>();
        Vector<Point> lowerRight = new Vector<Point>();
	Vector<Integer> intensities = new Vector<Integer>();
	int nPuncta = 0;
	int minSignal = (int)(5.0 / Math.pow(resolutionXY,2));
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                if(m.getValue(i,j) < 1){
                    continue;
                }
                int x1 = Math.max(i-punctaDetectionWindow,0);
                int x2 = Math.min(ndim.getWidth(),i+punctaDetectionWindow);
                int y1 = Math.max(j-punctaDetectionWindow,0);
                int y2 = Math.min(ndim.getHeight(),j+punctaDetectionWindow);
                if(ndim.getPixel(w,z,t,i,j,p) < ndim.max(w,z,t,p,x1,x2,y1,y2,m)) continue;
                localMaxima.add(new Point(i,j));
		intensities.add(ndim.getPixel(w,z,t,i,j,p));
                int nSignal = m.sum(x1,x2,y1,y2);
                while(nSignal < minSignal){
                    x1 = Math.max(x1-punctaDetectionWindow,0);
                    x2 = Math.min(ndim.getWidth(),x2+punctaDetectionWindow);
                    y1 = Math.max(y1-punctaDetectionWindow,0);
                    y2 = Math.min(ndim.getHeight(),y2+punctaDetectionWindow);
                    nSignal = m.sum(x1,x2,y1,y2);
		    if(x2 - x1 > 2000){
			System.out.println("Stuck finding local signal for maximum at ("+localMaxima.elementAt(localMaxima.size()-1).x+","+localMaxima.elementAt(localMaxima.size()-1).y+")");
			break;
		    }
                }
                upperLeft.add(new Point(x1,y1));
                lowerRight.add(new Point(x2,y2));
                //double signalFraction = ((double)m.sum(x1,x2,y1,y2)) / (punctaDetectionWindow*punctaDetectionWindow);
                //if(signalFraction < 0.05){
                //    zscores[i][j] = 0;
                //    continue;
                //}
                //double localMean = ndim.median(w,z,t,p,x1,x2,y1,y2,m);
                //double localStd = ndim.std(w,z,t,p,x1,x2,y1,y2,m);
                //zscores[i][j] = (ndim.getPixel(w,z,t,i,j,p) - localMean) / localStd;
                //double localMean = ndim.mean(w,z,t,p,x1,x2,y1,y2,m);
            }
        }
	for(int i = 0; i < intensities.size(); i++){
	    int max = intensities.elementAt(i);
	    int imax = i;
	    for(int j = i+1; j < intensities.size(); j++){
		if(intensities.elementAt(j) > max){
		    max = intensities.elementAt(j);
		    imax = j;
		}
	    }
	    intensities.set(imax,intensities.elementAt(i));
	    intensities.set(i,max);
	    Point temp = localMaxima.elementAt(i);
	    localMaxima.set(i,localMaxima.elementAt(imax));
	    localMaxima.set(imax,temp);
	    temp = upperLeft.elementAt(i);
	    upperLeft.set(i,upperLeft.elementAt(imax));
	    upperLeft.set(imax,temp);
	    temp = lowerRight.elementAt(i);
	    lowerRight.set(i,lowerRight.elementAt(imax));
	    lowerRight.set(imax,temp);
	}
        //int[] maxXY = argmax(zscores);
        int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
        Mask used = new Mask(ndim.getWidth(),ndim.getHeight(),false);
        used.multiply(m);
        Mask cMask = new Mask(ndim.getWidth(),ndim.getHeight());
        //while(zscores[maxXY[0]][maxXY[1]] > 3){
	//System.out.println("Found local maxima: "+localMaxima.size());
        for(int s = 0; s < localMaxima.size(); s++){
            //int Imax = ndim.getPixel(w,z,t,maxXY[0],maxXY[1],p);
            Point lm = localMaxima.elementAt(s);
	    //if(used.getValue(lm.x,lm.y) == 0) continue;
            Point ul = upperLeft.elementAt(s);
            Point lr = lowerRight.elementAt(s);
            int Imax = ndim.getPixel(w,z,t,lm.x,lm.y,p);
            //double localMean = ndim.mean(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double localMedian = ndim.median(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double localStd = ndim.std(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double minThreshold = Math.min(localMedian + (Imax - localMedian)/2, localMedian + 2.0*localStd);
	    double localThreshold = localMedian + 3*localStd;
	    if(Imax < localThreshold) continue;
	    localThreshold = Imax;
            Cluster c = new Cluster();
            Vector<Integer> borderX = new Vector<Integer>();
            Vector<Integer> borderY = new Vector<Integer>();
	    Vector<Integer> borderVal = new Vector<Integer>();
            //borderX.addElement(maxXY[0]);
            //borderY.addElement(maxXY[1]);
            //zscores[maxXY[0]][maxXY[1]] = 0;
            //used.setValue(maxXY[0],maxXY[1],1);
            borderX.addElement(lm.x);
            borderY.addElement(lm.y);
	    borderVal.addElement(Imax);
            used.setValue(lm.x,lm.y,0);
            cMask.setValue(lm.x,lm.y,1);
	    //System.out.println("Imax: "+Imax+", Threshold: "+localThreshold);
	    /*
	    double sumI = Imax;
	    int npix = 1;
	    int count = 0;
	    Vector<Double> removalQueueI = new Vector<Double>();
	    Vector<Integer> removalQueueN = new Vector<Integer>();
	    for(int k = 0; k < 5; k++){
		removalQueueI.addElement(0.0);
		removalQueueN.addElement(0);
	    }
	    double minVal = 1.0;
	    */
            while(borderX.size() > 0){
		//sumI -= removalQueueI.elementAt(0);
		//npix -= removalQueueN.elementAt(0);
		//removalQueueI.remove(0);
		//removalQueueN.remove(0);
		//double upperLimit = sumI / npix;
		//if(minVal > 0.1) upperLimit = Imax;
		int nborder = borderX.size();
		//double sumNew = 0.0;
		//int numNew = 0;
		for(int b = 0; b < nborder; b++){
		    int bi = borderX.elementAt(0);
		    int bj = borderY.elementAt(0);
		    c.addPixel(bi,bj);
		    double upperLimit = Imax;
		    if((borderVal.elementAt(0) - localThreshold)/(Imax - localThreshold) < 0.3) upperLimit = borderVal.elementAt(0);
		    for(int k = 0; k < 8; k++){
			int i = bi+diArr[k];
			int j = bj+djArr[k];
			localThreshold = minThreshold;// + Math.pow(0.95,(int)(Math.sqrt(i*i + j*j)))*(localThreshold - minThreshold);
			try{
			    //double val = (1-used.getValue(i,j))*((double)ndim.getPixel(w,z,t,i,j,p)) / Imax;
			    //double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localMean) / (Imax - localMean);
			    double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localThreshold) / (upperLimit - localThreshold);
			    if(val < 0.001 || val > 1.0) continue;
			    //if(val < minVal) minVal = val;
			    used.setValue(i,j,0);
			    borderX.addElement(i);
			    borderY.addElement(j);
			    borderVal.addElement(ndim.getPixel(w,z,t,i,j,p));
			    //sumNew += ndim.getPixel(w,z,t,i,j,p);
			    //numNew++;
			    //zscores[i][j] = 0;
			    cMask.setValue(i,j,1);
			}
			catch(ArrayIndexOutOfBoundsException e){ continue; }
		    }
		    //sumI -= ndim.getPixel(w,z,t,bi,bj,p);
		    //npix--;
		    borderX.remove(0);
		    borderY.remove(0);
		    borderVal.remove(0);
		}
		//sumI += sumNew;
		//npix += numNew;
		//removalQueueI.addElement(sumNew);
		//removalQueueN.addElement(numNew);
	    }
	    if(localMaxima.size() == 1161) System.out.println("Filling in blanks");
	    boolean unfilled = true;
	    while(unfilled){
		unfilled = false;
		for(int i = ul.x+1; i < lr.x-1; i++){
		    for(int j = ul.y+1; j < lr.y-1; j++){
			if(cMask.getValue(i,j) > 0 || m.getValue(i,j) < 1) continue;
			int sum = cMask.getValue(i-1,j-1) + cMask.getValue(i,j-1) + cMask.getValue(i+1,j-1) + cMask.getValue(i-1,j) + cMask.getValue(i+1,j) + cMask.getValue(i-1,j+1) + cMask.getValue(i,j+1) + cMask.getValue(i+1,j+1);
			if(sum > 4){
			    cMask.setValue(i,j,1);
			    c.addPixel(i,j);
			    unfilled = true;
			}
		    }
		}
	    }
	    for(int i = ul.x+1; i < lr.x-1; i++){
		for(int j = ul.y+1; j < lr.y-1; j++){
		    int sum = cMask.getValue(i-1,j-1) + cMask.getValue(i,j-1) + cMask.getValue(i+1,j-1) + cMask.getValue(i-1,j) + cMask.getValue(i+1,j) + cMask.getValue(i-1,j+1) + cMask.getValue(i,j+1) + cMask.getValue(i+1,j+1);
		    if(sum < 3){
			cMask.setValue(i,j,0);
			c.removePixel(new Point(i,j));
		    }
		}
	    }
	    /*
            double limit = 2*ndim.median(w,z,t,p,ul.x,lr.x,ul.y,lr.y,cMask) - Imax;
            for(int i = c.size()-1; i >=0; i--){
                Point pt = c.getPixel(i);
                if(ndim.getPixel(w,z,t,pt.x,pt.y,p) < limit) c.removePixel(i);
                cMask.setValue(pt.x,pt.y,0);
                used.setValue(pt.x,pt.y,1);
            }
            limit = ndim.mean(w,z,t,p,ul.x,lr.x,ul.y,lr.y,used) + 3*ndim.std(w,z,t,p,ul.x,lr.x,ul.y,lr.y,used);
            if(ndim.mean(w,z,t,p,ul.x,lr.x,ul.y,lr.y,cMask) < limit){
                used.add(cMask,ul.x,lr.x,ul.y,lr.y);
                cMask.clear(ul.x,lr.x,ul.y,lr.y);
                continue;
            }
	    */
	    if(c.size() < 2){
		for(int i = c.size()-1; i >=0; i--){
		    Point pt = c.getPixel(i);
		    used.setValue(pt.x,pt.y,1);
		    cMask.setValue(pt.x,pt.y,0);
		}
		continue;
	    }
	    Point center = c.getCentroid();
	    if(c.contains(center)){
		/*
		for(int i = c.size()-1; i >=0; i--){
		    Point pt = c.getPixel(i);
		    Point mirror = new Point(2*center.x - pt.x,2*center.y - pt.y);
		    if(!(c.contains(mirror))){
			c.removePixel(i);
			used.setValue(pt.x,pt.y,1);
		    }
		}
		*/
		if(c.size() > 10.6/Math.pow(resolutionXY,2)){
		    Point seed = c.getPixel(0);
		    System.out.println("Found ridiculously large punctum of size " + c.size() + " seeded at (" + seed.x + "," + seed.y + ")");
		    //maxXY = argmax(zscores);
		    cMask.clear(ul.x,lr.x,ul.y,lr.y);
		    continue;
		}
		r.addPunctum(w,c);
		nPuncta++;
	    }
	    else{
		for(int i = c.size()-1; i >=0; i--){
		    Point pt = c.getPixel(i);
		    used.setValue(pt.x,pt.y,1);
		}
	    }
            cMask.clear(ul.x,lr.x,ul.y,lr.y);
            //maxXY = argmax(zscores);
        }
	return nPuncta;
    }

    public void findSaturatedPuncta(int w, int z, int t, int p)
    {
	//int saturationThreshold = 4000;
	ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
	Mask m = r.getSignalMask(w);
	Mask used = new Mask(m);
	Mask cMask = new Mask(ndim.getWidth(),ndim.getHeight());
	Vector<Integer> borderSatX = new Vector<Integer>();
	Vector<Integer> borderSatY = new Vector<Integer>();
	Vector<Integer> borderUnsatX = new Vector<Integer>();
	Vector<Integer> borderUnsatY = new Vector<Integer>();
	Vector<Integer> borderVal = new Vector<Integer>();
	int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
	int minSignal = (int)Math.min(5.0 / Math.pow(resolutionXY,2),m.sum());
	for(int i = 0; i < ndim.getWidth(); i++){
	    for(int j = 0; j < ndim.getHeight(); j++){
		if(used.getValue(i,j)*ndim.getPixel(w,z,t,i,j,p) < saturationThreshold) continue;
		Cluster c = new Cluster();
		c.addPixel(i,j);
		used.setValue(i,j,0);
		cMask.setValue(i,j,1);
		borderSatX.addElement(i);
		borderSatY.addElement(j);
		while(borderSatX.size() > 0){
		    for(int k = 0; k < 8; k++){
			try{
			    int x = borderSatX.elementAt(0) + diArr[k];
			    int y = borderSatY.elementAt(0) + djArr[k];
			    int value = ndim.getPixel(w,z,t,x,y,p);
			    if(used.getValue(x,y) > 0){
				if(value > saturationThreshold){
				    borderSatX.addElement(x);
				    borderSatY.addElement(y);
				}
				else{
				    borderUnsatX.addElement(x);
				    borderUnsatY.addElement(y);
				    borderVal.addElement(value);
				}
				c.addPixel(x,y);
				used.setValue(x,y,0);
				cMask.setValue(x,y,1);
			    }
			}
			catch(ArrayIndexOutOfBoundsException e){ continue; }
		    }
		    borderSatX.remove(0);
		    borderSatY.remove(0);
		}
		//System.out.println("Done saturated cluster.");
		Point center = c.getCentroid();
		int x1 = Math.max(center.x-punctaDetectionWindow,0);
                int x2 = Math.min(ndim.getWidth(),center.x+punctaDetectionWindow);
                int y1 = Math.max(center.y-punctaDetectionWindow,0);
                int y2 = Math.min(ndim.getHeight(),center.y+punctaDetectionWindow);
                int nSignal = m.sum(x1,x2,y1,y2);
                while(nSignal < minSignal){
                    x1 = Math.max(x1-punctaDetectionWindow,0);
                    x2 = Math.min(ndim.getWidth(),x2+punctaDetectionWindow);
                    y1 = Math.max(y1-punctaDetectionWindow,0);
                    y2 = Math.min(ndim.getHeight(),y2+punctaDetectionWindow);
                    nSignal = m.sum(x1,x2,y1,y2);
		    //if(x2 - x1 == ndim.getWidth()) break;
                }
		int Imax = saturationThreshold;
		double localMedian = ndim.median(w,z,t,p,x1,x2,y1,y2,m);
		double localStd = ndim.std(w,z,t,p,x1,x2,y1,y2,m);
		double localThreshold = Math.min(localMedian + (Imax - localMedian)/2, localMedian + 2.0*localStd);
		while(borderUnsatX.size() > 0){
		    int upperLimit = Imax;
		    if((borderVal.elementAt(0) - localThreshold)/(Imax - localThreshold) < 0.3) upperLimit = borderVal.elementAt(0);
		    for(int k = 0; k < 8; k++){
			try{
			    int x = borderUnsatX.elementAt(0) + diArr[k];
			    int y = borderUnsatY.elementAt(0) + djArr[k];
			    int pixelValue = ndim.getPixel(w,z,t,x,y,p);
			    double val = used.getValue(x,y)*(pixelValue - localThreshold) / (upperLimit - localThreshold);
			    if(val < 0.001 || val > 1.0) continue;
			    borderUnsatX.addElement(x);
			    borderUnsatY.addElement(y);
			    borderVal.addElement(pixelValue);
			    c.addPixel(x,y);
			    used.setValue(x,y,0);
			    cMask.setValue(x,y,1);
			}
			catch(ArrayIndexOutOfBoundsException e){ continue; }
		    }
		    borderUnsatX.remove(0);
		    borderUnsatY.remove(0);
		    borderVal.remove(0);
		}
		boolean unfilled = true;
		while(unfilled){
		    unfilled = false;
		    for(int x = x1+1; x < x2-1; x++){
			for(int y = y1+1; y < y2-1; y++){
			    if(cMask.getValue(i,j) > 0 || m.getValue(i,j) < 1) continue;
			    int sum = cMask.getValue(x-1,y-1) + cMask.getValue(x,y-1) + cMask.getValue(x+1,y-1) + cMask.getValue(x-1,y) + cMask.getValue(x+1,y) + cMask.getValue(x-1,y+1) + cMask.getValue(x,y+1) + cMask.getValue(x+1,y+1);
			    if(sum > 4){
				cMask.setValue(x,y,1);
				c.addPixel(x,y);
				unfilled = true;
			    }
			}
		    }
		}
		for(int x = x1+1; x < x2-1; x++){
		    for(int y = y1+1; y < y2-1; y++){
			int sum = cMask.getValue(x-1,y-1) + cMask.getValue(x,y-1) + cMask.getValue(x+1,y-1) + cMask.getValue(x-1,y) + cMask.getValue(x+1,y) + cMask.getValue(x-1,y+1) + cMask.getValue(x,y+1) + cMask.getValue(x+1,y+1);
			if(sum < 3){
			    cMask.setValue(x,y,0);
			    c.removePixel(new Point(x,y));
			}
		    }
		}
		if(c.size() < 2){
		    for(int x = c.size()-1; x >=0; x--){
			Point pt = c.getPixel(x);
			used.setValue(pt.x,pt.y,1);
		    }
		    cMask.clear(x1,x2,y1,y2);
		    continue;
		}
		center = c.getCentroid();
		int index = c.indexOf(center);
		if(!(index < 0)){
		    if(c.size() > 10.6/Math.pow(resolutionXY,2)){
			Point seed = c.getPixel(0);
			System.out.println("Found ridiculously large punctum of size " + c.size() + " seeded at (" + seed.x + "," + seed.y + ")");
			cMask.clear(x1,x2,y1,y2);
			continue;
		    }
		    Point tmp = c.getPixel(0);
		    c.setPixel(0,c.getPixel(index));
		    c.setPixel(index,tmp);
		    r.addPunctum(w,c);
		}
		else{
		    for(int x = c.size()-1; x >=0; x--){
			Point pt = c.getPixel(x);
			used.setValue(pt.x,pt.y,1);
		    }
		}
		cMask.clear(x1,x2,y1,y2);
		borderSatX.clear();
		borderSatY.clear();
		borderUnsatX.clear();
		borderUnsatY.clear();
		borderVal.clear();
	    }
	}
    }

    public void resolveOverlaps(int w, int z, int t, int p)
    {
	ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
	//int saturationThreshold = 4000;
	int np = r.getNPuncta(w);
	Point[] peaks = new Point[np];
	Point[] troughs = new Point[np];
	Function2D[] fs = new Function2D[np];
	Cluster[] clusters = new Cluster[np];
	Vector<Cluster> satClusters = new Vector<Cluster>();
	for(int i = 0; i < np; i++){
	    Cluster c = r.getPunctum(w,i);
	    int imax = 0;
	    int imin = 65536;
	    int nSat = 0;
	    for(int j = 0; j < c.size(); j++){
		Point pt = c.getPixel(j);
		int value = ndim.getPixel(w,z,t,pt.x,pt.y,p);
		if(value > imax){
		    imax = value;
		    peaks[i] = pt;
		}
		if(value < imin){
		    imin = value;
		    troughs[i] = pt;
		}
		if(value > saturationThreshold){
		    nSat++;
		    clusters[i] = null;
		    satClusters.addElement(c);
		    break;
		}
	    }
	    if(nSat > 0) continue;
	    clusters[i] = new Cluster();
	    clusters[i].addPixel(peaks[i]);
	    fs[i] = Functions.gaussian2D();
	    double[] param = {imax,peaks[i].x,peaks[i].y,-0.5*(c.size()/Math.PI)/Math.log(((double)imin)/imax)};
	    /*
	    if(nSat > 0){
		peaks[i] = c.getCentroid();
		param[0] = param[0] / Math.exp(-(nSat/Math.PI)/(2*param[3]));
	    }
	    */
	    fs[i].setParameters(param);
	}
	Mask m = r.getPunctaMask(w,false);
	for(int i = 0; i < satClusters.size(); i++){
	    Cluster c = satClusters.elementAt(i);
	    for(int j = 0; j < c.size(); j++){
		Point pt = c.getPixel(j);
		m.setValue(pt.x,pt.y,0);
	    }
	}
	for(int i = 0; i < ndim.getWidth(); i++){
	    //int x1 = Math.max(0,i-resolutionWindow);
	    //int x2 = Math.min(ndim.getWidth(),i+resolutionWindow);
	    for(int j = 0; j < ndim.getHeight(); j++){
		if(m.getValue(i,j) < 1) continue;
		//int y1 = Math.max(0,j-resolutionWindow);
		//int y2 = Math.min(ndim.getHeight(),j+resolutionWindow);
		/*
		int value = ndim.getPixel(w,z,t,i,j,p);
		for(int k = 0; k < np; k++){
		    double kDiff = Math.abs(fs[k].calculate((double)i,(double)j)/value - 1.0);
		    if(kDiff < minDiff){
			minDiff = kDiff;
			minK = k;
		    }
		}
		*/
		int value = ndim.getPixel(w,z,t,i,j,p);
		double maxDiff = -10.0;
		int maxK = -1;
		double minDiff = 999999.0;
		int minK = -1;
		double minDist = 9999999.0;
		int distK = -1;
		boolean isPeak = false;
		for(int k = 0; k < np; k++){
		    if(clusters[k] == null) continue;
		    if(i == peaks[k].x && j == peaks[k].y){
			isPeak = true;
			break;
		    }
		    double kdist = Math.sqrt(Math.pow(i-peaks[k].x,2) + Math.pow(j-peaks[k].y,2));
		    if(kdist < minDist){
			minDist = kdist;
			distK = k;
		    }
		    double kDiff = fs[k].calculate((double)i,(double)j)/value - 1.0;
		    //double abs = Math.abs(kDiff);
		    if(kDiff > maxDiff){
			maxDiff = kDiff;
			maxK = k;
		    }
		    if(Math.abs(kDiff) < Math.abs(minDiff)){
			minDiff = kDiff;
			minK = k;
		    }
		}
		if(isPeak) continue;
		if(minDiff > -0.1 && minK >= 0) clusters[minK].addPixel(new Point(i,j));
		//else if(maxDiff > -0.5 || value > saturationThreshold) clusters[maxK].addPixel(new Point(i,j));
		else if(maxDiff > -1.0) clusters[maxK].addPixel(new Point(i,j));
		else clusters[distK].addPixel(new Point(i,j));
	    }
	}
	m.clear(0,ndim.getWidth(),0,ndim.getHeight());
	int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
	Vector<Integer> borderX = new Vector<Integer>();
	Vector<Integer> borderY = new Vector<Integer>();
	for(int l = 0; l < np; l++){
	    if(clusters[l] == null) continue;
	    Cluster rc = new Cluster();
	    for(int n = 0; n < clusters[l].size(); n++){
		Point pt = clusters[l].getPixel(n);
		m.setValue(pt.x,pt.y,1);
	    }
	    borderX.addElement(peaks[l].x);
	    borderY.addElement(peaks[l].y);
	    while(borderX.size() > 0){
		int nborder = borderX.size();
		for(int b = 0; b < nborder; b++){
		    int bi = borderX.elementAt(0);
		    int bj = borderY.elementAt(0);
		    rc.addPixel(bi,bj);
		    for(int k = 0; k < 8; k++){
			int i = bi+diArr[k];
			int j = bj+djArr[k];
			try{
			    if(m.getValue(i,j) > 0){
				m.setValue(i,j,0);
				borderX.addElement(i);
				borderY.addElement(j);
			    }
			}
			catch(ArrayIndexOutOfBoundsException e){ continue; }
		    }
		    borderX.remove(0);
		    borderY.remove(0);
		}
	    }
	    for(int n = 0; n < clusters[l].size(); n++){
		Point pt = clusters[l].getPixel(n);
		m.setValue(pt.x,pt.y,0);
	    }
	    clusters[l] = rc;
	}
	r.clearPuncta(w);
	for(int i = 0; i < satClusters.size(); i++) r.addPunctum(w,satClusters.elementAt(i));
	/*
	for(int i = 0; i < np; i++){
	    if(clusters[i] == null) continue;
	    if(clusters[i].size() > 0.07/Math.pow(resolutionXY,2)) r.addPunctum(w,clusters[i]);
	}
	*/
	//double areaThreshold = 0.07/Math.pow(resolutionXY,2);
	double borderThreshold = Math.sqrt(punctaAreaThreshold);
	boolean doneMerging = false;
	while(!doneMerging){
	    doneMerging = true;
	    for(int i = 0; i < np; i++){
		if(clusters[i] == null) continue;
		if(clusters[i].size() < 4){//punctaAreaThreshold){
		    int maxBorder = 0;
		    int maxJ = -1;
		    for(int j = 0; j < i; j++){
			if(clusters[j] == null) continue;
			if(clusters[i].distanceTo(clusters[j]) > clusters[i].size()+clusters[j].size()) continue;
			int jBorder = clusters[j].getBorderLength(clusters[i]);
			if(jBorder > maxBorder){
			    maxBorder = jBorder;
			    maxJ = j;
			}
		    }
		    for(int j = i+1; j < np; j++){
			if(clusters[j] == null) continue;
			int jBorder = clusters[j].getBorderLength(clusters[i]);
			if(jBorder > maxBorder){
			    maxBorder = jBorder;
			    maxJ = j;
			}
		    }
		    if(maxBorder > Math.min(borderThreshold,Math.sqrt(clusters[i].size()))){
			clusters[maxJ].add(clusters[i]);
			clusters[i] = null;
			doneMerging = false;
		    }
		}
	    }
	}
	for(int i = 0; i < np; i++){
	    if(clusters[i] == null) continue;
	    if(clusters[i].size() > 3 /*punctaAreaThreshold*/) r.addPunctum(w,clusters[i]);
	}
    }
    
    private int[] argmax(double[][] arr)
    {
        double max = -999999.9;
        int[] maxPos = {-1,-1};
        for(int i = 0; i < arr.length; i++){
            for(int j = 0; j < arr[0].length; j++){
                if(arr[i][j] > max){
                    max = arr[i][j];
                    maxPos[0] = i;
                    maxPos[1] = j;
                }
            }
        }
        return maxPos;
    }
    
    public int findSynapses(int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
	int nSynapses = 0;
	for(int icol = 0; icol < r.getNSynapseCollections(); icol++){
	    //System.out.println("Starting collection "+icol);
	    SynapseCollection sc = r.getSynapseCollection(icol);
	    int[] chan = sc.getChannels();
	    int[] np = new int[chan.length];
	    int[] pi = new int[chan.length];
	    int maxIndex = 1;
	    for(int i = 0; i < chan.length; i++){
		np[i] = r.getNPuncta(chan[i]);
		pi[i] = 0;
	    }
	    for(int i = 1; i < chan.length; i++) maxIndex *= np[i];
	    for(int j = 0; j < np[0]; j++){
		//System.out.println("Testing puncta "+j);
		Synapse best = null;
		double bestScore = -999;
		Cluster c = r.getPunctum(chan[0],j);
		int sumpi = 1;
		while(sumpi > 0){
		    Synapse s = new Synapse();
		    s.addPunctum(r.getPunctum(chan[0],j),j);
		    boolean potentialSynapse = true;
		    for(int i = 1; i < np.length; i++){
			Cluster c2 = r.getPunctum(chan[i],pi[i]);
			if(c.peakToPeakDistance2(c2) > 2*(c.size()+c2.size())){
			    potentialSynapse = false;
			    for(int k = i; k > 0; k--){
				pi[k]++;
				if(pi[k] < np[k]) break;
				pi[k] = 0;
			    }
			    for(int k = i+1; k < np.length; k++) pi[k] = 0;
			    break;
			}
			s.addPunctum(c2,pi[i]);
		    }
		    if(potentialSynapse){
			if(sc.computeColocalization(s)){
			    if(s.getColocalizationScore() > bestScore){
				bestScore = s.getColocalizationScore();
				best = s;
			    }
			}
			for(int i = np.length-1; i > 0; i--){
			    pi[i]++;
			    if(pi[i] < np[i]) break;
			    pi[i] = 0;
			}
		    }
		    sumpi = 0;
		    for(int i = 1; i < np.length; i++) sumpi += pi[i];
		}
		if(best != null){
		    sc.addSynapse(best);
		    nSynapses++;
		}
	    }
	    //System.out.println("Found " + nSynapses + " synapses.");
	}
	return nSynapses;
    }
}
