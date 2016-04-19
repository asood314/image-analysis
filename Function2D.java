public interface Function2D
{
    public void setParameters(double[] param);
    
    public double[] getParameters();
    
    public double calculate(double x, double y);

    public double integral(double x1, double x2, double y1, double y2);
}
