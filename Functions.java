public class Functions
{
    
    public static Function poisson()
    {
        return new Function(){
            private double[] parameters = new double[1];// 1 parameter - mean
            public void setParameters(double[] param){ parameters = param; }
            public double[] getParameters(){ return parameters; }
            public double calculate(double x){
                return Math.exp(x - parameters[0] + x*(Math.log(parameters[0])-Math.log(x)) -0.5*Math.log(2*Math.PI*x));
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
        };
    }

    public static Function poisson2()
    {
	return new Function(){
	    private double[] parameters = new double[2]; // 2 parameters - mean, ratio mean to var
            public void setParameters(double[] param){ parameters = param; }
            public double[] getParameters(){ return parameters; }
            public double calculate(double x){
		double m = parameters[0];
		double a = parameters[1];
                return (1.0/a)*Math.exp((x - m)/a + (((a-1)*m + x)/a)*(Math.log(a*m)-Math.log((a-1)*m+x)) -0.5*Math.log((2/a)*Math.PI*((a-1)*m + x)));
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
	};
    }

    public static Function gaussian()
    {
        return new Function(){
            private double[] parameters = new double[2];// 2 parameters - mean, standard deviation
            public void setParameters(double[] param){ parameters = param; }
            public double[] getParameters(){ return parameters; }
            public double calculate(double x){
                return Math.exp(-(x-parameters[0])*(x-parameters[0])/(2*parameters[1]*parameters[1]))/(parameters[1]*Math.sqrt(2*Math.PI));
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
        };
    }
    
    public static Function gamma()
    {
        return new Function(){
            private double[] parameters = new double[2];// 2 parameters - mean, variance
            public void setParameters(double[] param){ parameters = param; }
            public double[] getParameters(){ return parameters; }
            public double calculate(double x){
                double theta = parameters[1]/parameters[0];
                double k = parameters[0]/theta;
                return Math.exp(k - 1 - x/theta + (k - 1)*Math.log(x) - k*Math.log(theta) - 0.5*Math.log(2*Math.PI*(k-1)));
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
        };
    }

    public static Function exponentialDecay()
    {
	return new Function(){
            private double[] ps = new double[5];// 3 parameters - maximum, decay constant, constant offset, norm, range;
            public void setParameters(double[] param){ ps = param; }
            public double[] getParameters(){ return ps; }
            public double calculate(double x){
                return ((ps[3] - ps[2]*ps[4])/(ps[1]*Math.exp(ps[0]/ps[1]))) * Math.exp(-(1.0/ps[1])*(x-ps[0])) + ps[2];
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
        };
    }
    
    public static Function convolution(final Function f1, final Function f2, final int pIndex)
    {
        return new Function(){
            private Function funk1 = f1;
            private Function funk2 = f2;
            int convolutionParameter = pIndex;
            public void setParameters(double[] p){
                double[] param1 = f1.getParameters();
                double[] param2 = f2.getParameters();
                for(int i = 0; i < param1.length; i++) param1[i] = p[i];
                for(int i = 0; i < param2.length; i++) param2[i] = p[i+param1.length];
                f1.setParameters(param1);
                f2.setParameters(param2);
            }
            public double[] getParameters(){
                double[] param1 = f1.getParameters();
                double[] param2 = f2.getParameters();
                double[] p = new double[param1.length+param2.length];
                for(int i = 0; i < param1.length; i++) p[i] = param1[i];
                for(int i = 0; i < param2.length; i++) p[i+param1.length] = param2[i];
                return p;
            }
            public double calculate(double x){
                double sum = 0;
                double[] param1 = f1.getParameters();
                for(int y = 0; y < 65536; y++){
                    param1[convolutionParameter] = (double)y;
                    f1.setParameters(param1);
                    sum += f1.calculate(x)*f2.calculate((double)y);
                }
                return sum;
            }
            public double integral(double x1, double x2){
                double sum = 0;
                for(double x = x1; x < x2; x += 1.0) sum += calculate(x);
                return sum;
            }
        };
    }
}
