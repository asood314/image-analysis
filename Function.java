public interface Function
{
    public void setParameters(double[] param);
    
    public double[] getParameters();
    
    public double calculate(double x);

    public double integral(double x1, double x2);
}
