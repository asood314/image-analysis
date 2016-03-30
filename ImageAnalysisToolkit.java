import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Vector;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;

public abstract class ImageAnalysisToolkit
{
    public class BatchThread implements Runnable
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
            //analyzer.threadFinished();
        }
    }
    
    protected NDImage ndim;
    protected ImageReport[] reports;
    protected int activeThreads;
    
    public void setImage(NDImage im){ ndim = im; }
    
    public void setImageReports(ImageReport [] reps){ reports = reps; }
    
    public void batchProcess(ExecutorService executor, String outname) throws InterruptedException
    {
	//ExecutorService executor = Executors.newFixedThreadPool(ndim.getNZ()*ndim.getNT()*ndim.getNPos());
        for(int i = 0; i < ndim.getNZ(); i++){
            for(int j = 0; j < ndim.getNT(); j++){
                for(int k = 0; k < ndim.getNPos(); k++){
                    BatchThread bt = new BatchThread(this,i,j,k);
                    //addThread();
		    //bt.run();
		    executor.execute(bt);
                    System.out.println("Thread launched");
                }
            }
        }
	executor.shutdown();
        while(!executor.awaitTermination(10,TimeUnit.MINUTES)){
	    /*
            try{
                Thread.currentThread().join(600000);
                System.out.println(""+activeThreads+" threads currently active");
            }
            catch(InterruptedException e){ e.printStackTrace(); }
	    */
        }
        saveImageReports(outname);
    }
    
    public synchronized void threadFinished(){ activeThreads--; }
    
    public synchronized void addThread()
    {
        activeThreads++;
    }
    
    public abstract void standardAnalysis(int z, int t, int p);
    
    public abstract Mask findOutlierMask(int w, int z, int t, int p);
    
    public abstract Mask findSignalMask(int w, int z, int t, int p, Mask outMask);

    public abstract void findPuncta(int w, int z, int t, int p);
    
    public abstract void findSynapses(int z, int t, int p);
    
    public abstract void findSynapses(int z, int t, int p, int[] chan);
    
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
