public class Fitter
{
    private Function funk;
    private double[] parameters;
    private int[] range;
    private int[] dist;

    public Fitter(Function f, int min, int max, double[] par)
    {
	funk = f;
	range = new int[2];
	range[0] = min;
	range[1] =  max;
	parameters = par;
    }

    public void setFunction(Function f){ funk = f; }

    public Function getFunction(){ return funk; }

    public void setDistribution(int[] d){ dist = d; }

    public void setParameters(double[] p){ parameters = p; }

    public double[] getParameters(){ return parameters; }

    public void setRange(int min, int max)
    {
	range[0] = min;
	range[1] = max;
    }

    public void fit(double[] guess)
    {
	parameters = guess;
	fit();
    }

    public void fit()
    {
	double[] iparam = new double[parameters.length];
	double[] step = new double[parameters.length];
	boolean[] fixed = new boolean[parameters.length];
	for(int i = 0; i < parameters.length; i++){
	    iparam[i] = parameters[i];
	    step[i] = 0.1 * parameters[i];
	    fixed[i] = false;
	}
	boolean allFixed = false;
	double maxLL = logLikelihood(parameters);
	int[] updown = new int[parameters.length];
	int counter = 0;
	while(!allFixed && counter < 1000){
	    allFixed = true;
	    for(int i = 0; i < parameters.length; i++){
		if(fixed[i]) continue;
		updown[i] = 0;
		iparam[i] = parameters[i] + step[i];
		double iLL = logLikelihood(iparam);
		if(iLL > maxLL){
		    maxLL = iLL;
		    updown[i] = 1;
		}
		iparam[i] = parameters[i] - step[i];
		iLL = logLikelihood(iparam);
		if(iLL > maxLL){
		    maxLL = iLL;
		    updown[i] = -1;
		}
	    }
	    for(int i = 0; i < parameters.length; i++){
		parameters[i] += updown[i]*step[i];
		if(updown[i] < 1){
		    step[i] *= 0.5;
		    if(updown[i] < 0) step[i] *= -1;
		}
		if(step[i]/parameters[i] < 0.001) fixed[i] = true;
		allFixed = allFixed && fixed[i];
	    }
	    counter++;
	}
    }

    public double logLikelihood(double[] param)
    {
	double sum = 0;
	for(int i = range[0]; i < range[1]; i++) sum += dist[i] * Math.log(funk.calculate(param,(double)i));
	return sum;
    }
}
