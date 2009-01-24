
#include "../h/neurolab"

using namespace std;

int main(int argc, char **argv)
{
	// timestep
	double dt = 0.1; // in ms
	Time time(dt);
	
	// create two neurons
	double v0 = -60, vRest = -70,  tau = 5.0, threshold = -10, spikeheight = 20;
	IfNeuron nrnOne( &time, v0, threshold, spikeheight, tau, vRest, "first" );
	IfNeuron nrnTwo( &time, v0, threshold, spikeheight, tau, vRest, "second" );
	
	// create two noise sources and add to each neuron
	double revpot = 10, weight = 0.1, mean = 0.5;
	Wiener noiseOne( &time, "noise_1" );
	Wiener noiseTwo( &time, "noise_2" );
	noiseOne.setUnit(Unit("m","S") * time.getUnit());
	noiseTwo.setUnit(Unit("m","S") * time.getUnit());
	noiseOne.setMean( mean );
	noiseTwo.setMean( mean );
	noiseOne.setVariance( mean );
	noiseTwo.setVariance( mean );
	nrnOne.addStimulus( &noiseOne, weight, revpot );
	nrnTwo.addStimulus( &noiseTwo, weight, revpot );
	
	// calibrate both neurons to spike at 5Hz
	nrnOne.calibrate(200, 40, 500);
	nrnTwo.calibrate(200, 40, 500);
	
	// create two excitatory synapses from each neuron
	revpot = -80.0;
	weight = 0.1;
	Synapse one2Two(&time, &nrnOne, weight, revpot, "excitatory_1");
	Synapse two2One(&time, &nrnTwo, weight, revpot, "excitatory_2");
	
	// add a synapses as simulus to the other neuron respectively
	nrnOne.addStimulus( &two2One );
	nrnTwo.addStimulus( &one2Two );	
	
	// create two testers
	ProcessEstimator estNrn1(EST_SAMPLE, &nrnOne, &time, 2000);
	ProcessEstimator estNrn2(EST_SAMPLE, &nrnTwo, &time, 2000);
	ConditionalEstimator estNoise1(EST_DIFF|EST_MEAN, &noiseOne, &nrnOne, &time, 200, 200);
	ConditionalEstimator estNoise2(EST_DIFF|EST_MEAN, &noiseTwo, &nrnOne, &time, 200, 200);
	
	// run the simulation
	time.run( 2000000 );
	
	// create a display
	Display dspNrn("data/CoupledNeuronsSample");
	
	// send the recorded sample of nrnOne to the display
	dspNrn.setMode( DSP_TOP );
	dspNrn << estNrn1.mResult(EST_SAMPLE);
	
	// send the recorded sample of nrnTwo to the display
	dspNrn.setMode( DSP_BOTTOM );
	dspNrn << estNrn2.mResult(EST_SAMPLE);
	
	// show to user
	dspNrn.show();
	
	// create other display
	Display dspNoise("data/CoupledNeuronsRsc");
	
	// send the recorded conditional mean to the display
	dspNoise.setMode( DSP_TOP );
	dspNoise << estNoise1.mResult(EST_MEAN);
	
	// send the recorded conditional mean to the display
	dspNoise.setMode( DSP_BOTTOM );
	dspNoise << estNoise2.mResult(EST_MEAN);
	
	// show to user
	dspNoise.show();

	// show configuration of neurons to user
	cout
		<< noiseOne.getConfiguration() << endl << endl
		<< noiseTwo.getConfiguration() << endl << endl
		<< nrnOne.getConfiguration() << endl << endl
		<< nrnTwo.getConfiguration() << endl << endl;
};
