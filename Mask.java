public class Mask
{
    private int[][] mask;
    private int width;
    private int height;
    
    public Mask(int w, int h)
    {
        width = w;
        height = h;
        mask = new int[width][height];
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                mask[i][j] = 0;
            }
        }
    }
    
    public Mask(int w, int h, boolean startLow)
    {
        width = w;
        height = h;
        mask = new int[width][height];
        if(startLow){
            for(int i = 0; i < width; i++){
                for(int j = 0; j < height; j++){
                    mask[i][j] = 0;
                }
            }
        }
        else{
            for(int i = 0; i < width; i++){
                for(int j = 0; j < height; j++){
                    mask[i][j] = 1;
                }
            }
        }
    }
    
    public int getWidth(){ return width; }
    
    public int getHeight(){ return height; }
    
    public int getValue(int x, int y){ return mask[x][y]; }
    
    public void setValue(int x, int y, int val){ mask[x][y] = val; }
    
    public Mask getInverse()
    {
        Mask inv = new Mask(width,height);
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                inv.setValue(i,j,1 - mask[i][j]);
            }
        }
        return inv;
    }
    
    public void multiply(int val)
    {
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                mask[i][j] *= val;
            }
        }
    }
    
    public void multiply(Mask m)
    {
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                mask[i][j] *= m.getValue(i,j);
            }
        }
    }
    
    public void add(Mask m)
    {
        if(m == null) return;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                mask[i][j] += m.getValue(i,j);
            }
        }
    }

    public void add(Mask m, int coeff)
    {
        if(m == null) return;
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                mask[i][j] += coeff*m.getValue(i,j);
            }
        }
    }
    
    public void add(Mask m, int xOffset, int yOffset)
    {
        if(m == null) return;
        for(int i = 0; i < m.getWidth(); i++){
            for(int j = 0; j < m.getHeight(); j++){
                mask[xOffset+i][yOffset+j] += m.getValue(i,j);
            }
        }
    }

    public void add(Mask m, int x1, int x2, int y1, int y2)
    {
        if(m == null) return;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                mask[i][j] += m.getValue(i,j);
            }
        }
    }
    
    public void threshold(int val)
    {
        for(int i = 0; i < width; i++){
            for(int j = 0; j < height; j++){
                if(mask[i][j] > val) mask[i][j] = 1;
                else mask[i][j] = 0;
            }
        }
    }
    
    public int sum(int x1, int x2, int y1, int y2)
    {
        int sum = 0;
        for(int i = x1; i < x2; i++){
            for(int j = y1; j < y2; j++){
                sum += mask[i][j];
            }
        }
        return sum;
    }

    public void clear(int x1, int x2, int y1, int y2)
    {
	for(int i = x1; i < x2; i++){
	    for(int j = y1; j < y2; j++){
		mask[i][j] = 0;
	    }
	}
    }
}
