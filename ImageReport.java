import java.awt.Point;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.Vector;

public class ImageReport
{
    private Mask[] outlierMasks;
    private Mask[] signalMasks;
    private Mask[] utilityMasks;
    private Vector<Vector<Cluster>> puncta;
    private Vector<Synapse> synapses;
    private int nChannels;
    
    public ImageReport(int nchan)
    {
        nChannels = nchan;
        outlierMasks = new Mask[nChannels];
        signalMasks = new Mask[nChannels];
	utilityMasks = new Mask[nChannels];
        puncta = new Vector<Vector<Cluster>>();
        for(int i = 0; i < nchan; i++) puncta.add(new Vector<Cluster>());
        synapses = new Vector<Synapse>();
    }
    
    public int getNChannels(){ return nChannels; }
    
    public void setOutlierMask(int chan, Mask m){ outlierMasks[chan] = m; }
    
    public void setSignalMask(int chan, Mask m){ signalMasks[chan] = m; }
    
    public void setUtilityMask(int chan, Mask m){ utilityMasks[chan] = m; }
    
    public Mask getOutlierMask(int chan){ return outlierMasks[chan]; }
    
    public Mask getSignalMask(int chan){ return signalMasks[chan]; }

    public Mask getUtilityMask(int chan){ return utilityMasks[chan]; }
    
    public void addPunctum(int chan, Cluster c){ puncta.elementAt(chan).add(c); }
    
    public void removePunctum(int chan, int index){ puncta.elementAt(chan).remove(index); }
    
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
    
    public void write(RandomAccessFile fout) throws IOException
    {
        byte[] buf = new byte[400000];
        int width = outlierMasks[0].getWidth();
        int height = outlierMasks[0].getHeight();
        buf[0] = (byte)(nChannels & 0x000000ff);
        writeIntToBuffer(buf,1,width);
        writeIntToBuffer(buf,5,height);
        fout.write(buf,0,9);
        for(int c = 0; c < nChannels; c++){
            for(int j = 0; j < height; j++){
                for(int i = 0; i < width; i++){
                    buf[i] = (byte)(outlierMasks[c].getValue(i,j) & 0x000000ff);
                    buf[width+i] = (byte)(signalMasks[c].getValue(i,j) & 0x000000ff);
		    //buf[2*width+i] = (byte)(utilityMasks[c].getValue(i,j) & 0x000000ff);
                }
                fout.write(buf,0,2*width);
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
        ImageReport r = new ImageReport(nc);
        int width = readIntFromBuffer(buf,1);
        int height = readIntFromBuffer(buf,5);
        for(int c = 0; c < nc; c++){
            Mask om = new Mask(width,height);
            Mask sm = new Mask(width,height);
	    //Mask um = new Mask(width,height);
            for(int j = 0; j < height; j++){
                fin.read(buf,0,2*width);
                for(int i = 0; i < width; i++){
                    om.setValue(i,j,(int)buf[i]);
                    sm.setValue(i,j,(int)buf[width+i]);
		    //um.setValue(i,j,(int)buf[2*width+i]);
                }
            }
            r.setOutlierMask(c,om);
            r.setSignalMask(c,sm);
	    //r.setUtilityMask(c,sm);
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
}
