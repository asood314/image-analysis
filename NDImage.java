import java.awt.image.BufferedImage;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.StringTokenizer;

public class NDImage
{
    public static final byte WAVELENGTH = 0;
    public static final byte ZSLICE = 1;
    public static final byte TIMEPOINT = 2;
    public static final byte WIDTH = 3;
    public static final byte HEIGHT = 4;
    public static final byte POSITION = 5;
    
    private int[][][][][][] image;
    private int nWavelengths, nZ, nT, width, height, npos;
    
    public NDImage(int nwl, int nz, int np, String filename) throws IOException
    {
        init(nwl,nz,np,filename,"wztp");
    }
    
    public NDImage(int nwl, int nz, int np, String filename, String order) throws IOException
    {
        init(nwl,nz,np,filename,order);
    }
    
    public void init(int nwl, int nz, int np, String filename, String order) throws IOException
    {
        RandomAccessFile fin = new RandomAccessFile(filename,"r");
        byte[] buf = new byte[1024*1024*10];
        int[] offsets = new int[1000];
        int nOffsets = 0;
        fin.read(buf,0,8);
        if(convertToShort(buf[2],buf[3]) != 42) System.out.println("Whoops!");
        offsets[nOffsets] = this.convertToInt(buf[4],buf[5],buf[6],buf[7]);
        while(offsets[nOffsets] > 0)
        {
            fin.seek(offsets[nOffsets]);
            fin.read(buf,0,2);
            int nTags = this.convertToShort(buf[0],buf[1]);
            fin.seek(offsets[nOffsets] + 2 + 12*nTags);
            nOffsets++;
            fin.read(buf,0,4);
            offsets[nOffsets] = this.convertToInt(buf[0],buf[1],buf[2],buf[3]);
        }
        //System.out.println(nOffsets);
        nWavelengths = nwl;
        nZ = nz;
        npos = np;
        nT = nOffsets / (nwl*nz*np);
        fin.seek(offsets[0]);
        fin.read(buf,0,2);
        int nTags = this.convertToShort(buf[0],buf[1]);
        width = 1;
        height = 1;
        for(int i = 0; i < nTags; i++)
        {
            fin.read(buf,0,12);
            int tag = this.convertToShort(buf[0],buf[1]);
            if(tag == 256) width = this.convertToInt(buf[8],buf[9],buf[10],buf[11]);
            else if(tag == 257) height = this.convertToInt(buf[8],buf[9],buf[10],buf[11]);
        }
        image = new int[nWavelengths][nZ][nT][width][height][npos];
        StringTokenizer st = new StringTokenizer(order,"wztp",true);
        int[] wztp = {0,0,0,0};
        int[] nwztp = {nwl,nz,nT,np};
        int[] indices = {0,0,0,0};
        for(int i = 0; i < 4; i++){
            String tok = st.nextToken();
            if(tok.equals("w")) indices[i] = 0;
            else if(tok.equals("z")) indices[i] = 1;
            else if(tok.equals("t")) indices[i] = 2;
            else if(tok.equals("p")) indices[i] = 3;
        }
        for(int ifd = 0; ifd < nOffsets; ifd++)
        {
            fin.seek(offsets[ifd]);
            fin.read(buf,0,2);
            nTags = this.convertToShort(buf[0],buf[1]);
            int nStrips = 0;
            int stripOffsets = 0;
            int stripByteCountOffsets = 0;
            for(int i = 0; i < nTags; i++)
            {
                fin.read(buf,0,12);
                int tag = this.convertToShort(buf[0],buf[1]);
                if(tag == 273){
                    nStrips = this.convertToInt(buf[4],buf[5],buf[6],buf[7]);
                    stripOffsets = this.convertToInt(buf[8],buf[9],buf[10],buf[11]);
                }
                else if(tag == 279){
                    stripByteCountOffsets = this.convertToInt(buf[8],buf[9],buf[10],buf[11]);
                }
            }
            wztp[indices[0]] = ifd % nwztp[indices[0]];
            wztp[indices[1]] = (ifd % (nwztp[indices[1]]*nwztp[indices[0]])) / nwztp[indices[0]];
            wztp[indices[2]] = (ifd % (nwztp[indices[2]]*nwztp[indices[1]]*nwztp[indices[0]])) / (nwztp[indices[1]]*nwztp[indices[0]]);
            wztp[indices[3]] = ifd / (nwztp[indices[2]]*nwztp[indices[1]]*nwztp[indices[0]]);
            int pos = wztp[3];
            int t = wztp[2];
            int z = wztp[1];
            int wl = wztp[0];
            //System.out.println(t+", "+z+", "+wl);
            //System.out.println(nStrips);
            int index = 0;
            for(int i = 0; i < nStrips; i++)
            {
                fin.seek(stripByteCountOffsets + 4*i);
                fin.read(buf,0,4);
                int nbytes = this.convertToInt(buf[0],buf[1],buf[2],buf[3]);
                //System.out.println(nbytes);
                fin.seek(stripOffsets + 4*i);
                fin.read(buf,0,4);
                int stripOff = this.convertToInt(buf[0],buf[1],buf[2],buf[3]);
                fin.seek(stripOff);
                fin.read(buf,0,nbytes);
                for(int pixel = 0; pixel < nbytes; pixel += 2)
                {
                    int y = index / width;
                    int x = index - width*y;
                    //System.out.println(x);
                    //System.out.println(y);
                    image[wl][z][t][x][y][pos] = this.convertToShort(buf[pixel],buf[pixel+1]);
                    index++;
                }
            }
        }
        fin.close();
        //System.out.println(image[0][0][0][0][0]);
        //System.out.println(image[0][0][0][1][0]);
    }
    
    public NDImage(int[][][][][][] im)
    {
        image = im;
        nWavelengths = im.length;
        nZ = im[0].length;
        nT = im[0][0].length;
        width = im[0][0][0].length;
        height = im[0][0][0][0].length;
        npos = im[0][0][0][0][0].length;
    }
    
    public void append(NDImage ndim, byte dimension)
    {
        int [][][][][][] im;
        if(dimension == WAVELENGTH){
            if(nZ != ndim.getNZ() || nT != ndim.getNT() || width != ndim.getWidth() || height != ndim.getHeight() || npos != ndim.getNPos()){
                System.out.println("Append(WAVELENGTH) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths+ndim.getNWavelengths()][nZ][nT][width][height][npos];
            for(int z = 0; z < nZ; z++){
                for(int t = 0; t < nT; t++){
                    for(int x = 0; x < width; x++){
                        for(int y = 0; y < height; y++){
                            for(int p = 0; p < npos; p++){
                                for(int w = 0; w < nWavelengths; w++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int w = 0; w < ndim.getNWavelengths(); w++) im[nWavelengths+w][z][t][x][y][p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            nWavelengths += ndim.getNWavelengths();
        }
        else if(dimension == ZSLICE){
            if(nWavelengths != ndim.getNWavelengths() || nT != ndim.getNT() || width != ndim.getWidth() || height != ndim.getHeight() || npos != ndim.getNPos()){
                System.out.println("Append(ZSLICE) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths][nZ+ndim.getNZ()][nT][width][height][npos];
            for(int w = 0; w < nWavelengths; w++){
                for(int t = 0; t < nT; t++){
                    for(int x = 0; x < width; x++){
                        for(int y = 0; y < height; y++){
                            for(int p = 0; p < npos; p++){
                                for(int z = 0; z < nZ; z++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int z = 0; z < ndim.getNZ(); z++) im[w][nZ+z][t][x][y][p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            nZ += ndim.getNZ();
        }
        else if(dimension == TIMEPOINT){
            if(nWavelengths != ndim.getNWavelengths() || nZ != ndim.getNZ() || width != ndim.getWidth() || height != ndim.getHeight() || npos != ndim.getNPos()){
                System.out.println("Append(TIMEPOINT) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths][nZ][nT+ndim.getNT()][width][height][npos];
            for(int w = 0; w < nWavelengths; w++){
                for(int z = 0; z < nZ; z++){
                    for(int x = 0; x < width; x++){
                        for(int y = 0; y < height; y++){
                            for(int p = 0; p < npos; p++){
                                for(int t = 0; t < nT; t++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int t = 0; t < ndim.getNT(); t++) im[w][z][nT+t][x][y][p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            nT += ndim.getNT();
        }
        else if(dimension == WIDTH){
            if(nWavelengths != ndim.getNWavelengths() || nZ != ndim.getNZ() || nT != ndim.getNT() || height != ndim.getHeight() || npos != ndim.getNPos()){
                System.out.println("Append(WIDTH) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths][nZ][nT][width+ndim.getWidth()][height][npos];
            for(int w = 0; w < nWavelengths; w++){
                for(int z = 0; z < nZ; z++){
                    for(int t = 0; t < nT; t++){
                        for(int y = 0; y < height; y++){
                            for(int p = 0; p < npos; p++){
                                for(int x = 0; x < width; x++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int x = 0; x < ndim.getWidth(); x++) im[w][z][t][width+x][y][p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            width += ndim.getWidth();
        }
        else if(dimension == HEIGHT){
            if(nWavelengths != ndim.getNWavelengths() || nZ != ndim.getNZ() || width != ndim.getWidth() || nT != ndim.getNT() || npos != ndim.getNPos()){
                System.out.println("Append(HEIGHT) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths][nZ][nT][width][height+ndim.getHeight()][npos];
            for(int w = 0; w < nWavelengths; w++){
                for(int z = 0; z < nZ; z++){
                    for(int t = 0; t < nT; t++){
                        for(int x = 0; x < width; x++){
                            for(int p = 0; p < npos; p++){
                                for(int y = 0; y < height; y++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int y = 0; y < ndim.getHeight(); y++) im[w][z][t][x][height+y][p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            height += ndim.getHeight();
        }
        else if(dimension == POSITION){
            if(nWavelengths != ndim.getNWavelengths() || nZ != ndim.getNZ() || width != ndim.getWidth() || nT != ndim.getNT() || height != ndim.getHeight()){
                System.out.println("Append(POSITION) failed: Dimension mismatch.");
                return;
            }
            im = new int[nWavelengths][nZ][nT][width][height][npos+ndim.getNPos()];
            for(int w = 0; w < nWavelengths; w++){
                for(int z = 0; z < nZ; z++){
                    for(int t = 0; t < nT; t++){
                        for(int x = 0; x < width; x++){
                            for(int y = 0; y < height; y++){
                                for(int p = 0; p < npos; p++) im[w][z][t][x][y][p] = image[w][z][t][x][y][p];
                                for(int p = 0; p < ndim.getNPos(); p++) im[w][z][t][x][y][npos+p] = ndim.getPixel(w,z,t,x,y,p);
                            }
                        }
                    }
                }
            }
            image = im;
            npos += ndim.getNPos();
        }
    }
    
    public static NDImage loadDiskoveryImage(String basename, int nw, String[] waves, int pos, int nt, int nz) throws IOException
    {
        NDImage ndim = null;
        if(nw > -1){
            if(pos > -1){
                if(nt > -1){
                    String phil = basename + "_w1" + waves[0] + "_s" + pos + "_t1.TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int t = 2; t < nt+1; t++){
                        phil = basename + "_w1" + waves[0] + "_s" + pos + "_t" + t + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                    }
                    for(int w = 2; w < nw+1; w++){
                        phil = basename + "_w" + w + waves[w-1] + "_s" + pos + "_t1.TIF";
                        NDImage tmp = new NDImage(1,nz,1,phil);
                        for(int t = 2; t < nt+1; t++){
                            phil = basename + "_w" + w + waves[w-1] + "_s" + pos + "_t" + t + ".TIF";
                            tmp.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                        }
                        ndim.append(tmp,NDImage.WAVELENGTH);
                    }
                }
                else{
                    String phil = basename + "_w1" + waves[0] + "_s" + pos + ".TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int w = 2; w < nw+1; w++){
                        phil = basename + "_w" + w + waves[w-1] + "_s" + pos + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.WAVELENGTH);
                    }
                }
            }
            else{
                if(nt > -1){
                    String phil = basename + "_w1" + waves[0] + "_t1.TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int t = 2; t < nt+1; t++){
                        phil = basename + "_w1" + waves[0] + "_t" + t + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                    }
                    for(int w = 2; w < nw+1; w++){
                        phil = basename + "_w" + w + waves[w-1] + "_t1.TIF";
                        NDImage tmp = new NDImage(1,nz,1,phil);
                        for(int t = 2; t < nt+1; t++){
                            phil = basename + "_w" + w + waves[w-1] + "_t" + t + ".TIF";
                            tmp.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                        }
                        ndim.append(tmp,NDImage.WAVELENGTH);
                    }
                }
                else{
                    String phil = basename + "_w1" + waves[0] + ".TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int w = 2; w < nw+1; w++){
                        phil = basename + "_w" + w + waves[w-1] + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.WAVELENGTH);
                    }
                }
            }
        }
        else{
            if(pos > -1){
                if(nt > -1){
                    String phil = basename + "_s" + pos + "_t1.TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int t = 2; t < nt+1; t++){
                        phil = basename + "_s" + pos + "_t" + t + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                    }
                }
                else{
                    String phil = basename + "_s" + pos + ".TIF";
                    ndim = new NDImage(1,nz,1,phil);
                }
            }
            else{
                if(nt > -1){
                    String phil = basename + "_t1.TIF";
                    ndim = new NDImage(1,nz,1,phil);
                    for(int t = 2; t < nt+1; t++){
                        phil = basename + "_t" + t + ".TIF";
                        ndim.append(new NDImage(1,nz,1,phil),NDImage.TIMEPOINT);
                    }
                }
                else{
                    String phil = basename + ".TIF";
                    ndim = new NDImage(1,nz,1,phil);
                }
            }
        }
        return ndim;
    }
    
    public static NDImage loadSamanthaData(String dir, String basename, int pos, int nz, int nt) throws IOException
    {
        String phil = dir + "g" + basename + "t01xy" + pos + "z1c1.tif";
        NDImage ndim = new NDImage(1,1,1,phil);
        for(int t = 2; t < nt+1; t++){
            if(t < 10) phil = dir + "g" + basename + "t0" + t + "xy" + pos + "z1c1.tif";
            else phil = dir + "g" + basename + "t" + t + "xy" + pos + "z1c1.tif";
            ndim.append(new NDImage(1,1,1,phil),NDImage.TIMEPOINT);
        }
        for(int z = 2; z < nz+1; z++){
            phil = dir + "g" + basename + "t01xy" + pos + "z" + z + "c1.tif";
            NDImage tmp = new NDImage(1,1,1,phil);
            for(int t = 2; t < nt+1; t++){
                if(t < 10) phil = dir + "g" + basename + "t0" + t + "xy" + pos + "z" + z + "c1.tif";
                else phil = dir + "g" + basename + "t" + t + "xy" + pos + "z" + z + "c1.tif";
                tmp.append(new NDImage(1,1,1,phil),NDImage.TIMEPOINT);
            }
            ndim.append(tmp,NDImage.ZSLICE);
        }
        phil = dir + "r" + basename + "t01xy" + pos + "z1c2.tif";
        NDImage tmp = new NDImage(1,1,1,phil);
        for(int t = 2; t < nt+1; t++){
            if(t < 10) phil = dir + "r" + basename + "t0" + t + "xy" + pos + "z1c2.tif";
            else phil = dir + "r" + basename + "t" + t + "xy" + pos + "z1c2.tif";
            tmp.append(new NDImage(1,1,1,phil),NDImage.TIMEPOINT);
        }
        for(int z = 2; z < nz+1; z++){
            phil = dir + "r" + basename + "t01xy" + pos + "z" + z + "c2.tif";
            NDImage tmp2 = new NDImage(1,1,1,phil);
            for(int t = 2; t < nt+1; t++){
                if(t < 10) phil = dir + "r" + basename + "t0" + t + "xy" + pos + "z" + z + "c2.tif";
                else phil = dir + "r" + basename + "t" + t + "xy" + pos + "z" + z + "c2.tif";
                tmp2.append(new NDImage(1,1,1,phil),NDImage.TIMEPOINT);
            }
            tmp.append(tmp,NDImage.ZSLICE);
        }
        ndim.append(tmp,NDImage.WAVELENGTH);
        return ndim;
    }
    
    private int convertToInt(int b0, int b1, int b2, int b3)
    {
        return ((b3 & 0xff) << 24) | ((b2 & 0xff) << 16) | ((b1 & 0xff) << 8) | (b0 & 0xff);
    }
    
    private int convertToShort(int b0, int b1)
    {
        return ((b1 & 0xff) << 8) | (b0 & 0xff);
    }
    
    public int getNWavelengths(){ return nWavelengths; }
    
    public int getNZ(){ return nZ; }
    
    public int getNT(){ return nT; }
    
    public int getWidth(){ return width; }
    
    public int getHeight(){ return height; }
    
    public int getNPos(){ return npos; }
    
    public int getPixel(int w, int z, int t, int x, int y, int p){ return image[w][z][t][x][y][p]; }
    
    public void divide(int a)
    {
        for(int w = 0; w < nWavelengths; w++){
            for(int z = 0; z < nZ; z++){
                for(int t = 0; t < nT; t++){
                    for(int p = 0; p < npos; p++){
                        for(int i = 0; i < width; i++){
                            for(int j = 0; j < height; j++){
                                image[w][z][t][i][j][p] = image[w][z][t][i][j][p] / a;
                            }
                        }
                    }
                }
            }
        }
    }
    
    public int[] getMinMax(int wl, int z, int t, int p)
    {
        int[] region = {0,0,width,height};
        return getMinMax(wl,z,t,p,region);
    }
    
    public int[] getMinMax(int wl, int z, int t, int p, int[] region)
    {
        int min = 999999;
        int max = 0;
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
                if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
            }
        }
        int[] retval = {min,max};
        return retval;
    }
    
    public int[] getMinMax(int[] wl, int z, int t, int p)
    {
        int[] region = {0,0,width,height};
        return getMinMax(wl,z,t,p,region);
    }
    
    public int[] getMinMax(int[] wl, int z, int t, int p, int[] region)
    {
        int[] minMax = new int[2*wl.length];
        for(int i = 0; i < wl.length; i++){
            minMax[2*i] = 999999;
            minMax[2*i+1] = 0;
        }
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    if(image[wl[w]][z][t][i][j][p] > minMax[2*w+1]) minMax[2*w+1] = image[wl[w]][z][t][i][j][p];
                    if(image[wl[w]][z][t][i][j][p] < minMax[2*w]) minMax[2*w] = image[wl[w]][z][t][i][j][p];
                }
            }
        }
        return minMax;
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p)
    {
        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int min = 999999;
        int max = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
                if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
            }
        }
        int[] region = {0,0,width,height};
        return getGrayImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p, int min, int max)
    {
        int[] region = {0,0,width,height};
        return getGrayImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p, int min, int max, int[] region)
    {
        BufferedImage bi = new BufferedImage(region[2], region[3], BufferedImage.TYPE_INT_RGB);
        double sf = 255.0 / (max - min);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int value = (int)((image[wl][z][t][i][j][p] - min) * sf);
                if(value > 255) value = 255;
                else if(value < 0) value = 0;
                value = 0xff000000 | (value << 16) | (value << 8) | value;
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        return bi;
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p, Mask m, int color)
    {
        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int min = 999999;
        int max = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
                if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
            }
        }
        int[] region = {0,0,width,height};
        return getGrayImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p, int min, int max, Mask m, int color)
    {
        int[] region = {0,0,width,height};
        return getGrayImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getGrayImage(int wl, int z, int t, int p, int min, int max, Mask m, int color, int[] region)
    {
        BufferedImage bi = new BufferedImage(region[2], region[3], BufferedImage.TYPE_INT_RGB);
        double sf = 255.0 / (max - min);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int value = (int)((image[wl][z][t][i][j][p] - min) * sf);
                if(value > 255) value = 255;
                else if(value < 0) value = 0;
                value = 0xff000000 | (value << 16) | (value << 8) | value;
                if(m.getValue(i,j) > 0) value = color;
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        return bi;
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p)
    {
        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int[] min = {999999,999999,999999};
        int[] max = {0,0,0};
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    if(image[wl[w]][z][t][i][j][p] > max[w]) max[w] = image[wl[w]][z][t][i][j][p];
                    if(image[wl[w]][z][t][i][j][p] < min[w]) min[w] = image[wl[w]][z][t][i][j][p];
                }
            }
        }
        int[] region = {0,0,width,height};
        return getRGBImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p, int[] min, int[] max)
    {
        int[] region = {0,0,width,height};
        return getRGBImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p, int[] min, int[] max, int [] region)
    {
        BufferedImage bi = new BufferedImage(region[2], region[3], BufferedImage.TYPE_INT_RGB);
        double[] sf = {0,0,0};
        sf[0] = 255.0 / (max[0] - min[0]);
        sf[1] = 255.0 / (max[1] - min[1]);
        sf[2] = 255.0 / (max[2] - min[2]);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int[] rgbPixel = {0,0,0};
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    rgbPixel[w] = (int)((image[wl[w]][z][t][i][j][p] - min[w]) * sf[w]);
                    if(rgbPixel[w] > 255) rgbPixel[w] = 255;
                    else if(rgbPixel[w] < 0) rgbPixel[w] = 0;
                }
                int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        return bi;
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p, Mask m, int color)
    {
        BufferedImage bi = new BufferedImage(width, height, BufferedImage.TYPE_INT_RGB);
        int[] min = {999999,999999,999999};
        int[] max = {0,0,0};
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    if(image[wl[w]][z][t][i][j][p] > max[w]) max[w] = image[wl[w]][z][t][i][j][p];
                    if(image[wl[w]][z][t][i][j][p] < min[w]) min[w] = image[wl[w]][z][t][i][j][p];
                }
            }
        }
        int[] region = {0,0,width,height};
        return getRGBImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p, int[] min, int[] max, Mask m, int color)
    {
        int[] region = {0,0,width,height};
        return getRGBImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getRGBImage(int[] wl, int z, int t, int p, int[] min, int[] max, Mask m, int color, int[] region)
    {
        BufferedImage bi = new BufferedImage(region[2], region[3], BufferedImage.TYPE_INT_RGB);
        double[] sf = {0,0,0};
        sf[0] = 255.0 / (max[0] - min[0]);
        sf[1] = 255.0 / (max[1] - min[1]);
        sf[2] = 255.0 / (max[2] - min[2]);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int[] rgbPixel = {0,0,0};
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    rgbPixel[w] = (int)((image[wl[w]][z][t][i][j][p] - min[w]) * sf[w]);
                    if(rgbPixel[w] > 255) rgbPixel[w] = 255;
                    else if(rgbPixel[w] < 0) rgbPixel[w] = 0;
                }
                int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                if(m.getValue(i,j) > 0) value = color;
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        return bi;
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p)
    {
        int[] min = {999999,999999,999999};
        int[] max = {0,0,0};
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    if(image[wl[w]][z][t][i][j][p] > max[w]) max[w] = image[wl[w]][z][t][i][j][p];
                    if(image[wl[w]][z][t][i][j][p] < min[w]) min[w] = image[wl[w]][z][t][i][j][p];
                }
            }
        }
        int[] region = {0,0,width,height};
        return getSplitImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p, int[] min, int[] max)
    {
        int[] region = {0,0,width,height};
        return getSplitImage(wl,z,t,p,min,max,region);
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p, int[] min, int[] max, int[] region)
    {
        BufferedImage bi = new BufferedImage(region[2]*2, region[3]*2, BufferedImage.TYPE_INT_RGB);
        double[] sf = {0,0,0};
        sf[0] = 255.0 / (max[0] - min[0]);
        sf[1] = 255.0 / (max[1] - min[1]);
        sf[2] = 255.0 / (max[2] - min[2]);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int[] rgbPixel = {0,0,0};
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    rgbPixel[w] = (int)((image[wl[w]][z][t][i][j][p] - min[w]) * sf[w]);
                    if(rgbPixel[w] > 255) rgbPixel[w] = 255;
                    else if(rgbPixel[w] < 0) rgbPixel[w] = 0;
                }
                int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        if(wl[0] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {(int)((image[wl[0]][z][t][i][j][p] - min[0]) * sf[0]),0,0};
                    if(rgbPixel[0] > 255) rgbPixel[0] = 255;
                    else if(rgbPixel[0] < 0) rgbPixel[0] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    bi.setRGB(region[2]+i-region[0],j-region[1],value);
                }
            }
        }
        if(wl[1] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {0,(int)((image[wl[1]][z][t][i][j][p] - min[1]) * sf[1]),0};
                    if(rgbPixel[1] > 255) rgbPixel[1] = 255;
                    else if(rgbPixel[1] < 0) rgbPixel[1] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    bi.setRGB(i-region[0],region[3]+j-region[1],value);
                }
            }
        }
        if(wl[2] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {0,0,(int)((image[wl[2]][z][t][i][j][p] - min[2]) * sf[2])};
                    if(rgbPixel[2] > 255) rgbPixel[2] = 255;
                    else if(rgbPixel[2] < 0) rgbPixel[2] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    bi.setRGB(region[2]+i-region[0],region[3]+j-region[1],value);
                }
            }
        }
        return bi;
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p, Mask m, int color)
    {
        int[] min = {999999,999999,999999};
        int[] max = {0,0,0};
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    if(image[wl[w]][z][t][i][j][p] > max[w]) max[w] = image[wl[w]][z][t][i][j][p];
                    if(image[wl[w]][z][t][i][j][p] < min[w]) min[w] = image[wl[w]][z][t][i][j][p];
                }
            }
        }
        int[] region = {0,0,width,height};
        return getSplitImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p, int[] min, int[] max, Mask m, int color)
    {
        int[] region = {0,0,width,height};
        return getSplitImage(wl,z,t,p,min,max,m,color,region);
    }
    
    public BufferedImage getSplitImage(int wl[], int z, int t, int p, int[] min, int[] max, Mask m, int color, int[] region)
    {
        BufferedImage bi = new BufferedImage(region[2]*2, region[3]*2, BufferedImage.TYPE_INT_RGB);
        double[] sf = {0,0,0};
        sf[0] = 255.0 / (max[0] - min[0]);
        sf[1] = 255.0 / (max[1] - min[1]);
        sf[2] = 255.0 / (max[2] - min[2]);
        for(int i = region[0]; i < region[0]+region[2]; i++){
            for(int j = region[1]; j < region[1]+region[3]; j++){
                int[] rgbPixel = {0,0,0};
                for(int w = 0; w < wl.length; w++){
                    if(wl[w] < 0) continue;
                    rgbPixel[w] = (int)((image[wl[w]][z][t][i][j][p] - min[w]) * sf[w]);
                    if(rgbPixel[w] > 255) rgbPixel[w] = 255;
                    else if(rgbPixel[w] < 0) rgbPixel[w] = 0;
                }
                int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                if(m.getValue(i,j) > 0) value = color;
                bi.setRGB(i-region[0],j-region[1],value);
            }
        }
        if(wl[0] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {(int)((image[wl[0]][z][t][i][j][p] - min[0]) * sf[0]),0,0};
                    if(rgbPixel[0] > 255) rgbPixel[0] = 255;
                    else if(rgbPixel[0] < 0) rgbPixel[0] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    if(m.getValue(width+i,j) > 0) value = color;
                    bi.setRGB(region[2]+i-region[0],j-region[1],value);
                }
            }
        }
        if(wl[1] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {0,(int)((image[wl[1]][z][t][i][j][p] - min[1]) * sf[1]),0};
                    if(rgbPixel[1] > 255) rgbPixel[1] = 255;
                    else if(rgbPixel[1] < 0) rgbPixel[1] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    if(m.getValue(i,height+j) > 0) value = color;
                    bi.setRGB(i-region[0],region[3]+j-region[1],value);
                }
            }
        }
        if(wl[2] >= 0){
            for(int i = region[0]; i < region[0]+region[2]; i++){
                for(int j = region[1]; j < region[1]+region[3]; j++){
                    int[] rgbPixel = {0,0,(int)((image[wl[2]][z][t][i][j][p] - min[2]) * sf[2])};
                    if(rgbPixel[2] > 255) rgbPixel[2] = 255;
                    else if(rgbPixel[2] < 0) rgbPixel[2] = 0;
                    int value = 0xff000000 | (rgbPixel[0] << 16) | (rgbPixel[1] << 8) | rgbPixel[2];
                    if(m.getValue(width+i,height+j) > 0) value = color;
                    bi.setRGB(region[2]+i-region[0],region[3]+j-region[1],value);
                }
            }
        }
        return bi;
    }

    public double percentile(double frac, int wl, int z, int t, int p)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int sum = 0;
	int target = (int)(width*height*frac);
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double percentile(double frac, int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int sum = 0;
	int target = (int)((x2-x1)*(y2-y1)*frac);
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double percentile(double frac, int wl, int z, int t, int p, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	int sum = 0;
	target = (int)(target*frac);
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double percentile(double frac, int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	int sum = 0;
	target = (int)(target*frac);
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double median(int wl, int z, int t, int p)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int sum = 0;
	int target = width*height/2;
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double median(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int sum = 0;
	int target = (x2-x1)*(y2-y1)/2;
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double median(int wl, int z, int t, int p, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	int sum = 0;
	target = target/2;
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }

    public double median(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	int sum = 0;
	target = target/2;
	int index;
	for(index = 0; sum < target; index++) sum += values[index];
	return (double)index;
    }
    
    public double mean(int wl, int z, int t, int p)
    {
        double sum = 0.0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                sum += image[wl][z][t][i][j][p];
            }
        }
        return sum / (width*height);
    }
    
    public double mean(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
        double sum = 0.0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                sum += image[wl][z][t][i][j][p];
            }
        }
        return sum / ((x2-x1)*(y2-y1));
    }
    
    public double mean(int wl, int z, int t, int p, Mask m)
    {
        double sum = 0.0;
        int npix = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                int a = m.getValue(i,j);
                sum += a * image[wl][z][t][i][j][p];
                npix += a;
            }
        }
        return sum / npix;
    }
    
    public double mean(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
        double sum = 0.0;
        int npix = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                int a = m.getValue(i,j);
                sum += a * image[wl][z][t][i][j][p];
                npix += a;
            }
        }
        return sum / npix;
    }
    
    public double std(int wl, int z, int t, int p)
    {
        double mean = mean(wl,z,t,p);
        double sum = 0.0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                sum += Math.pow(image[wl][z][t][i][j][p] - mean,2);
            }
        }
        return Math.sqrt(sum / (width*height - 1));
    }
    
    public double std(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
        double mean = mean(wl,z,t,p,x1,x2,y1,y2);
        double sum = 0.0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                sum += Math.pow(image[wl][z][t][i][j][p] - mean,2);
            }
        }
        return Math.sqrt(sum / ((x2-x1)*(y2-y1) - 1));
    }
    
    public double std(int wl, int z, int t, int p, Mask m)
    {
        double mean = mean(wl,z,t,p,m);
        double sum = 0.0;
        int npix = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                int a = m.getValue(i,j);
                sum += a * Math.pow(image[wl][z][t][i][j][p] - mean,2);
                npix += a;
            }
        }
        return Math.sqrt(sum / (npix - 1));
    }
    
    public double std(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
        double mean = mean(wl,z,t,p,x1,x2,y1,y2,m);
        double sum = 0.0;
        int npix = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                int a = m.getValue(i,j);
                sum += a * Math.pow(image[wl][z][t][i][j][p] - mean,2);
                npix += a;
            }
        }
        return Math.sqrt(sum / (npix - 1));
    }

    public double mode(int wl, int z, int t, int p)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int max = 0;
	int index = 0;
	for(int i = 0; i < 65536; i++){
	    if(values[i] > max){
		max = values[i];
		index = i;
	    }
	}
	return (double)index;
    }

    public double mode(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	int max = 0;
	int index = 0;
	for(int i = 0; i < 65536; i++){
	    if(values[i] > max){
		max = values[i];
		index = i;
	    }
	}
	return (double)index;
    }

    public double mode(int wl, int z, int t, int p, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
      	int max = 0;
	int index = 0;
	for(int i = 0; i < 65536; i++){
	    if(values[i] > max){
		max = values[i];
		index = i;
	    }
	}
	return (double)index;
    }

    public double mode(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
       	int max = 0;
	int index = 0;
	for(int i = 0; i < 65536; i++){
	    if(values[i] > max){
		max = values[i];
		index = i;
	    }
	}
	return (double)index;
    }

    public int min(int wl, int z, int t, int p)
    {
        int min = 65535;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
            }
        }
        return min;
    }
    
    public int min(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
        int min = 65535;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
            }
        }
        return min;
    }
    
    public int min(int wl, int z, int t, int p, Mask m)
    {
        int min = 65535;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(m.getValue(i,j) > 0){
		    if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
		}
            }
        }
        return min;
    }
    
    public int min(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
        int min = 65535;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                if(m.getValue(i,j) > 0){
		    if(image[wl][z][t][i][j][p] < min) min = image[wl][z][t][i][j][p];
		}
            }
        }
        return min;
    }

    public int max(int wl, int z, int t, int p)
    {
        int max = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
            }
        }
        return max;
    }
    
    public int max(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
        int max = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
            }
        }
        return max;
    }
    
    public int max(int wl, int z, int t, int p, Mask m)
    {
        int max = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(m.getValue(i,j) > 0){
		    if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
		}
            }
        }
        return max;
    }
    
    public int max(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
        int max = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                if(m.getValue(i,j) > 0){
		    if(image[wl][z][t][i][j][p] > max) max = image[wl][z][t][i][j][p];
		}
            }
        }
        return max;
    }

    public int[] getDistribution(int wl, int z, int t, int p)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	return values;
    }

    public int[] getDistribution(int wl, int z, int t, int p, int x1, int x2, int y1, int y2)
    {
	int[] values = new int[65536];
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                values[image[wl][z][t][i][j][p]]++;
            }
        }
	return values;
    }

    public int[] getDistribution(int wl, int z, int t, int p, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	return values;
    }

    public int[] getDistribution(int wl, int z, int t, int p, int x1, int x2, int y1, int y2, Mask m)
    {
	int[] values = new int[65536];
	int target = 0;
	for(int i = 0; i < 65536; i++) values[i] = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
		int a = m.getValue(i,j);
                values[image[wl][z][t][i][j][p]] += a;
		target += a;
            }
        }
	return values;
    }
    
    public NDImage zprojection()
    {
        int[][][][][][] im = new int[nWavelengths][1][nT][width][height][npos];
        for(int w = 0; w < nWavelengths; w++){
            for(int t = 0; t < nT; t++){
                for(int i = 0; i < width; i++){
                    for(int j = 0; j < height; j++){
                        for(int p = 0; p < npos; p++){
                            int max = 0;
                            for(int z = 0; z < nZ; z++){
                                if(image[w][z][t][i][j][p] > max) max = image[w][z][t][i][j][p];
                            }
                            im[w][0][t][i][j][p] = max;
                        }
                    }
                }
            }
        }
        return new NDImage(im);
    }
}
