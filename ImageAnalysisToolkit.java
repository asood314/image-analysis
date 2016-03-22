import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Vector;

public class ImageAnalysisToolkit
{
    public class BatchThread extends Thread
    {
        private ImageAnalysisToolkit analyzer;
        private int zslice,timepoint,position;
        
        public BatchThread(ImageAnalysisToolkit iat, int z, int t, int p)
        {
            analyzer = iat;
            zslice = z;
            timepoint = t;
            position = p;
        }
        
        public void run()
        {
            analyzer.standardAnalysis(zslice,timepoint,position);
            analyzer.threadFinished();
        }
    }
    
    private NDImage ndim;
    private ImageReport[] reports;
    private int activeThreads;
    
    public ImageAnalysisToolkit(NDImage im, ImageReport[] r)
    {
        ndim = im;
        reports = r;
        activeThreads = 0;
    }
    
    public void setImage(NDImage im){ ndim = im; }
    
    public void setImageReports(ImageReport [] reps){ reports = reps; }
    
    public void batchProcess(String outname)
    {
        for(int i = 0; i < ndim.getNZ(); i++){
            for(int j = 0; j < ndim.getNT(); j++){
                for(int k = 0; k < ndim.getNPos(); k++){
                    BatchThread bt = new BatchThread(this,i,j,k);
                    activeThreads++;
                    bt.run();
                }
            }
        }
        while(activeThreads > 0){}
        saveImageReports(outname);
    }
    
    public void threadFinished(){ activeThreads--; }
    
    public void standardAnalysis(int z, int t, int p)
    {
        int index = p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z;
        reports[index] = new ImageReport(ndim.getNWavelengths());
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
        double mean = ndim.mean(w,z,t,p);
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
    
    public Mask findSignalMask(int w, int z, int t, int p, Mask outMask)
    {
        Mask m = new Mask(ndim.getWidth(),ndim.getHeight());
        double std = ndim.std(w,z,t,p,outMask);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                int x1 = Math.max(i-40,0);
                int x2 = Math.min(ndim.getWidth(),i+40);
                int y1 = Math.max(j-40,0);
                int y2 = Math.min(ndim.getHeight(),j+40);
                double zscore = (ndim.getPixel(w,z,t,i,j,p) - ndim.mean(w,z,t,p,x1,x2,y1,y2,outMask)) / std;
                if(zscore > 1.0) m.setValue(i,j,1);
            }
        }
        Vector<Integer> borderX = new Vector<Integer>(10);
        Vector<Integer> borderY = new Vector<Integer>(10);
        //Mask master = new Mask(ndim.getWidth(),ndim.getHeight());
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
    
    public void findPuncta(int w, int z, int t, int p)
    {
        ImageReport r = reports[p*ndim.getNT()*ndim.getNZ() + t*ndim.getNZ() + z];
        double[][] zscores = new double[ndim.getWidth()][ndim.getHeight()];
        Mask m = r.getSignalMask(w);
        for(int i = 0; i < ndim.getWidth(); i++){
            for(int j = 0; j < ndim.getHeight(); j++){
                int x1 = Math.max(i-20,0);
                int x2 = Math.min(ndim.getWidth(),i+20);
                int y1 = Math.max(j-20,0);
                int y2 = Math.min(ndim.getHeight(),j+20);
                double signalFraction = ((double)r.getSignalMask(w).sum(x1,x2,y1,y2)) / ((x2-x1)*(y2-y1));
                if(signalFraction < 0.05){
                    zscores[i][j] = 0;
                    continue;
                }
                double localMean = ndim.mean(w,z,t,p,x1,x2,y1,y2,m);
                double localStd = ndim.std(w,z,t,p,x1,x2,y1,y2,m);
                zscores[i][j] = (ndim.getPixel(w,z,t,i,j,p) - localMean) / localStd;
            }
        }
        int[] maxXY = argmax(zscores);
        while(zscores[maxXY[0]][maxXY[1]] > 3){
            Cluster c = new Cluster();
            Vector<Integer> borderX = new Vector<Integer>();
            Vector<Integer> borderY = new Vector<Integer>();
            borderX.addElement(maxXY[0]);
            borderY.addElement(maxXY[1]);
            zscores[maxXY[0]][maxXY[1]] = 0;
            //Mask subMask = new Mask(ndim.getWidth(),ndim.getHeight());
            //subMask.setValue(i,j,1);
            while(borderX.size() > 0){
                int bi = borderX.elementAt(0);
                int bj = borderY.elementAt(0);
                c.addPixel(bi,bj);
                for(int di = -1; di < 2; di++){
                    for(int dj = -1; dj < 2; dj++){
                        try{
                            double val = zscores[bi+di][bj+dj];
                            if(val > 1 /*&& subMask.getValue(bi+di,bj+dj) == 0*/){
                                //subMask.setValue(bi+di,bj+dj,1);
                                borderX.addElement(bi+di);
                                borderY.addElement(bj+dj);
                                zscores[bi+di][bj+dj] = 0;
                            }
                        }
                        catch(ArrayIndexOutOfBoundsException e){ continue; }
                    }
                }
                borderX.remove(0);
                borderY.remove(0);
            }
            r.addPunctum(w,c);
            maxXY = argmax(zscores);
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
    
    public void saveImageReports(String phil)
    {
        try{
            RandomAccessFile raf = new RandomAccessFile(phil,"rw");
            byte[] buf = new byte[10];
            for(int i = 0; i < reports.length; i++){
                if(reports[i] == null){
                    buf[0] = (byte)(0x000000ff);
                    raf.write(buf,0,1);
                    continue;
                }
                reports[i].write(raf);
            }
            raf.close();
        }
        catch(IOException e){ e.printStackTrace(); }
    }
    
    public void loadImageReports(String phil)
    {
        try{
            RandomAccessFile raf = new RandomAccessFile(phil,"r");
            for(int i = 0; i < reports.length; i++){
                reports[i] = ImageReport.read(raf);
            }
            raf.close();
        }
        catch(IOException e){ e.printStackTrace(); }
    }
}
