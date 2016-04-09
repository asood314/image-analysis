import java.awt.Point;
import java.util.Vector;

public class FixedCellAnalyzer extends ImageAnalysisToolkit
{
    private int signalDetectionWindow;
    private int punctaDetectionWindow;
    private int punctaFindingIterations;
    
    public FixedCellAnalyzer(NDImage im, ImageReport[] r)
    {
        ndim = im;
        reports = r;
        activeThreads = 0;
        signalDetectionWindow = 50*(ndim.getWidth()/1000);
        punctaDetectionWindow = 10*(ndim.getWidth()/1000);
	punctaFindingIterations = 2;
    }
    
    public void setSignalDetectionWindow(int sdt){ signalDetectionWindow = sdt; }
    
    public void setPunctaDetectionWindow(int pdt){ punctaDetectionWindow = pdt; }

    public void standardAnalysis(int z, int t, int p)
    {
        int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
        reports[index] = new ImageReport(ndim.getNWavelengths());
        for(int w = 0; w < ndim.getNWavelengths(); w++){
            Mask m = findBackgroundMask(w,z,t,p);//findOutlierMask(w,z,t,p);
            reports[index].setOutlierMask(w,m.getInverse());
            reports[index].setSignalMask(w,findSignalMask(w,z,t,p,m));
        }
	System.out.println("Done signal finding.");
	for(int i = 0; i < punctaFindingIterations; i++){
	    for(int w = 0; w < ndim.getNWavelengths(); w++){
		findPuncta(w,z,t,p);
		System.out.println("Iteration " + i + " found " + reports[index].getNPuncta(w) + " puncta in channel " + w + ".");
	    }
	}
	//System.out.println("Done puncta finding.");
        //int[] sc = {0,1};
        findSynapses(z,t,p);
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
	    if(lmedian < 3*mode) m.setValue(i,19,1);
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
		if(lmedian < 3*mode) m.setValue(i,j,1);
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
	//Mask m2 = new Mask(ndim.getWidth(),ndim.getHeight());
	double stdBkg = ndim.std(w,z,t,p,bkgdMask);
	//double thresholdSig = ndim.max(w,z,t,p,bkgdMask);
	double globalThreshold = ndim.mean(w,z,t,p,bkgdMask) + 5*stdBkg;
	//int[] dist = new int[8192];
	//for(int i = 0; i < 8192; i++) dist[i] = 0;
	for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
		//if(bkgdMask.getValue(i,j) > 0) continue;
		int value = ndim.getPixel(w,z,t,i,j,p);
		if(value > globalThreshold){
		    m.setValue(i,j,1);
		    //m2.setValue(i,j,1);
		    //dist[value/8]++;
		}
	    }
	}
	/*
	int modeSig = 0;
	int modeHeight = 0;
	for(int i = 0; i < 8192; i++){
	    if(dist[i] > modeHeight){
		modeHeight = dist[i];
		modeSig = i;
	    }
	}
	int halfMode = modeSig;
	for(halfMode = modeSig; modeHeight/dist[halfMode] < 2; halfMode++);
	double stdSig = 8*(halfMode - modeSig);
	*/
	//System.out.println(globalThreshold);
	//System.out.println(stdBkg);
	/*
	double outlierThreshold = 8*globalThreshold;
	for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
		if(ndim.getPixel(w,z,t,i,j,p) < outlierThreshold) m2.setValue(i,j,m.getValue(i,j));
	    }
	}
	System.out.println(outlierThreshold);
	*/
	Vector<Integer> borderX = new Vector<Integer>(10);
        Vector<Integer> borderY = new Vector<Integer>(10);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                if(m.getValue(i,j) != 1) continue;
                borderX.addElement(i);
                borderY.addElement(j);
                Mask subMask = new Mask(ndim.getWidth(),ndim.getHeight());
                subMask.setValue(i,j,1);
                int size = 0;
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
                subMask.multiply(size);
                m.add(subMask);
            }
        }
        m.threshold(100);
        for(int i = 1; i < ndim.getWidth()-1; i++){
            for(int j = 1; j < ndim.getHeight()-1; j++){
                int sum = m.getValue(i-1,j-1) + m.getValue(i,j-1) + m.getValue(i+1,j-1) + m.getValue(i-1,j) + m.getValue(i+1,j) + m.getValue(i-1,j+1) + m.getValue(i,j+1) + m.getValue(i+1,j+1);
                if(sum > 4) m.setValue(i,j,1);
            }
        }
	return m;
    }
    
    public void findPuncta(int w, int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        double[][] zscores = new double[ndim.getWidth()][ndim.getHeight()];
        Mask m = new Mask(r.getSignalMask(w));
        Mask m2 = r.getPunctaMask(w);
        m.add(m2,-1);
        Vector<Point> localMaxima = new Vector<Point>();
        Vector<Point> upperLeft = new Vector<Point>();
        Vector<Point> lowerRight = new Vector<Point>();
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                if(m.getValue(i,j) < 1){
                    zscores[i][j] = 0;
                    continue;
                }
                int x1 = Math.max(i-punctaDetectionWindow,0);
                int x2 = Math.min(ndim.getWidth(),i+punctaDetectionWindow);
                int y1 = Math.max(j-punctaDetectionWindow,0);
                int y2 = Math.min(ndim.getHeight(),j+punctaDetectionWindow);
                if(ndim.getPixel(w,z,t,i,j,p) < ndim.max(w,z,t,p,x1,x2,y1,y2,m)) continue;
                localMaxima.add(new Point(i,j));
                int nSignal = m.sum(x1,x2,y1,y2);
                while(nSignal < 10000){
                    x1 = Math.max(x1-punctaDetectionWindow,0);
                    x2 = Math.min(ndim.getWidth(),x2+punctaDetectionWindow);
                    y1 = Math.max(y1-punctaDetectionWindow,0);
                    y2 = Math.min(ndim.getHeight(),y2+punctaDetectionWindow);
                    nSignal = m.sum(x1,x2,y1,y2);
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
        //int[] maxXY = argmax(zscores);
        int[] diArr = {-1,0,1,-1,1,-1,0,1};
        int[] djArr = {-1,-1,-1,0,0,1,1,1};
        Mask used = new Mask(ndim.getWidth(),ndim.getHeight(),false);
        used.multiply(m);
        //Mask cMask = new Mask(ndim.getWidth(),ndim.getHeight());
        //while(zscores[maxXY[0]][maxXY[1]] > 3){
	//System.out.println(localMaxima.size());
        for(int s = 0; s < localMaxima.size(); s++){
            //int Imax = ndim.getPixel(w,z,t,maxXY[0],maxXY[1],p);
            Point lm = localMaxima.elementAt(s);
            Point ul = upperLeft.elementAt(s);
            Point lr = lowerRight.elementAt(s);
            int Imax = ndim.getPixel(w,z,t,lm.x,lm.y,p);
            //double localMean = ndim.mean(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double localMedian = ndim.median(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double localStd = ndim.std(w,z,t,p,ul.x,lr.x,ul.y,lr.y,m);
	    double minThreshold = localMedian + 1.5*localStd;
	    double localThreshold = localMedian + 3*localStd;
	    if(Imax < localThreshold) continue;
	    localThreshold = Imax;
            Cluster c = new Cluster();
            Vector<Integer> borderX = new Vector<Integer>();
            Vector<Integer> borderY = new Vector<Integer>();
            //borderX.addElement(maxXY[0]);
            //borderY.addElement(maxXY[1]);
            //zscores[maxXY[0]][maxXY[1]] = 0;
            //used.setValue(maxXY[0],maxXY[1],1);
            borderX.addElement(lm.x);
            borderY.addElement(lm.y);
            used.setValue(lm.x,lm.y,0);
            //cMask.setValue(lm.x,lm.y,1);
	    //System.out.println("Imax: "+Imax+", Threshold: "+localThreshold);
            while(borderX.size() > 0){
                int bi = borderX.elementAt(0);
                int bj = borderY.elementAt(0);
                c.addPixel(bi,bj);
		localThreshold = minThreshold + 0.9*(localThreshold - minThreshold);
                for(int k = 0; k < 8; k++){
                    int i = bi+diArr[k];
                    int j = bj+djArr[k];
                    try{
                        //double val = (1-used.getValue(i,j))*((double)ndim.getPixel(w,z,t,i,j,p)) / Imax;
                        //double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localMean) / (Imax - localMean);
			double val = used.getValue(i,j)*((double)ndim.getPixel(w,z,t,i,j,p) - localThreshold) / (Imax - localThreshold);
                        if(val < 0.001 || val > 1.0) continue;
                        used.setValue(i,j,0);
                        borderX.addElement(i);
                        borderY.addElement(j);
                        //zscores[i][j] = 0;
                        //cMask.setValue(i,j,1);
                    }
                    catch(ArrayIndexOutOfBoundsException e){ continue; }
                }
                borderX.remove(0);
                borderY.remove(0);
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
	    if(c.size() < 10) continue;
            if(c.size() > 1000){
                Point seed = c.getPixel(0);
                System.out.println("Found ridiculously large punctum of size " + c.size() + " seeded at (" + seed.x + "," + seed.y + ")");
                //maxXY = argmax(zscores);
                //cMask.clear(ul.x,lr.x,ul.y,lr.y);
                continue;
            }
            if(c.contains(c.getCentroid())) r.addPunctum(w,c);
            //cMask.clear(ul.x,lr.x,ul.y,lr.y);
            //maxXY = argmax(zscores);
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
    
    public void findSynapses(int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        int[] np = new int[r.getNChannels()];
        int[] chan = new int[r.getNChannels()];
        int maxIndex = 1;
        for(int i = 0; i < r.getNChannels(); i++){
            np[i] = r.getNPuncta(i);
            chan[i] = i;
            //System.out.println(np[i]);
            maxIndex *= np[i];
        }
        for(int j = 0; j < maxIndex; j++){
            Synapse s = new Synapse(chan);
            //System.out.println("Potential syanpse " + j);
            for(int i = 0; i < np.length; i++){
                int index = j;
                for(int k = 0; k < i; k++) index = index / np[k];
                index = index % np[i];
                //System.out.println("Puncta " + i + " center: " + r.getPunctum(i,index).getCentroid().toString());
                s.addPunctum(r.getPunctum(i,index),index);
            }
            if(s.isColocalized()) r.addSynapse(s);
        }
    }
    
    public void findSynapses(int z, int t, int p, int[] chan)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        int[] np = new int[chan.length];
        int maxIndex = 1;
        for(int i = 0; i < chan.length; i++){
            np[i] = r.getNPuncta(chan[i]);
            maxIndex *= np[i];
        }
        for(int j = 0; j < maxIndex; j++){
            Synapse s = new Synapse(chan);
            for(int i = 0; i < np.length; i++){
                int index = j;
                for(int k = 0; k < i; k++) index = index / np[k];
                index = index % np[i];
                s.addPunctum(r.getPunctum(chan[i],index),index);
            }
            if(s.isColocalized()) r.addSynapse(s);
        }
    }    
}

	/*
	int Imax = (int)ndim.max(w,z,t,p);
	int[] avgDist = new int[Imax];
	Vector<Integer> maxima = new Vector<Integer>();
	for(int i = 0; i < Imax; i+=1000){
	    int max = 0;
	    int maxbin = i;
	    int lim = Math.min(Imax,i+1000);
	    for(int j = i; j < lim; j++){
		if(dist[j] > max){
		    max = dist[j];
		}
	    }
	    if(max < 10) continue;
	    int avgLength = Math.max(1,1500/max);
	    for(int j = i; j < lim; j++){
		int sum = 0;
		for(int k = j-avgLength+1; k < j+avgLength+1; k++) sum += dist[k];
		avgDist[j] = sum/avgLength;
	    }
	    max = 0;
	    for(int j = i; j < lim; j++){
		if(avgDist[j] > max){
		    max = avgDist[j];
		    maxbin = j;
		}
	    }
	    maxima.addElement(new Integer(maxbin));
	}
	for(int i = maxima.size()-1; i >= 0; i--){
	    int max = avgDist[maxima.elementAt(i)];
	    int low = Math.max(0,maxima.elementAt(i)-1500);
	    int high = Math.min(Imax,maxima.elementAt(i)+1500);
	    for(int j = low; j < high; j++){
		if(avgDist[j] > max){
		    maxima.remove(i);
		    break;
		}
	    }
	}
	for(int i = 0; i < maxima.size(); i++) System.out.println(maxima.elementAt(i));
	*/
