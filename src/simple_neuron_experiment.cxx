
#include "../h/neurolab"

int main( int argc, char **argv )
{
	// time settings
	double dt = 0.1; // time step
	Time t(dt); // central time object
	
	// construct a neuron with exc. white noise
	double v0 = -60.0, theta = -54.0, spike = -20.0, tau = 5.0, v_rest = -70.0;
	IfNeuron nrn(&t, v0, theta, spike, tau, v_rest);
	double excMu = 0.1, excSigma = 0.1; excW = 2.0, excVRev = -20.0;
	Wiener excNoise(&t, excMu, excSigma);
	nrn.addStimulus(&excNoise);
	
	// run simulation:
	int points = 10;
	int spikes = 500;
	
	// 1. influence of mean
	excMu = 0.1;
	excSigma = 0.1;
	for (int cPoint=0; cPoint<points; ++cPoint) {
		excSigma += 0.1;
		excNoise.setSigma( excSigma );
		nrn.calibrate(500, 500, 5000);
		for (int cSpike=0; cSpike<spikes;) {
			nrn.proceedToNextState();
			nrn.prepareNextState();
			if (nrn.hasEvent())
				++cSpike;
		}
	}
	// 2. influence of variance
	
	// 3. influence of weight
	
	// 4. influence of rate
	
	return 0;
}
