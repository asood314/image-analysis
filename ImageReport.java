import java.awt.Point;
import java.awt.Polygon;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;
import java.util.Vector;

public class ImageReport
{
    private Mask[] outlierMasks;
    private Mask[] signalMasks;
    private Mask[] utilityMasks;
    private Vector<Mask> regionsOfInterest;
    private Vector<Vector<Cluster>> puncta;
    private Vector<Synapse> synapses;
    private int nChannels, imWidth, imHeight;
    private double resolutionXY;
    
    public ImageReport(int nchan, int w, int h)
    {
        nChannels = nchan;
	imWidth = w;
	imHeight = h;
        outlierMasks = new Mask[nChannels];
        signalMasks = new Mask[nChannels];
	utilityMasks = new Mask[nChannels];
	regionsOfInterest = new Vector<Mask>();
        puncta = new Vector<Vector<Cluster>>();
        for(int i = 0; i < nchan; i++) puncta.add(new Vector<Cluster>());
        synapses = new Vector<Synapse>();
	resolutionXY = 0.046;
    }

    public void setResolutionXY(double res){ resolutionXY = res; }

    public double getResolutionXY(){ return resolutionXY; }
    
    public int getNChannels(){ return nChannels; }
    
    public void setOutlierMask(int chan, Mask m){ outlierMasks[chan] = m; }
    
    public void setSignalMask(int chan, Mask m){ signalMasks[chan] = m; }
    
    public void setUtilityMask(int chan, Mask m){ utilityMasks[chan] = m; }
    
    public Mask getOutlierMask(int chan){ return outlierMasks[chan]; }
    
    public Mask getSignalMask(int chan){ return signalMasks[chan]; }

    public Mask[] getSignalMasks(){ return signalMasks; }

    public Mask getUtilityMask(int chan){ return utilityMasks[chan]; }

    public void addROI(Mask m){ regionsOfInterest.addElement(m); }

    public Mask getROI(int index){ return regionsOfInterest.elementAt(index); }

    public int getNROI(){ return regionsOfInterest.size(); }
    
    public void addPunctum(int chan, Cluster c){ puncta.elementAt(chan).add(c); }
    
    public void removePunctum(int chan, int index){ puncta.elementAt(chan).remove(index); }

    public void clearPuncta(int chan){ puncta.elementAt(chan).clear(); }
    
    public void addSynapse(Synapse s){ synapses.add(s); }
    
    public void removeSynapse(int index){ synapses.remove(index); }
    
    public Cluster getPunctum(int chan, int index){ return puncta.elementAt(chan).elementAt(index); }
    
    public int getNPuncta(int chan){ return puncta.elementAt(chan).size(); }
    
    public Synapse getSynapse(int index){ return synapses.elementAt(index); }
    
    public int getNSynapses(){ return synapses.size(); }
    
    public Cluster selectPunctum(Point p){
        double minDist = 999999;
        Cluster closest = null;
        for(int i = 0; i < nChannels; i++){
            Vector<Cluster> channel = puncta.elementAt(i);
            for(int j = 0; j < channel.size(); j++){
                double dist = channel.elementAt(j).distanceTo(p);
                if(dist < minDist){
                    minDist = dist;
                    closest = channel.elementAt(j);
                }
            }
        }
        return closest;
    }

    public Cluster selectPunctum(Point p, int chan){
        double minDist = 999999;
        Cluster closest = null;
	Vector<Cluster> channel = puncta.elementAt(chan);
	for(int j = 0; j < channel.size(); j++){
	    double dist = channel.elementAt(j).distanceTo(p);
	    if(dist < minDist){
		minDist = dist;
		closest = channel.elementAt(j);
	    }
	}
        return closest;
    }
    
    public Synapse selectSynapse(Point p){
        double minDist = 999999;
        Synapse closest = null;
        for(int j = 0; j < synapses.size(); j++){
            double dist = synapses.elementAt(j).distanceTo(p);
            if(dist < minDist){
                minDist = dist;
                closest = synapses.elementAt(j);
            }
        }
        return closest;
    }
    
    public Mask getPunctaMask(int chan){
        Mask m = new Mask(signalMasks[chan].getWidth(),signalMasks[chan].getHeight());
        Vector<Cluster> pList = puncta.elementAt(chan);
        for(int i = 0; i < pList.size(); i++){
            Cluster c = pList.elementAt(i);
            for(int j = 0; j < c.size(); j++){
                Point x = c.getPixel(j);
                m.setValue((int)x.getX(),(int)x.getY(),1);
            }
        }
        return m;
    }
    
    public Mask getSynapseMask(int chan){
        Mask m = new Mask(signalMasks[chan].getWidth(),signalMasks[chan].getHeight());
        Vector<Cluster> pList = puncta.elementAt(chan);
        for(int i = 0; i < synapses.size(); i++){
            Synapse s = synapses.elementAt(i);
            for(int j = 0; j < s.getNPuncta(); j++){
                Cluster c = s.getPunctum(j);
                for(int k = 0; k < c.size(); k++){
                    Point x = c.getPixel(k);
                    m.setValue((int)x.getX(),(int)x.getY(),1);
                }
            }
        }
        return m;
    }

    public Mask getSkeleton(int chan)
    {
	int window = (int)(2.0/resolutionXY);
	Mask oldMask = new Mask(signalMasks[chan]);
	Mask newMask = new Mask(imWidth,imHeight);
	int count = 0;
	boolean changed = true;
	while(changed && count < 10){
	    changed = false;
	    int nSignal = 0;
	    int centerX = 0;
	    int centerY = 0;
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(oldMask.getValue(i,j) == 0) continue;
		    int sum = oldMask.getValue(i-1,j-1)+oldMask.getValue(i-1,j)+oldMask.getValue(i-1,j+1)+oldMask.getValue(i,j-1)+oldMask.getValue(i,j+1)+oldMask.getValue(i+1,j-1)+oldMask.getValue(i+1,j)+oldMask.getValue(i+1,j+1);
		    //int sumDi = oldMask.getValue(i+1,j-1)+oldMask.getValue(i+1,j)+oldMask.getValue(i+1,j+1) - (oldMask.getValue(i-1,j-1)+oldMask.getValue(i-1,j)+oldMask.getValue(i-1,j+1));
		    //int sumDj = oldMask.getValue(i-1,j+1)+oldMask.getValue(i,j+1)+oldMask.getValue(i+1,j+1) - (oldMask.getValue(i-1,j-1)+oldMask.getValue(i,j-1)+oldMask.getValue(i+1,j-1));
		    //if(sum < 3 && sumDi*sumDj == 0) continue;
		    newMask.setValue(i,j,sum);
		    changed = true;
		}
	    }
	    oldMask.copy(newMask);
	    count++;
	}
	for(int i = 1; i < imWidth-1; i++){
	    for(int j = 1; j < imHeight-1; j++){
		if(newMask.getValue(i,j) > 256000) newMask.setValue(i,j,1);
		else newMask.setValue(i,j,0);
	    }
	}
	return newMask;
    }

    public String getSynapseDensity(int postChan, String[] chanNames)
    {
	String msg = "";
	int[] di = {-1,0,1,-1,1,-1,0,1};
	int[] dj = {-1,-1,-1,0,0,1,1,1};
	for(int r = 0; r < regionsOfInterest.size(); r++){
	    Mask roi = regionsOfInterest.elementAt(r);
	    double area = 0;
	    double perimeter = 0;
	    msg += "Region "+r+":\n---------------------\n";
	    for(int i = 1; i < roi.getWidth()-1; i++){
		for(int j = 1; j < roi.getHeight()-1; j++){
		    if(roi.getValue(i,j) * signalMasks[postChan].getValue(i,j) > 0){
			area += 1.0;
			int sum = 0;
			for(int k = 0; k < di.length; k++) sum += signalMasks[postChan].getValue(i+di[k],j+dj[k]);
			if(sum < 8) perimeter += 1.0;
		    }
		}
	    }
	    area *= resolutionXY*resolutionXY;
	    perimeter *= resolutionXY;
	    double lengthEstimate = perimeter / 2.0;
	    int nSynapses = 0;
	    int sumOverlap = 0;
	    int sumSize = 0;
	    for(int i = 0; i < synapses.size(); i++){
		Synapse s = synapses.elementAt(i);
		Point p = s.getCenter();
		if(roi.getValue(p.x,p.y) > 0){
		    nSynapses++;
		    sumSize += s.size();
		    sumOverlap += s.getOverlap();
		}
	    }
	    double densityA = nSynapses/area;
	    double densityL = nSynapses/lengthEstimate;
	    double avgSize = ((double)sumSize)/nSynapses;
	    double avgOverlap = ((double)sumOverlap)/nSynapses;
	    msg += "Number of synapes: "+nSynapses+"\nAverage synapse size (pixels): "+avgSize+"\nAverage area of overlap (pixels): "+avgOverlap+"\nDendrite area (um^2): "+area+"\nDendrite perimeter (um): "+perimeter+"\nSynapse area density (um^-2): "+densityA+"\nSynapse length density (um^-1):"+densityL+"\n";
	    for(int chan = 0; chan < puncta.size(); chan++){
		Vector<Cluster> clusters = puncta.elementAt(chan);
		int nPuncta = 0;
		sumSize = 0;
		for(int i = 0; i < clusters.size(); i++){
		    Point p = clusters.elementAt(i).getCentroid();
		    if(roi.getValue(p.x,p.y) > 0){
			nPuncta++;
			sumSize += clusters.elementAt(i).size();
		    }
		}
		densityA = nPuncta/area;
		densityL = nPuncta/lengthEstimate;
		avgSize = ((double)sumSize)/nPuncta;
		msg += "Number of "+chanNames[chan]+" puncta: "+nPuncta+"\nArea density (um^-2): "+densityA+"\nLength density (um^-1): "+densityL+"\nAverage puncta size (pixels): "+avgSize+"\n";
	    }
	}
	return msg;
    }
    
    public void write(RandomAccessFile fout) throws IOException
    {
        byte[] buf = new byte[400000];
        int width = imWidth;
        int height = imHeight;
        buf[0] = (byte)(nChannels & 0x000000ff);
        writeIntToBuffer(buf,1,width);
        writeIntToBuffer(buf,5,height);
        fout.write(buf,0,9);
        for(int c = 0; c < nChannels; c++){
	    if(outlierMasks[c] != null){
		buf[0] = 1;
		fout.write(buf,0,1);
		for(int j = 0; j < height; j++){
		    for(int i = 0; i < width; i++){
			buf[i] = (byte)(outlierMasks[c].getValue(i,j) & 0x000000ff);
		    }
		    fout.write(buf,0,width);
		}
	    }
	    else{
		buf[0] = 0;
		fout.write(buf,0,1);
	    }
	    if(signalMasks[c] != null){
		buf[0] = 1;
		fout.write(buf,0,1);
		for(int j = 0; j < height; j++){
		    for(int i = 0; i < width; i++){
			buf[i] = (byte)(signalMasks[c].getValue(i,j) & 0x000000ff);
		    }
		    fout.write(buf,0,width);
		}
	    }
	    else{
		buf[0] = 0;
		fout.write(buf,0,1);
	    }
            writeShortToBuffer(buf,0,(short)getNPuncta(c));
            fout.write(buf,0,2);
            for(int i = 0; i < getNPuncta(c); i++){
                Cluster clust = getPunctum(c,i);
                writeShortToBuffer(buf,0,(short)clust.size());
                for(int j = 0; j < clust.size(); j++){
                    Point p = clust.getPixel(j);
                    writeShortToBuffer(buf,4*j+2,(short)p.getX());
                    writeShortToBuffer(buf,4*j+4,(short)p.getY());
                }
                fout.write(buf,0,4*clust.size() + 2);
            }
        }
        writeShortToBuffer(buf,0,(short)synapses.size());
        int offset = 2;
        for(int i = 0; i < synapses.size(); i++){
            Synapse s = synapses.elementAt(i);
            buf[offset] = (byte)(s.getNChannels() & 0x000000ff);
            offset++;
            for(int j = 0; j < s.getNChannels(); j++){
                buf[offset] = (byte)(s.getChannel(j) & 0x000000ff);
                writeShortToBuffer(buf,offset+1,(short)s.getPunctumIndex(j));
                offset += 3;
            }
        }
        fout.write(buf,0,offset);
	int nROI = regionsOfInterest.size();
	buf[0] = (byte)nROI;
	fout.write(buf,0,1);
	for(int j = 0; j < height; j++){
	    for(int i = 0; i < width; i++){
		for(int k = 0; k < nROI; k++){
		    buf[k*width+i] = (byte)(regionsOfInterest.elementAt(k).getValue(i,j) & 0x000000ff);
		}
	    }
	    fout.write(buf,0,nROI*width);
	}
    }
    
    public static void writeIntToBuffer(byte[] buffer, int offset, int data)
    {
        buffer[offset] = (byte)(data & 0x000000ff);
        buffer[offset+1] = (byte)((data & 0x0000ff00) >> 8);
        buffer[offset+2] = (byte)((data & 0x00ff0000) >> 16);
        buffer[offset+3] = (byte)((data & 0xff000000) >> 24);
    }
    
    public static void writeShortToBuffer(byte[] buffer, int offset, short data)
    {
        buffer[offset] = (byte)(data & 0x00ff);
        buffer[offset+1] = (byte)((data & 0xff00) >> 8);
    }
    
    public static ImageReport read(RandomAccessFile fin) throws IOException
    {
        byte[] buf = new byte[40000];
        fin.read(buf,0,9);
        int nc = (int)buf[0];
        if((nc & 0x000000ff) == 0x000000ff) return null;
        int width = readIntFromBuffer(buf,1);
        int height = readIntFromBuffer(buf,5);
	ImageReport r = new ImageReport(nc,width,height);
        for(int c = 0; c < nc; c++){
	    fin.read(buf,0,1);
	    if(buf[0] > 0){
		Mask om = new Mask(width,height);
		for(int j = 0; j < height; j++){
		    fin.read(buf,0,width);
		    for(int i = 0; i < width; i++){
			om.setValue(i,j,(int)buf[i]);
		    }
		}
		r.setOutlierMask(c,om);
	    }
	    fin.read(buf,0,1);
	    if(buf[0] > 0){
		Mask sm = new Mask(width,height);
		for(int j = 0; j < height; j++){
		    fin.read(buf,0,width);
		    for(int i = 0; i < width; i++){
			sm.setValue(i,j,(int)buf[i]);
		    }
		}
		r.setSignalMask(c,sm);
	    }
            fin.read(buf,0,2);
            int np = readShortFromBuffer(buf,0);
            for(int i = 0; i < np; i++){
                fin.read(buf,0,2);
                int cs = readShortFromBuffer(buf,0);
                fin.read(buf,0,4*cs);
                Cluster clust = new Cluster();
                for(int j = 0; j < cs; j++){
                    clust.addPixel(readShortFromBuffer(buf,4*j),readShortFromBuffer(buf,4*j+2));
                }
                r.addPunctum(c,clust);
            }
        }
        fin.read(buf,0,2);
        int ns = readShortFromBuffer(buf,0);
        for(int i = 0; i < ns; i++){
            fin.read(buf,0,1);
            int nchan = (int)buf[0];
            int [] chans = new int[nchan];
            int [] ids = new int[nchan];
            for(int j = 0; j < nchan; j++){
                fin.read(buf,0,3);
                chans[j] = (int)buf[0];
                ids[j] = readShortFromBuffer(buf,1);
            }
            Synapse s = new Synapse(chans);
            for(int j = 0; j < nchan; j++) s.addPunctum(r.getPunctum(chans[j],ids[j]),ids[j]);
            r.addSynapse(s);
        }
	fin.read(buf,0,1);
	Mask[] rois = new Mask[buf[0]];
	for(int k = 0; k < rois.length; k++) rois[k] = new Mask(width,height);
	for(int j = 0; j < height; j++){
	    fin.read(buf,0,rois.length*width);
	    for(int i = 0; i < width; i++){
		for(int k = 0; k < rois.length; k++){
		    rois[k].setValue(i,j,(int)buf[k*width+i]);
		}
	    }
	}
	for(int k = 0; k < rois.length; k++) r.addROI(rois[k]);
        return r;
    }
    
    public static int readIntFromBuffer(byte[] buffer, int offset)
    {
        return ((buffer[offset+3] & 0xff) << 24) | ((buffer[offset+2] & 0xff) << 16) | ((buffer[offset+1] & 0xff) << 8) | (buffer[offset] & 0xff);
    }
    
    public static int readShortFromBuffer(byte[] buffer, int offset)
    {
        return ((buffer[offset+1] & 0xff) << 8) | (buffer[offset] & 0xff);
    }

    public void loadMetaMorphRegions(String phil)
    {
	try{
	    BufferedReader fin = new BufferedReader(new FileReader(phil));
	    String line = fin.readLine();
	    while(line != null){
		StringTokenizer st = new StringTokenizer(line,",");
		for(int i = 0; i < 7; i++) line = st.nextToken();
		st = new StringTokenizer(line," ");
		if(Integer.parseInt(st.nextToken()) != 6){
		    System.out.println("Something's wrong.");
		    return;
		}
		int nv = Integer.parseInt(st.nextToken());
		int[] xpoints = new int[nv];
		int[] ypoints = new int[nv];
		for(int i = 0; i < nv; i++){
		    xpoints[i] = Integer.parseInt(st.nextToken());
		    ypoints[i] = Integer.parseInt(st.nextToken());
		}
		Polygon p = new Polygon(xpoints,ypoints,nv);
		Mask m = new Mask(imWidth,imHeight);
		for(int i = 0; i < imWidth; i++){
		    for(int j = 0; j < imHeight; j++){
			if(p.contains(i,j)) m.setValue(i,j,1);
		    }
		}
		addROI(m);
		line = fin.readLine();
	    }
	    fin.close();
	}
	catch(IOException e){ e.printStackTrace(); }
    }
}
