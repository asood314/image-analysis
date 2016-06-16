import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Locale;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;

public class TemplateScript
{
    public static void main(String[] args)
    {
	String dir = "Z:\\For ALEX\\MIA test 6\\";
	String outdir = dir+"output\\";
	String mmDir = dir;
	String[] samples = {"131","135","136","140","142","143","144","150","151","154","155","157","183","184","185","189","192","193"};
	String[][] subSamples = {{"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5","6","7","8","9","10"},
				 {"1","2","3","4","5"},
				 {"1","2","3","4","5"}};
	
	try{
	    long t_start = System.currentTimeMillis();
	    if(args.length > 1 && !args[1].equals("all")){
		int index = -1;
		for(int i = 0; i < samples.length; i++){
		    if(samples[i].equals(args[1])){
			index = i;
			break;
		    }
		}
		if(index < 0) return;
		if(args[0].equals("density")){
		    String[] cNames = {"VAMP2","PSD95"};
		    ImageReport[] reps;
		    BufferedWriter fout;
		    String basename = "";
		    if(args.length > 2){
			reps = ImageAnalysisToolkit.readImageReports(outdir+args[1]+"_"+args[2]+".dat");
			basename = outdir+args[1]+"_"+args[2];
		    }
		    else{
			reps = ImageAnalysisToolkit.readImageReports(outdir+args[1]+".dat");
			basename = outdir+args[1];
		    }
		    for(int i = 0; i < reps.length; i++){
			reps[i].loadMetaMorphRegions(mmDir+"regions_"+args[1]+"_vamppsdxy"+subSamples[index][i]+"c2.rgn");
			reps[i].setResolutionXY(0.046);
			fout = new BufferedWriter(new FileWriter(basename+"_"+subSamples[index][i]+"_synapseDensity.csv"));
			fout.write(reps[i].getSynapseDensityTable(1,cNames));
			fout.close();
		    }
		}
		else{
		    NDImage ndim = new NDImage(1,1,1,dir+buildFilename(args[1],subSamples[index][0],1));
		    ndim.append(new NDImage(1,1,1,dir+buildFilename(args[1],subSamples[index][0],2)),NDImage.WAVELENGTH);
		    for(int i = 1; i < subSamples[index].length; i++){
			NDImage tmp = new NDImage(1,1,1,dir+buildFilename(args[1],subSamples[index][i],1));
			tmp.append(new NDImage(1,1,1,dir+buildFilename(args[1],subSamples[index][i],2)),NDImage.WAVELENGTH);
			ndim.append(tmp,NDImage.POSITION);
		    }
		    ndim.divide(16);
		    if(args[0].equals("view")){
			ImageAnalyzer iman = new ImageAnalyzer(ndim,"ndim");
			if(args.length > 2) iman.loadReports(outdir+args[1]+"_"+args[2]+".dat");
			else iman.loadReports(outdir+args[1]+".dat");

			for(int i = 0; i < subSamples[index].length; i++){
			    iman.loadMetaMorphRegions(i,mmDir+"regions_"+args[1]+"_vamppsdxy"+subSamples[index][i]+"c2.rgn");
			    //for(int j = 1; j < 4; j++){
			    //for(int w = 1; w < 3; w++) iman.loadMetaMorphTraces(i-1,mmDir+" tr "+j+" "+args[1]+"_vamppsdxy"+i+"c"+w+".rgn",w-1,false);
			    //}
			}
			
		    }
		    else if(args[0].equals("process")){
			ImageAnalysisToolkit.setExecutor(Executors.newFixedThreadPool(5));
			ImageAnalysisToolkit.resetActiveThreads();
			FixedCellAnalyzer iat = new FixedCellAnalyzer(ndim,new ImageReport[subSamples[index].length]);
			iat.setChannelName(0,"VAMP2");
			iat.setChannelName(1,"PSD95");
			iat.setPrePost(0,-1);
			iat.setPrePost(1,1);
			iat.setSaturationThreshold(4094);
			iat.setResolutionXY(0.046);
			iat.setMasterChannel(1);
			iat.setMasterMode(FixedCellAnalyzer.OR);
			int [] chans = {0,1};
			SynapseCollection sc = new SynapseCollection(chans);
			sc.setDescription("VAMP2 colocalized with PSD95");
			iat.addSynapseCollection(sc);
			int[] chans2 = {1,0};
			sc = new SynapseCollection(chans2);
			sc.setDescription("PSD95 colocalized with VAMP2");
			iat.addSynapseCollection(sc);
			iat.batchProcess(5);
			ImageAnalysisToolkit.finishBatchJobs();
			if(args.length > 2) iat.saveImageReports(outdir+args[1]+"_"+args[2]+".dat");
			else iat.saveImageReports(outdir+args[1]+".dat");
		    }
		}
	    }
	    else if(args[0].equals("process")){
		for(int j = 0; j < samples.length; j++){
		    long t1 = System.currentTimeMillis();
		    System.out.println("Starting sample "+samples[j]+": "+subSamples[j].length+" images to process");
		    NDImage ndim = new NDImage(1,1,1,dir+buildFilename(samples[j],subSamples[j][0],1));
		    ndim.append(new NDImage(1,1,1,dir+buildFilename(samples[j],subSamples[j][0],2)),NDImage.WAVELENGTH);
		    for(int i = 1; i < subSamples[j].length; i++){
			NDImage tmp = new NDImage(1,1,1,dir+buildFilename(samples[j],subSamples[j][i],1));
			tmp.append(new NDImage(1,1,1,dir+buildFilename(samples[j],subSamples[j][i],2)),NDImage.WAVELENGTH);
			ndim.append(tmp,NDImage.POSITION);
		    }
		    ndim.divide(16);
		    ImageAnalysisToolkit.setExecutor(Executors.newFixedThreadPool(5));
		    ImageAnalysisToolkit.resetActiveThreads();
		    FixedCellAnalyzer iat = new FixedCellAnalyzer(ndim,new ImageReport[subSamples[j].length]);
		    iat.setChannelName(0,"VAMP2");
		    iat.setChannelName(1,"PSD95");
		    iat.setPrePost(0,-1);
		    iat.setPrePost(1,1);
		    iat.setSaturationThreshold(4094);
		    iat.setResolutionXY(0.046);
		    iat.setMasterChannel(1);
		    iat.setMasterMode(FixedCellAnalyzer.OR);
		    int [] chans = {0,1};
		    SynapseCollection sc = new SynapseCollection(chans);
		    sc.setDescription("VAMP2 colocalized with PSD95");
		    iat.addSynapseCollection(sc);
		    int[] chans2 = {1,0};
		    sc = new SynapseCollection(chans2);
		    sc.setDescription("PSD95 colocalized with VAMP2");
		    iat.addSynapseCollection(sc);
		    iat.batchProcess(5);
		    ImageAnalysisToolkit.finishBatchJobs();
		    if(args.length > 2) iat.saveImageReports(outdir+samples[j]+"_"+args[2]+".dat");
		    else iat.saveImageReports(outdir+samples[j]+".dat");
		    ndim = null;
		    iat = null;
		    Runtime.getRuntime().gc();
		    long t2 = System.currentTimeMillis();
		    long diff = (t2-t1)/1000;
		    System.out.println("Finished sample "+samples[j]+" in "+(diff/60)+" minutes and "+(diff%60)+" seconds.");
		}
	    }
	    else if(args[0].equals("density")){
		String[] cNames = {"VAMP2","PSD95"};
		for(int j = 0; j < samples.length; j++){
		    ImageReport[] reps;
		    BufferedWriter fout;
		    String basename = "";
		    if(args.length > 2){
			reps = ImageAnalysisToolkit.readImageReports(outdir+samples[j]+"_"+args[2]+".dat");
			basename = outdir+samples[j]+"_"+args[2];
		    }
		    else{
			reps = ImageAnalysisToolkit.readImageReports(outdir+samples[j]+".dat");
			basename = outdir+samples[j];
		    }
		    for(int i = 0; i < reps.length; i++){
			reps[i].loadMetaMorphRegions(mmDir+"regions_"+samples[j]+"_vamppsdxy"+subSamples[j][i]+"c2.rgn");
			reps[i].setResolutionXY(0.046);
			fout = new BufferedWriter(new FileWriter(basename+"_"+subSamples[j][i]+"_synapseDensity.csv"));
			fout.write(reps[i].getSynapseDensityTable(1,cNames));
			fout.close();
		    }
		}
	    }
	    else if(args[0].equals("test")){
		boolean passed = true;
		for(int j = 0; j < samples.length; j++){
		    for(int i = 0; i < subSamples[j].length; i++){
			String filename = dir+buildFilename(samples[j],subSamples[j][i],1);
			File phil = new File(filename);
			if(!phil.exists()){
			    System.out.println("ERROR: Could not find file "+filename);
			    passed = false;
			}
			filename = dir+buildFilename(samples[j],subSamples[j][i],2);
			phil = new File(filename);
			if(!phil.exists()){
			    System.out.println("ERROR: Could not find file "+filename);
			    passed = false;
			}
		    }
		}
		if(passed) System.out.println("Test Passed.");
		else System.out.println("Test Failed: One or more samples missing.");
	    }
	    long t_end = System.currentTimeMillis();
	    long t_elapsed = (t_end-t_start)/1000;
	    System.out.println("All operations finished in "+(t_elapsed/60)+" minutes and "+(t_elapsed%60)+" seconds.");
	}
	catch(Exception e){ e.printStackTrace(); }
    }

    public static String buildFilename(String sample, String subSample, int channel)
    {
	String name = sample+"_vamppsdxy"+subSample+"c"+channel+"_cmle2.tif";
	return name;
    }
}
