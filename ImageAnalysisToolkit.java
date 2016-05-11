import java.awt.AWTException;
import java.awt.MouseInfo;
import java.awt.Point;
import java.awt.Robot;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Vector;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public abstract class ImageAnalysisToolkit
{

    public static final Lock threadPoolLock = new ReentrantLock();
    public static final Lock threadCounterLock = new ReentrantLock();
    public static final Condition ready = threadPoolLock.newCondition();
    public static ExecutorService executor;
    protected static int activeThreads;

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
            analyzer.threadFinished();
	    threadPoolLock.lock();
	    try{ ready.signal(); }
	    finally{ threadPoolLock.unlock(); }
        }
    }
    
    protected NDImage ndim;
    protected ImageReport[] reports;
    protected String[] channelNames;
    protected boolean[] isPost;
    protected int[] prePost;
    protected double resolutionXY;
    protected int saturationThreshold;

    protected void init(NDImage im, ImageReport[] r)
    {
	ndim = im;
        reports = r;
	channelNames = new String[ndim.getNWavelengths()];
	isPost = new boolean[channelNames.length];
	prePost = new int[channelNames.length];
	resolutionXY = 0.046;
	saturationThreshold = 65536;
    }
    
    public void setImage(NDImage im){ ndim = im; }
    
    public void setImageReports(ImageReport [] reps){ reports = reps; }

    public void setChannelName(int w, String name){ channelNames[w] = name; }

    public String[] getChannelNames(){ return channelNames; }

    public void setPost(int w, boolean tf){ isPost[w] = tf; }

    public void setPrePost(int w, int pp){ prePost[w] = pp; }

    public void setResolutionXY(double res)
    {
	resolutionXY = res;
	//for(int i = 0; i < reports.length; i++) reports[i].setResolutionXY(res);
    }

    public double getResolutionXY(){ return resolutionXY; }

    public void setSaturationThreshold(int st){ saturationThreshold = st; }

    public static void setExecutor(ExecutorService ex){ executor = ex; }
    
    public void batchProcess(int maxThreads) throws InterruptedException
    {
	Robot bot = null;
	try{ bot = new Robot(); }
	catch(AWTException e){ e.printStackTrace(); }
        for(int i = 0; i < ndim.getNZ(); i++){
            for(int j = 0; j < ndim.getNT(); j++){
                for(int k = 0; k < ndim.getNPos(); k++){
		    threadPoolLock.lock();
		    try{
			while(activeThreads == maxThreads){
			    ready.await(8,TimeUnit.MINUTES);
			    if(bot != null){
				Point mouseLocation = MouseInfo.getPointerInfo().getLocation();
				bot.mouseMove(mouseLocation.x + 1, mouseLocation.y);
				bot.mouseMove(mouseLocation.x - 1, mouseLocation.y);
				bot.mouseMove(mouseLocation.x, mouseLocation.y);
			    }
			}
			BatchThread bt = new BatchThread(this,i,j,k);
			addThread();
			executor.execute(bt);
			//System.out.println("Thread launched " + activeThreads);
		    }
		    finally{ threadPoolLock.unlock(); }
                }
            }
        }
    }

    public static void finishBatchJobs() throws InterruptedException
    {
	Robot bot = null;
	try{ bot = new Robot(); }
	catch(AWTException e){ e.printStackTrace(); }
	executor.shutdown();
        while(!executor.awaitTermination(8,TimeUnit.MINUTES)){
	    if(bot != null){
		Point mouseLocation = MouseInfo.getPointerInfo().getLocation();
		bot.mouseMove(mouseLocation.x + 1, mouseLocation.y);
		bot.mouseMove(mouseLocation.x - 1, mouseLocation.y);
		bot.mouseMove(mouseLocation.x, mouseLocation.y);
	    }
        }
    }
    
    public void threadFinished()
    {
	threadCounterLock.lock();
	try{ activeThreads--; }
	finally{ threadCounterLock.unlock(); }
    }
    
    public void addThread()
    {
	threadCounterLock.lock();
        try{ activeThreads++; }
	finally{ threadCounterLock.unlock(); }
    }

    public static void resetActiveThreads()
    {
	threadCounterLock.lock();
        try{ activeThreads = 0; }
	finally{ threadCounterLock.unlock(); }
    }
    
    public abstract void standardAnalysis(int z, int t, int p);
    
    public abstract Mask findOutlierMask(int w, int z, int t, int p);

    public abstract Mask findBackgroundMask(int w, int z, int t, int p);
    
    public abstract Mask findSignalMask(int w, int z, int t, int p, Mask outMask);

    public abstract int findPuncta(int w, int z, int t, int p);
    
    public abstract void findSynapses(int z, int t, int p);
    
    public abstract void findSynapses(int z, int t, int p, int[] chan);
    
    public void saveImageReports(String phil)
    {
        try{
            RandomAccessFile raf = new RandomAccessFile(phil,"rw");
            byte[] buf = new byte[100];
	    int nchan = ndim.getNWavelengths();
	    buf[0] = (byte)nchan;
	    for(int i = 0; i < nchan; i++){
		//if(isPost[i]) buf[i+1] = 1;
		//else buf[i+1] = 0;
		buf[i+1] = (byte)prePost[i];
	    }
	    raf.write(buf,0,1+nchan);
	    for(int i = 0; i < nchan; i++){
		byte[] nameBytes = channelNames[i].getBytes();
		ImageReport.writeIntToBuffer(buf,0,nameBytes.length);
		raf.write(buf,0,4);
		raf.write(nameBytes);
	    }
	    ImageReport.writeIntToBuffer(buf,0,reports.length);
	    raf.write(buf,0,4);
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
	    byte[] buf = new byte[100];
	    raf.read(buf,0,1);
	    int nchan = (int)buf[0];
	    raf.read(buf,0,nchan);
	    for(int i = 0; i < nchan; i++){
		//if(buf[i] > 0) isPost[i] = true;
		//else isPost[i] = false;
		prePost[i] = (int)buf[i];
	    }
	    for(int i = 0; i < nchan; i++){
		raf.read(buf,0,4);
		int length = ImageReport.readIntFromBuffer(buf,0);
		byte[] nameBytes = new byte[length];
		raf.read(nameBytes,0,length);
		channelNames[i] = new String(nameBytes);
	    }
	    raf.read(buf,0,4);
	    int nReports = ImageReport.readIntFromBuffer(buf,0);
	    if(nReports != reports.length){
		System.out.println("WARNING: Number of image analysis records does not match number of images loaded. Proceeding anyway...");
		reports = new ImageReport[nReports];
	    }
            for(int i = 0; i < reports.length; i++){
                reports[i] = ImageReport.read(raf);
            }
            raf.close();
        }
        catch(IOException e){ e.printStackTrace(); }
    }

    public static ImageReport[] readImageReports(String phil)
    {
	ImageReport[] r;
        try{
            RandomAccessFile raf = new RandomAccessFile(phil,"r");
	    byte[] buf = new byte[100];
	    raf.read(buf,0,1);
	    int nchan = (int)buf[0];
	    raf.read(buf,0,nchan);
	    for(int i = 0; i < nchan; i++){
		raf.read(buf,0,4);
		int length = ImageReport.readIntFromBuffer(buf,0);
		byte[] nameBytes = new byte[length];
		raf.read(nameBytes,0,length);
		System.out.println("Channel " + i + ": " + new String(nameBytes));
	    }
	    raf.read(buf,0,4);
	    int nReports = ImageReport.readIntFromBuffer(buf,0);
	    r = new ImageReport[nReports];
            for(int i = 0; i < r.length; i++){
                r[i] = ImageReport.read(raf);
            }
            raf.close();
        }
        catch(IOException e){
	    e.printStackTrace();
	    r = null;
	}
	return r;
    }
}
