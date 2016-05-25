import java.awt.Point;
import java.awt.Polygon;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;
import java.util.Vector;

public class ImageReport
{
    private Mask[] outlierMasks;
    private Mask[] signalMasks;
    private Mask[] utilityMasks;
    private Vector<Integer> axisLengths;
    private Vector<Mask> regionsOfInterest;
    private Vector<Vector<Cluster>> puncta;
    private Vector<SynapseCollection> synapseCollections;
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
	axisLengths = new Vector<Integer>();
        puncta = new Vector<Vector<Cluster>>();
        for(int i = 0; i < nchan; i++) puncta.add(new Vector<Cluster>());
        synapseCollections = new Vector<SynapseCollection>();
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

    public void addAxis(int len){ axisLengths.addElement(len); }

    public int getAxisLength(int index){ return axisLengths.elementAt(index); }
    
    public void addPunctum(int chan, Cluster c){ puncta.elementAt(chan).add(c); }
    
    public void removePunctum(int chan, int index){ puncta.elementAt(chan).remove(index); }

    public void clearPuncta(int chan){ puncta.elementAt(chan).clear(); }
    
    public void addSynapseCollection(SynapseCollection sc){ synapseCollections.add(sc); }
    
    public void removeSynapseCollection(int index){ synapseCollections.remove(index); }
    
    public Cluster getPunctum(int chan, int index){ return puncta.elementAt(chan).elementAt(index); }
    
    public int getNPuncta(int chan){ return puncta.elementAt(chan).size(); }
    
    public SynapseCollection getSynapseCollection(int index){ return synapseCollections.elementAt(index); }
    
    public int getNSynapseCollections(){ return synapseCollections.size(); }
    
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
	for(int i = 0; i < synapseCollections.size(); i++){
	    SynapseCollection sc = synapseCollections.elementAt(i);
	    for(int j = 0; j < sc.getNSynapses(); j++){
		Synapse s = sc.getSynapse(j);
		double dist = s.distanceTo(p);
		if(dist < minDist){
		    minDist = dist;
		    closest = s;
		}
	    }
	}
        return closest;
    }

    public Synapse selectSynapseFromCollection(Point p, int i){
        double minDist = 999999;
        Synapse closest = null;
	SynapseCollection sc = synapseCollections.elementAt(i);
	for(int j = 0; j < sc.getNSynapses(); j++){
	    Synapse s = sc.getSynapse(j);
	    double dist = s.distanceTo(p);
	    if(dist < minDist){
		minDist = dist;
		closest = s;
	    }
	}
        return closest;
    }
    
    public Mask getPunctaMask(int chan, boolean outline){
        Mask m = new Mask(signalMasks[chan].getWidth(),signalMasks[chan].getHeight());
        Vector<Cluster> pList = puncta.elementAt(chan);
        for(int i = 0; i < pList.size(); i++){
            Cluster c = pList.elementAt(i);
            for(int j = 0; j < c.size(); j++){
                Point x = c.getPixel(j);
                m.setValue((int)x.getX(),(int)x.getY(),1);
            }
        }
	Mask m2 = m.getCopy();
	if(outline){
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(m.getValue(i,j) < 1) continue;
		    int sum = m.getValue(i-1,j-1)+m.getValue(i-1,j)+m.getValue(i-1,j+1)+m.getValue(i,j-1)+m.getValue(i,j+1)+m.getValue(i+1,j-1)+m.getValue(i+1,j)+m.getValue(i+1,j+1);
		    if(sum > 7) m2.setValue(i,j,0);
		}
	    }
	}
        return m2;
    }
    
    public Mask getSynapseMask(boolean outline){
        Mask m = new Mask(imWidth,imHeight);
	for(int icol = 0; icol < synapseCollections.size(); icol++){
	    SynapseCollection sc = synapseCollections.elementAt(icol);
	    for(int i = 0; i < sc.getNSynapses(); i++){
		Synapse s = sc.getSynapse(i);
		for(int j = 0; j < s.getNPuncta(); j++){
		    Cluster c = s.getPunctum(j);
		    for(int k = 0; k < c.size(); k++){
			Point pt = c.getPixel(k);
			m.setValue(pt.x,pt.y,1);
		    }
		}
	    }
	}
	Mask m2 = m.getCopy();
	if(outline){
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(m.getValue(i,j) < 1) continue;
		    int sum = m.getValue(i-1,j-1)+m.getValue(i-1,j)+m.getValue(i-1,j+1)+m.getValue(i,j-1)+m.getValue(i,j+1)+m.getValue(i+1,j-1)+m.getValue(i+1,j)+m.getValue(i+1,j+1);
		    if(sum > 7) m2.setValue(i,j,0);
		}
	    }
	}
        return m2;
    }

    public Mask getSynapseMask(int chan, boolean outline){
        Mask m = new Mask(imWidth,imHeight);
	for(int icol = 0; icol < synapseCollections.size(); icol++){
	    SynapseCollection sc = synapseCollections.elementAt(icol);
	    int index = sc.getChannelIndex(chan);
	    if(index < 0) continue;
	    for(int i = 0; i < sc.getNSynapses(); i++){
		Synapse s = sc.getSynapse(i);
		Cluster c = s.getPunctum(index);
		for(int k = 0; k < c.size(); k++){
		    Point pt = c.getPixel(k);
		    m.setValue(pt.x,pt.y,1);
		}
	    }
	}
	Mask m2 = m.getCopy();
	if(outline){
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(m.getValue(i,j) < 1) continue;
		    int sum = m.getValue(i-1,j-1)+m.getValue(i-1,j)+m.getValue(i-1,j+1)+m.getValue(i,j-1)+m.getValue(i,j+1)+m.getValue(i+1,j-1)+m.getValue(i+1,j)+m.getValue(i+1,j+1);
		    if(sum > 7) m2.setValue(i,j,0);
		}
	    }
	}
        return m2;
    }

    public Mask getSynapseMaskFromCollection(int icol, boolean outline){
        Mask m = new Mask(imWidth,imHeight);
	SynapseCollection sc = synapseCollections.elementAt(icol);
	for(int i = 0; i < sc.getNSynapses(); i++){
	    Synapse s = sc.getSynapse(i);
	    for(int j = 0; j < s.getNPuncta(); j++){
		Cluster c = s.getPunctum(j);
		for(int k = 0; k < c.size(); k++){
		    Point pt = c.getPixel(k);
		    m.setValue(pt.x,pt.y,1);
		}
	    }
	}
	Mask m2 = m.getCopy();
	if(outline){
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(m.getValue(i,j) < 1) continue;
		    int sum = m.getValue(i-1,j-1)+m.getValue(i-1,j)+m.getValue(i-1,j+1)+m.getValue(i,j-1)+m.getValue(i,j+1)+m.getValue(i+1,j-1)+m.getValue(i+1,j)+m.getValue(i+1,j+1);
		    if(sum > 7) m2.setValue(i,j,0);
		}
	    }
	}
        return m2;
    }

    public Mask getSynapseMaskFromCollection(int icol, int chan, boolean outline){
        Mask m = new Mask(imWidth,imHeight);
	SynapseCollection sc = synapseCollections.elementAt(icol);
	int index = sc.getChannelIndex(chan);
	if(index < 0) return null;
	for(int i = 0; i < sc.getNSynapses(); i++){
	    Synapse s = sc.getSynapse(i);
	    Cluster c = s.getPunctum(index);
	    for(int k = 0; k < c.size(); k++){
		Point pt = c.getPixel(k);
		m.setValue(pt.x,pt.y,1);
	    }
	}
	Mask m2 = m.getCopy();
	if(outline){
	    for(int i = 1; i < imWidth-1; i++){
		for(int j = 1; j < imHeight-1; j++){
		    if(m.getValue(i,j) < 1) continue;
		    int sum = m.getValue(i-1,j-1)+m.getValue(i-1,j)+m.getValue(i-1,j+1)+m.getValue(i,j-1)+m.getValue(i,j+1)+m.getValue(i+1,j-1)+m.getValue(i+1,j)+m.getValue(i+1,j+1);
		    if(sum > 7) m2.setValue(i,j,0);
		}
	    }
	}
        return m2;
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

    public String getSynapseDensityTable(int postChan, String[] chanNames)
    {
	int nROI = regionsOfInterest.size();
	int ncol = synapseCollections.size();
	double[] areas = new double[nROI];
	int[][] nSynapses = new int[ncol][nROI];
	int[][] sumSynapseSizes = new int[ncol][nROI];
	int[][][] sumOverlaps = new int[ncol][10][nROI];
	int[][][] sumSynapticPunctaSizes = new int[ncol][10][nROI];
	int[] nReqs = new int[ncol];
	int[][] nPuncta = new int[puncta.size()][nROI];
	int[][] sumPunctaSizes = new int[puncta.size()][nROI];

	for(int r = 0; r < regionsOfInterest.size(); r++){
	    Mask roi = regionsOfInterest.elementAt(r);
	    areas[r] = 0;
	    for(int i = 0; i < roi.getWidth(); i++){
		for(int j = 0; j < roi.getHeight(); j++){
		    if(roi.getValue(i,j) * signalMasks[postChan].getValue(i,j) > 0){
			areas[r] += 1.0;
		    }
		}
	    }
	    areas[r] *= resolutionXY*resolutionXY;
	    for(int icol = 0; icol < ncol; icol++){
		nSynapses[icol][r] = 0;
		sumSynapseSizes[icol][r] = 0;
		SynapseCollection sc = synapseCollections.elementAt(icol);
		if(sc.allRequired()) nReqs[icol] = 1;
		else nReqs[icol] = sc.getNRequirements();
		int nChannels = sc.getNChannels();
		for(int j = 0; j < nReqs[icol]; j++) sumOverlaps[icol][j][r] = 0;
		for(int j = 0; j < nChannels; j++) sumSynapticPunctaSizes[icol][j][r] = 0;
		for(int i = 0; i < sc.getNSynapses(); i++){
		    Synapse s = sc.getSynapse(i);
		    Point p = s.getCentroid();
		    if(roi.getValue(p.x,p.y) > 0){
			nSynapses[icol][r]++;
			sumSynapseSizes[icol][r] += s.size();
			if(sc.allRequired()) sumOverlaps[icol][0][r] += s.getClusterOverlap();
			else{
			    for(int j = 0; j < nReqs[icol]; j++) sumOverlaps[icol][j][r] += s.getClusterOverlap(sc.getRequiredColocalization(j));
			}
			for(int j = 0; j < nChannels; j++) sumSynapticPunctaSizes[icol][j][r] += s.getPunctum(j).size();
		    }
		}
	    }
	    for(int chan = 0; chan < puncta.size(); chan++){
		Vector<Cluster> clusters = puncta.elementAt(chan);
		nPuncta[chan][r] = 0;
		sumPunctaSizes[chan][r] = 0;
		for(int i = 0; i < clusters.size(); i++){
		    Point p = clusters.elementAt(i).getCentroid();
		    if(roi.getValue(p.x,p.y) > 0){
			nPuncta[chan][r]++;
			sumPunctaSizes[chan][r] += clusters.elementAt(i).size();
		    }
		}
	    }
	}
	
	String msg = "Field,";
	for(int r = 0; r < nROI; r++) msg += "\"Region "+r+"\",";
	msg += "Average,Description\n";
	msg += "\"Dendrite area (um^2)\",";
	double sum = 0;
	for(int r = 0; r < nROI; r++){
	    msg += ""+areas[r]+",";
	    sum += areas[r];
	}
	msg += "" + (sum/nROI) + ",-\n";
	for(int icol = 0; icol < ncol; icol++){
	    SynapseCollection sc = synapseCollections.elementAt(icol);
	    msg += "\"Type "+icol+" synapses\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		msg += ""+nSynapses[icol][r]+",";
		sum += nSynapses[icol][r];
	    }
	    msg += "" + (sum/nROI) + ",\""+sc.getDescription()+"\"\n\"Density (per 100 um^2)\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		double densityA = 100 * nSynapses[icol][r]/areas[r];
		msg += ""+densityA+",";
		sum += densityA;
	    }
	    msg += "" + (sum/nROI) + ",-\n\"Average size\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		double avgSize = ((double)sumSynapseSizes[icol][r])/nSynapses[icol][r];
		msg += ""+avgSize+",";
		sum += avgSize;
	    }
	    msg += "" + (sum/nROI) + ",-\n";
	    for(int i = 0; i < sc.getNChannels(); i++){
		msg += "\"Average puncta "+i+" size\",";
		sum = 0;
		for(int r = 0; r < nROI; r++){
		    double avgSize = ((double)sumSynapticPunctaSizes[icol][i][r])/nSynapses[icol][r];
		    msg += ""+avgSize+",";
		    sum += avgSize;
		}
		msg += "" + (sum/nROI) + "," + chanNames[sc.getChannel(i)] + "\n";
	    }
	    for(int i = 0; i < nReqs[icol]; i++){
		msg += "\"Average overlap "+i+"\",";
		sum = 0;
		for(int r = 0; r < nROI; r++){
		    double avgOverlap = ((double)sumOverlaps[icol][i][r])/nSynapses[icol][r];
		    msg += ""+avgOverlap+",";
		    sum += avgOverlap;
		}
		msg += "" + (sum/nROI) + ",";
		if(sc.allRequired()) msg += "-\n";
		else{
		    int[] req = sc.getRequiredColocalization(i);
		    //System.out.println(sc.getChannelIndex(req[0]));
		    msg += "\""+chanNames[sc.getChannel(req[0])];
		    for(int j = 1; j < req.length; j++) msg += " and " + chanNames[sc.getChannel(req[j])];
		    msg += "\"\n";
		}
	    }
	    msg += "-,-,-,-,-,-\n";
	}
	for(int chan = 0; chan < puncta.size(); chan++){
	    msg += "\""+chanNames[chan]+" puncta\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		msg += ""+nPuncta[chan][r]+",";
		sum += nPuncta[chan][r];
	    }
	    msg += "" + (sum/nROI) + ",-\n\"Density (per 100 um^2)\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		double densityA = 100 * nPuncta[chan][r]/areas[r];
		msg += ""+densityA+",";
		sum += densityA;
	    }
	    msg += "" + (sum/nROI) + ",-\n\"Average size\",";
	    sum = 0;
	    for(int r = 0; r < nROI; r++){
		double avgSize = ((double)sumPunctaSizes[chan][r])/nPuncta[chan][r];
		msg += ""+avgSize+",";
		sum += avgSize;
	    }
	    msg += "" + (sum/nROI) + ",-\n";
	    msg += "-,-,-,-,-,-\n";
	}
	return msg;
    }

    public String getSynapseDensity(int postChan, String[] chanNames, int collectionIndex)
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
	    if(axisLengths.size() > r) lengthEstimate = (double)axisLengths.elementAt(r);
	    int nSynapses = 0;
	    int sumOverlap = 0;
	    int sumSize = 0;
	    SynapseCollection sc = synapseCollections.elementAt(collectionIndex);
	    for(int i = 0; i < sc.getNSynapses(); i++){
		Synapse s = sc.getSynapse(i);
		Point p = s.getCentroid();
		if(roi.getValue(p.x,p.y) > 0){
		    nSynapses++;
		    sumSize += s.size();
		    sumOverlap += s.getClusterOverlap(sc.getChannels());
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
	writeDoubleToBuffer(buf,9,resolutionXY);
        fout.write(buf,0,17);
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
        writeShortToBuffer(buf,0,(short)synapseCollections.size());
	fout.write(buf,0,2);
	for(int k = 0; k < synapseCollections.size(); k++){
	    SynapseCollection sc = synapseCollections.elementAt(k);
	    byte[] nameBytes = sc.getDescription().getBytes();
	    writeIntToBuffer(buf,0,nameBytes.length);
	    fout.write(buf,0,4);
	    fout.write(nameBytes);
            buf[0] = (byte)(sc.getNChannels() & 0x000000ff);
	    for(int i = 0; i < sc.getNChannels(); i++) buf[i+1] = (byte)(sc.getChannel(i) & 0x000000ff);
	    writeShortToBuffer(buf,1+sc.getNChannels(),(short)sc.getNSynapses());
	    int offset = 3+sc.getNChannels();
	    for(int i = 0; i < sc.getNSynapses(); i++){
		Synapse s = sc.getSynapse(i);
		writeDoubleToBuffer(buf,offset,s.getColocalizationScore());
		offset += 8;
		for(int j = 0; j < sc.getNChannels(); j++){
		    writeShortToBuffer(buf,offset,(short)s.getPunctumIndex(j));
		    offset += 2;
		}
	    }
	    fout.write(buf,0,offset);
	    writeIntToBuffer(buf,0,sc.getOverlapThreshold());
	    writeDoubleToBuffer(buf,4,sc.getDistanceThreshold());
	    if(sc.allRequired()){
		buf[12] = 1;
		fout.write(buf,0,13);
	    }
	    else{
		buf[12] = 0;
		writeIntToBuffer(buf,13,sc.getNRequirements());
		offset = 17;
		for(int i = 0; i < sc.getNRequirements(); i++){
		    int[] req = sc.getRequiredColocalization(i);
		    buf[offset] = (byte)(req.length & 0x000000ff);
		    offset++;
		    for(int j = 0; j < req.length; j++){
			buf[offset] = (byte)(req[j] & 0x000000ff);
			offset++;
		    }
		}
		fout.write(buf,0,offset);
	    }
	}
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
	int nAxes = axisLengths.size();
	buf[0] = (byte)nAxes;
	for(int i = 0; i < nAxes; i++) writeIntToBuffer(buf, 1+(4*i), axisLengths.elementAt(i));
	fout.write(buf,0,1+(4*nAxes));
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

    public static void writeDoubleToBuffer(byte[] buffer, int offset, double data)
    {
	long ldata = Double.doubleToLongBits(data);
        buffer[offset] = (byte)(ldata & 0xff);
        buffer[offset+1] = (byte)((ldata >> 8) & 0xff);
        buffer[offset+2] = (byte)((ldata >> 16) & 0xff);
        buffer[offset+3] = (byte)((ldata >> 24) & 0xff);
        buffer[offset+4] = (byte)((ldata >> 32) & 0xff);
        buffer[offset+5] = (byte)((ldata >> 40) & 0xff);
        buffer[offset+6] = (byte)((ldata >> 48) & 0xff);
        buffer[offset+7] = (byte)((ldata >> 56) & 0xff);
    }
    
    public static ImageReport read(RandomAccessFile fin) throws IOException
    {
        byte[] buf = new byte[400000];
        fin.read(buf,0,17);
        int nc = (int)buf[0];
        if((nc & 0x000000ff) == 0x000000ff) return null;
        int width = readIntFromBuffer(buf,1);
        int height = readIntFromBuffer(buf,5);
	double res = readDoubleFromBuffer(buf,9);
	ImageReport r = new ImageReport(nc,width,height);
	r.setResolutionXY(res);
	//System.out.println(""+nc+", "+width+", "+height);
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
	    //System.out.println(np);
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
	int ncol = readShortFromBuffer(buf,0);
	for(int k = 0; k < ncol; k++){
	    fin.read(buf,0,4);
	    int length = ImageReport.readIntFromBuffer(buf,0);
	    byte[] nameBytes = new byte[length];
	    fin.read(nameBytes,0,length);
	    fin.read(buf,0,1);
	    int nchan = (int)buf[0];
	    fin.read(buf,0,nchan+2);
            int [] chans = new int[nchan];
	    for(int i = 0; i < nchan; i++) chans[i] = (int)buf[i];
	    SynapseCollection sc = new SynapseCollection(chans);
	    sc.setDescription(new String(nameBytes));
	    r.addSynapseCollection(sc);
	    int ns = readShortFromBuffer(buf,nchan);
	    //System.out.println(ns);
            int [] ids = new int[nchan];
	    fin.read(buf,0,ns*(8+2*nchan));
	    for(int i = 0; i < ns; i++){
		double score = readDoubleFromBuffer(buf,(8+2*nchan)*i);
		for(int j = 0; j < nchan; j++){
		    ids[j] = readShortFromBuffer(buf,i*(8+2*nchan) + 2*j + 8);
		}
		Synapse s = new Synapse();
		for(int j = 0; j < nchan; j++){
		    s.addPunctum(r.getPunctum(chans[j],ids[j]),ids[j]);
		}
		s.setColocalizationScore(score);
		sc.addSynapse(s);
	    }
	    fin.read(buf,0,13);
	    sc.setOverlapThreshold(readIntFromBuffer(buf,0));
	    sc.setDistanceThreshold(readDoubleFromBuffer(buf,4));
	    if(buf[12] != 1){
		sc.setRequireAll(false);
		fin.read(buf,0,4);
		int nreq = readIntFromBuffer(buf,0);
		for(int i = 0; i < nreq; i++){
		    fin.read(buf,0,1);
		    int[] req = new int[(int)buf[0]];
		    fin.read(buf,0,req.length);
		    for(int j = 0; j < req.length; j++) req[j] = (int)buf[j];
		    sc.addRequiredColocalizationByIndex(req);
		}
	    }
	    else sc.setRequireAll(true);
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
	fin.read(buf,0,1);
	int nAxes = buf[0];
	for(int i = 0; i < nAxes; i++){
	    fin.read(buf,0,4);
	    r.addAxis(readIntFromBuffer(buf,0));
	}
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

    public static double readDoubleFromBuffer(byte[] buffer, int offset)
    {
        long tmp =  ((buffer[offset+7] & 0xff) << 56) | ((buffer[offset+6] & 0xff) << 48) | ((buffer[offset+5] & 0xff) << 40) | ((buffer[offset+4] & 0xff) << 32) | ((buffer[offset+3] & 0xff) << 24) | ((buffer[offset+2] & 0xff) << 16) | ((buffer[offset+1] & 0xff) << 8) | (buffer[offset] & 0xff);
	return Double.longBitsToDouble(tmp);
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

    public void loadMetaMorphTraces(String phil, int w, boolean overwrite)
    {
	try{
	    if(overwrite || utilityMasks[w] == null) utilityMasks[w] = new Mask(imWidth,imHeight);
	    //Mask m = new Mask(imWidth,imHeight);
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
		int np = Integer.parseInt(st.nextToken());
		for(int i = 0; i < np; i++){
		    int x = Integer.parseInt(st.nextToken());
		    int y = Integer.parseInt(st.nextToken());
		    utilityMasks[w].setValue(x,y,1);
		}
		//utilityMasks[w].add(m);
		line = fin.readLine();
	    }
	    fin.close();
	}
	catch(IOException e){ e.printStackTrace(); }
    }
}
