public class Functions
{
    
    public static Function poisson()
    {
	return new Function(){
	    public double calculate(double[] param, double x){
		return Math.exp(x - param[0] + x*(Math.log(param[0])-Math.log(x)) -0.5*Math.log(2*Math.PI*x));
	    }
	    public double integral(double[] param, double x1, double x2){
		double sum = 0;
		for(double x = x1; x < x2; x += 1.0) sum += calculate(param,x);
		return sum;
	    }
	};
    }

    public static Function gaussian()
    {
	return new Function(){
	    public double calculate(double[] param, double x){
		return Math.exp(-(x-param[0])*(x-param[0])/(2*param[1]*param[1]))/(param[1]*Math.sqrt(2*Math.PI));
	    }
	    public double integral(double[] param, double x1, double x2){
		double sum = 0;
		for(double x = x1; x < x2; x += 1.0) sum += calculate(param,x);
		return sum;
	    }
	};
    }
}
