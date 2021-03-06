public class Fitter
{
    private Function funk;
    private int[] range;
    private int[] dist;
    private boolean[] fixedParameters;
    private double[] upperLimit;
    private double[] lowerLimit;

    public Fitter(Function f, int min, int max, double[] par)
    {
        funk = f;
        range = new int[2];
        range[0] = min;
        range[1] =  max;
        funk.setParameters(par);
	fixedParameters = new boolean[par.length];
	for(int i = 0; i < par.length; i++) fixedParameters[i] = false;
	upperLimit = new double[par.length];
	lowerLimit = new double[par.length];
    }

    public void setFunction(Function f){ funk = f; }

    public Function getFunction(){ return funk; }

    public void setDistribution(int[] d){ dist = d; }

    public void setRange(int min, int max)
    {
        range[0] = min;
        range[1] = max;
    }

    public void fixParameter(int parIndex){ fixedParameters[parIndex] = true; }

    public void setUpperLimit(double[] ul){ upperLimit = ul; }
    
    public void setLowerLimit(double[] ll){ lowerLimit = ll; }

    public void setLimits(int index, double lower, double upper)
    {
	upperLimit[index] = upper;
	lowerLimit[index] = lower;
    }

    public void fit(double[] guess)
    {
        funk.setParameters(guess);
        fit();
    }

    public void fit()
    {
        double[] param = funk.getParameters();
        double[] iparam = new double[param.length];
        double[] step = new double[param.length];
        boolean[] fixed = new boolean[param.length];
	int nUnfixed = 0;
	int[] unfixed = new int[param.length];
	double[][] canvas = new double[param.length][10];
        for(int i = 0; i < param.length; i++){
            iparam[i] = param[i];
            step[i] = 0.1 * (upperLimit[i] - lowerLimit[i]);//param[i];
            fixed[i] = fixedParameters[i];
	    if(!fixed[i]){
		unfixed[nUnfixed] = i;
		for(int j = 0; j < 10; j++) canvas[i][j] = lowerLimit[i] + j*step[i];
		nUnfixed++;
	    }
        }
        boolean allFixed = false;
        double maxLL = logLikelihood();
	//System.out.println(maxLL);
	for(int j = 0; j < Math.pow(10,nUnfixed); j++){
	    for(int i = 0; i < nUnfixed; i++) iparam[unfixed[i]] = canvas[unfixed[i]][(j / ((int)Math.pow(10,i))) % 10];
	    funk.setParameters(iparam);
	    double iLL = logLikelihood();
	    if(iLL > maxLL){
		maxLL = iLL;
		for(int i = 0; i < param.length; i++) param[i] = iparam[i];
	    }
	}
	//System.out.println(maxLL);
	for(int i = 0; i < param.length; i++){
	    step[i] *= 0.5;
	    iparam[i] = param[i];
	}
        int[] updown = new int[param.length];
        int counter = 0;
        while(!allFixed && counter < 10000){
            allFixed = true;
            for(int i = 0; i < param.length; i++){
                if(fixed[i]) continue;
                updown[i] = 0;
                iparam[i] = param[i] + step[i];
                funk.setParameters(iparam);
                double iLL = logLikelihood();
                if(iLL > maxLL){
                    maxLL = iLL;
                    updown[i] = 1;
                }
                iparam[i] = param[i] - step[i];
                funk.setParameters(iparam);
                iLL = logLikelihood();
                if(iLL > maxLL){
                    maxLL = iLL;
                    updown[i] = -1;
                }
		iparam[i] = param[i];
            }
	    //System.out.println(maxLL);
            for(int i = 0; i < param.length; i++){
                param[i] += updown[i]*step[i];
		iparam[i] = param[i];
                if(updown[i] < 1){
                    step[i] *= 0.5;
                    if(updown[i] < 0) step[i] *= -1;
                }
                if(Math.abs(step[i]/param[i]) < 0.001) fixed[i] = true;
		else if(param[i] > upperLimit[i]){
		    param[i] = upperLimit[i];
		    fixed[i] = true;
		}
		else if(param[i] < lowerLimit[i]){
		    param[i] = lowerLimit[i];
		    fixed[i] = true;
		}
                allFixed = allFixed && fixed[i];
            }
            counter++;
        }
        if(counter > 9999) System.out.println("Failed to converge: Iteration limit reached.");
	else System.out.println("Converged in " + counter + " iterations.");
        funk.setParameters(param);
    }

    public double logLikelihood()
    {
        double sum = 0;
        for(int i = range[0]; i < range[1]; i++) sum += dist[i] * Math.log(funk.calculate((double)i));
        return sum;
    }
}
