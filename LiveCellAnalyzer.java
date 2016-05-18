import java.util.Vector;

public class LiveCellAnalyzer extends ImageAnalysisToolkit
{
    private int signalDetectionWindow;
    private int punctaDetectionWindow;
    
    public LiveCellAnalyzer(NDImage im, ImageReport[] r)
    {
        init(im,r);
        activeThreads = 0;
        signalDetectionWindow = 40*(ndim.getWidth()/1000);
        punctaDetectionWindow = 20*(ndim.getWidth()/1000);
    }
    
    public void setSignalDetectionWindow(int sdt){ signalDetectionWindow = sdt; }
    
    public void setPunctaDetectionWindow(int pdt){ punctaDetectionWindow = pdt; }
    
    public void standardAnalysis(int z, int t, int p)
    {
        int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
        reports[index] = new ImageReport(ndim.getNWavelengths(),ndim.getWidth(),ndim.getHeight());
        for(int w = 0; w < ndim.getNWavelengths(); w++){
            Mask m = findOutlierMask(w,z,t,p);
            reports[index].setOutlierMask(w,m);
            reports[index].setSignalMask(w,findSignalMask(w,z,t,p,m));
        }
        for(int w = 0; w < ndim.getNWavelengths(); w++) findPuncta(w,z,t,p);
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

    public Mask findBackgroundMask(int w, int z, int t, int p){ return (Mask)null; }
    
    public Mask findSignalMask(int w, int z, int t, int p, Mask outMask)
    {
        Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
        double std = ndim.std(w,z,t,p,outMask);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                int x1 = Math.max(i-signalDetectionWindow,0);
                int x2 = Math.min(ndim.getWidth(),i+signalDetectionWindow);
                int y1 = Math.max(j-signalDetectionWindow,0);
                int y2 = Math.min(ndim.getHeight(),j+signalDetectionWindow);
                double zscore = (ndim.getPixel(w,z,t,i,j,p) - ndim.median(w,z,t,p,x1,x2,y1,y2,outMask)) / std;
                if(zscore > 1.0) m.setValue(i,j,1);
            }
        }
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
        m.threshold(10);
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
        double[][] zscores = new double[ndim.getWidth()][ndim.getHeight()];
        Mask m = new Mask(r.getSignalMask(w));
	Mask m2 = r.getPunctaMask(w,false);
	m.add(m2,-1);
	int nPuncta = 0;
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                int x1 = Math.max(i-punctaDetectionWindow,0);
                int x2 = Math.min(ndim.getWidth(),i+punctaDetectionWindow);
                int y1 = Math.max(j-punctaDetectionWindow,0);
                int y2 = Math.min(ndim.getHeight(),j+punctaDetectionWindow);
                double signalFraction = ((double)r.getSignalMask(w).sum(x1,x2,y1,y2)) / ((x2-x1)*(y2-y1));
                if(signalFraction < 0.05){
                    zscores[i][j] = 0;
                    continue;
                }
                double localMean = ndim.median(w,z,t,p,x1,x2,y1,y2,m);
                double localStd = ndim.std(w,z,t,p,x1,x2,y1,y2,m);
                zscores[i][j] = (ndim.getPixel(w,z,t,i,j,p) - localMean) / localStd;
            }
        }
        int[] maxXY = argmax(zscores);
	int[] diArr = {-1,0,1,-1,1,-1,0,1};
	int[] djArr = {-1,-1,-1,0,0,1,1,1};
	Mask used = new Mask(ndim.getWidth(),ndim.getHeight());
        while(zscores[maxXY[0]][maxXY[1]] > 3){
	    int Imax = ndim.getPixel(w,z,t,maxXY[0],maxXY[1],p);
            Cluster c = new Cluster();
            Vector<Integer> borderX = new Vector<Integer>();
            Vector<Integer> borderY = new Vector<Integer>();
            borderX.addElement(maxXY[0]);
            borderY.addElement(maxXY[1]);
            zscores[maxXY[0]][maxXY[1]] = 0;
            used.setValue(maxXY[0],maxXY[1],1);
            while(borderX.size() > 0){
                int bi = borderX.elementAt(0);
                int bj = borderY.elementAt(0);
                c.addPixel(bi,bj);
                for(int k = 0; k < 8; k++){
                    int i = bi+diArr[k];
		    int j = bj+djArr[k];
		    try{
			double val = (1-used.getValue(i,j))*zscores[i][j];
			if(val > 1 /*&& subMask.getValue(bi+di,bj+dj) == 0*/){
			    used.setValue(i,j,1);
			    borderX.addElement(i);
			    borderY.addElement(j);
			    zscores[i][j] = 0;
			}
		    }
		    catch(ArrayIndexOutOfBoundsException e){ continue; }
                }
                borderX.remove(0);
                borderY.remove(0);
            }
            if(c.contains(c.getCentroid())){
		r.addPunctum(w,c);
		nPuncta++;
	    }
            maxXY = argmax(zscores);
        }
	return nPuncta;
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
	    SynapseCollection sc = r.getSynapseCollection(icol);
	    int[] chan = sc.getChannels();
	    int[] np = new int[chan.length];
	    int maxIndex = 1;
	    for(int i = 0; i < chan.length; i++) np[i] = r.getNPuncta(chan[i]);
	    for(int i = 1; i < chan.length; i++) maxIndex *= np[i];
	    for(int j = 0; j < np[0]; j++){
		Synapse best = null;
		double bestScore = -999;
		for(int l = 0; l < maxIndex; l++){
		    Synapse s = new Synapse();
		    s.addPunctum(r.getPunctum(chan[0],j),j);
		    for(int i = 1; i < np.length; i++){
			int index = l;
			for(int k = 1; k < i; k++) index = index / np[k];
			index = index % np[i];
			s.addPunctum(r.getPunctum(chan[i],index),index);
		    }
		    if(!sc.computeColocalization(s)) continue;
		    if(s.getColocalizationScore() > bestScore){
			bestScore = s.getColocalizationScore();
			best = s;
		    }
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
