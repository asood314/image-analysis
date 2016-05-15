import java.util.Vector;

public class SynapseCollection
{
    private int[] channels;
    private boolean requireAllColocalized;
    private boolean[] requirePairColocalized;
    private int overlapThreshold;
    private int distanceThreshold;
    private Vector<Synapse> synapses;

    public SynapseCollection(int[] chan)
    {
	channels = chan;
	requireAllColocalized = true;
	requirePairColocalized = new boolean[channels.length*(channels.length - 1)];
	overlapThreshold = 0;
	distanceThreshold = -1;
	synapses = new Vector<Synapse>();
    }
}