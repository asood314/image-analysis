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

    public Mask findSignalMask(int w, int z, int t, int p, Mask bkgdMask)
    {
	Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
	double lowerLimit = ndim.mode(w,z,t,p);
	double best = ndim.mean(w,z,t,p);
	double upperLimit = 2*best;
	double step = (upperLimit - lowerLimit) / 10;
	int nsteps = 10;
	if(step < 1.0){
	    step = 1.0;
	    nsteps = (int)(upperLimit - lowerLimit);
	}
	int fom = 0;
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
		int maxSize = 0;
		int avgSigSize = 0;
		int nSigClusters = 0;
		Vector<Integer> borderX = new Vector<Integer>(10);
		Vector<Integer> borderY = new Vector<Integer>(10);
		Mask subMask = new Mask(ndim.getWidth(),ndim.getHeight());
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			if(m.getValue(i,j) != 1) continue;
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
					if(val > 0 && subMask.getValue(bi+di,bj+dj) == 0){
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
			if(size > maxSize) maxSize = size;
			avgSigSize += size;
			nSigClusters++;
		    }
		}
		int avgSize = 0;
		int nBkgClusters = 0;
		for(int i = 0; i < ndim.getWidth(); i++){
		    for(int j = 0; j < ndim.getHeight(); j++){
			if(m.getValue(i,j) != 0) continue;
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
		int ifom = avgSigSize*avgSize;
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
	System.out.println("Best threshold: "+best);
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
	//Vector<Integer> clusterX = new Vector<Integer>(10);
	//Vector<Integer> clusterY = new Vector<Integer>(10);
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
				    //m.setValue(bi+di,bj+dj,-1);
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
		    //clusterX.addElement(borderX.elementAt(0));
		    //clusterY.addElement(borderY.elementAt(0));
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
		//clusterX.clear();
		//clusterY.clear();
		//System.out.println("Added cluster: " + i + "," + j+", " + size);
            }
        }
	//System.out.println("Thresholding");
        //m.threshold((int)(0.25/Math.pow(resolutionXY,2)));
	System.out.println("Filling in blanks");
        for(int i = 1; i < ndim.getWidth()-1; i++){
            for(int j = 1; j < ndim.getHeight()-1; j++){
		if(m.getValue(i,j) > 0) continue;
                int sum = m.getValue(i-1,j-1) + m.getValue(i,j-1) + m.getValue(i+1,j-1) + m.getValue(i-1,j) + m.getValue(i+1,j) + m.getValue(i-1,j+1) + m.getValue(i,j+1) + m.getValue(i+1,j+1);
                if(sum > 4) m.setValue(i,j,1);
            }
        }
	return m;
    }

    public int findPuncta(int w, int z, int t, int p)
    {
	//findSaturatedPuncta(w,z,t,p);
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
	double minThreshold = Math.min(localMedian + (Imax - localMedian)/2, localMedian + 2.0*localStd);
	double localThreshold = localMedian + 3*localStd;
	System.out.println("Imax: "+Imax+" Median: "+localMedian+" Threshold: "+localThreshold);
	if(Imax < localThreshold){
	    System.out.println("Not bright enough");
	    return 0;
	}
	localThreshold = Imax;
	Cluster c = new Cluster();
	Mask cMask = new Mask(ndim.getWidth(),ndim.getHeight());
	Vector<Integer> borderX = new Vector<Integer>();
	Vector<Integer> borderY = new Vector<Integer>();
	Vector<Integer> borderVal = new Vector<Integer>();
	borderX.addElement(lm.x);
	borderY.addElement(lm.y);
	borderVal.addElement(Imax);
	used.setValue(lm.x,lm.y,0);
	cMask.setValue(lm.x,lm.y,1);
	while(borderX.size() > 0){
	    int nborder = borderX.size();
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
			double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localThreshold) / (upperLimit - localThreshold);
			if(val < 0.001 || val > 1.0) continue;
			used.setValue(i,j,0);
			borderX.addElement(i);
			borderY.addElement(j);
			borderVal.addElement(ndim.getPixel(w,z,t,i,j,p));
			cMask.setValue(i,j,1);
		    }
		    catch(ArrayIndexOutOfBoundsException e){ continue; }
		}
		borderX.remove(0);
		borderY.remove(0);
		borderVal.remove(0);
	    }
	}
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

    public void findSaturatedPuncta(int w, int z, int t, int p)
    {
	System.out.println("Start.");
	int saturationThreshold = 4000;
	int punctaDetectionWindow = 10;
	ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
	r.clearPuncta(w);
	Mask m = r.getSignalMask(w);
	Mask used = new Mask(m);//ndim.getWidth(),ndim.getHeight(),false);
	Mask cMask = new Mask(ndim.getWidth(),ndim.getHeight());
	Vector<Integer> borderSatX = new Vector<Integer>();
	Vector<Integer> borderSatY = new Vector<Integer>();
	Vector<Integer> borderUnsatX = new Vector<Integer>();
	Vector<Integer> borderUnsatY = new Vector<Integer>();
	Vector<Integer> borderVal = new Vector<Integer>();
	int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
	int minSignal = (int)Math.min(20.0 / Math.pow(resolutionXY,2),m.sum());
	for(int i = 0; i < ndim.getHeight(); i++){
	    for(int j = 0; j < ndim.getWidth(); j++){
		if(used.getValue(i,j)*ndim.getPixel(w,z,t,i,j,p) < saturationThreshold) continue;
		System.out.println("Starting new cluster");
		Cluster c = new Cluster();
		c.addPixel(i,j);
		used.setValue(i,j,0);
		cMask.setValue(i,j,1);
		borderSatX.addElement(i);
		borderSatY.addElement(j);
		int nSat = 1;
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
				    nSat++;
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
		System.out.println("Done saturated cluster: "+nSat);
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
		System.out.println("Got local window");
		int Imax = saturationThreshold;
		double localMedian = ndim.median(w,z,t,p,x1,x2,y1,y2,m);
		double localStd = ndim.std(w,z,t,p,x1,x2,y1,y2,m);
		double localThreshold = Math.min(localMedian + (Imax - localMedian)/2, localMedian + 2*localStd);
		System.out.println(""+localMedian+", "+localStd+", "+localThreshold);
		while(borderUnsatX.size() > 0){
		    int upperLimit = borderVal.elementAt(0);
		    for(int k = 0; k < 8; k++){
			try{
			    int x = borderUnsatX.elementAt(0) + diArr[k];
			    int y = borderUnsatY.elementAt(0) + djArr[k];
			    int pixelValue = ndim.getPixel(w,z,t,x,y,p);
			    double val = used.getValue(x,y)*(pixelValue - localThreshold) / (upperLimit - localThreshold);
			    System.out.println(""+upperLimit+", "+val);
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
		System.out.println("Done unsaturated cluster");
		for(int x = x1+1; x < x2-1; x++){
		    for(int y = y1+1; y < y2-1; y++){
			int sum = cMask.getValue(x-1,y-1) + cMask.getValue(x,y-1) + cMask.getValue(x+1,y-1) + cMask.getValue(x-1,y) + cMask.getValue(x+1,y) + cMask.getValue(x-1,y+1) + cMask.getValue(x,y+1) + cMask.getValue(x+1,y+1);
			if(sum > 4){
			    cMask.setValue(x,y,1);
			    c.addPixel(x,y);
			}
			else if(sum < 3){
			    cMask.setValue(x,y,0);
			    c.removePixel(new Point(x,y));
			}
		    }
		}
		if(c.size() < 2){
		    System.out.println("Too small");
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
		    System.out.println("Adding cluster");
		    Point tmp = c.getPixel(0);
		    c.setPixel(0,c.getPixel(index));
		    c.setPixel(index,tmp);
		    r.addPunctum(w,c);
		}
		else{
		    System.out.println("Weird shape");
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
	System.out.println("Done");
    }
    
    public int findSynapses(int z, int t, int p)
    {
	return 0;
    }
    
}
