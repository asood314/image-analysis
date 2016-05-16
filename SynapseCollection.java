import java.util.Vector;

public class SynapseCollection
{
    private int[] channels;
    private boolean requireAllColocalized;
    private Vector<int[]> requiredColocalizations;
    private int overlapThreshold;
    private int distanceThreshold;
    private Vector<Synapse> synapses;

    public SynapseCollection(int[] chan)
    {
	channels = chan;
	requireAllColocalized = true;
	requiredColocalizations = new Vector<int[]>();
	overlapThreshold = 0;
	distanceThreshold = -1;
	synapses = new Vector<Synapse>();
    }

    public void addSynapse(Synapse s){ synapses.addElement(s); }

    public Synapse getSynapse(int index){ return synapses.elementAt(index); }

    public void removeSynapse(int index){ synapses.remove(index); }

    public int getNSynapses(){ return synapses.size(); }

    public void setRequireAll(boolean tf){ requireAllColocalized = tf; }

    public boolean allRequired(){ return requireAllColocalized; }

    public void addRequiredColocalization(int[] chans){ requiredColocalizations.addElement(chans); }

    public int[] getRequiredColocalization(int index){ return requiredColocalizations.elementAt(index); }

    public int getNRequirements(){ return requiredColocalizations.size(); }

    public void setOverlapThreshold(int ot){ overlapThreshold = ot; }

    public int getOverlapThreshold(){ return overlapThreshold; }

    public void setDistanceThreshold(int dt){ distanceThreshold = dt; }

    public int getDistanceThreshold(){ return distanceThreshold; }

    public int getNChannels(){ return channels.length; }

    public int[] getChannels(){ return channels; }

    public int getChannel(int index){ return channels[index]; }

    public int getChannelIndex(int chan)
    {
	for(int i = 0; i < channels.length; i++){
	    if(channels[i] == chan) return i;
	}
	return -1;
    }

    public boolean computeColocalization(Synapse s)
    {
	if(requireAllColocalized){
	    if(overlapThreshold >= 0){
		int overlap = s.getClusterOverlap(channels) - overlapThreshold;
		s.setColocalizationScore((double)overlap);
		if(overlap > 0) return true;
		else return false;
	    }
	    else if(distanceThreshold >= 0){
		double distance = distanceThreshold - s.getMaxClusterDistance(channels);
		s.setColocalizationScore(distance);
		if(distance > 0) return true;
		else return false;
	    }
	    System.out.println("Can't test colocalization: Neither overlap nor distance thresholds set.");
	    return false;
	}
	double score = 1.0;
	if(overlapThreshold >= 0){
	    for(int i = 0; i < requiredColocalizations.size(); i++){
		score *= s.getClusterOverlap(requiredColocalizations.elementAt(i)) - overlapThreshold;
	    }
	}
	else if(distanceThreshold >= 0){
	    for(int i = 0; i < requiredColocalizations.size(); i++){
		score *= distanceThreshold - s.getMaxClusterDistance(requiredColocalizations.elementAt(i));
	    }
	}
	else{
	    System.out.println("Can't test colocalization: Neither overlap nor distance thresholds set.");
	    return false;
	}
	s.setColocalizationScore(score);
	if(score > 0) return true;
	else return false;
    }
}