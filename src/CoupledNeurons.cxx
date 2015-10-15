
#include "../h/neurolab"

using namespace std;

int main(int argc, char **argv)
{
	// create a time object, using a time step of 1 ms
	double dt = 0.1; // in ms
	Time time(dt);
	time.setUnit(Unit("m","s")); // set unit for display to ms
	
	// create two neurons
	double v0 = -60, vRest = -70,  tau = 5.0, threshold = -10, spikeheight = 20;
	IfNeuron nrnOne( &time, v0, threshold, spikeheight, tau, vRest, "neuron-1" );
	IfNeuron nrnTwo( &time, v0, threshold, spikeheight, tau, vRest, "neuron-2" );
	
	// create two noise sources with mean = variance = 0.5
	double mean = 0.5, var = 0.5;
	Wiener noiseOne( &time, "stimulus-1" );
	Wiener noiseTwo( &time, "stimulus-2" );
	noiseOne.setMean( mean );
	noiseTwo.setMean( mean );
	noiseOne.setVariance( var );
	noiseTwo.setVariance( var );

	// declare them as conductances (correct unit and description, will show in plot)
	noiseOne.setUnit(Unit("m","S") * time.getUnit());
	noiseTwo.setUnit(Unit("m","S") * time.getUnit());
	noiseOne.setPhysicalDescription("conductance density");
	noiseTwo.setPhysicalDescription("conductance density");
	
	// and add as syncaptic conductance to each neuron, with reversal potential 10 mV
	double revpot = 10, weight = 0.1;
	nrnOne.addStimulus( &noiseOne, weight, revpot );
	nrnTwo.addStimulus( &noiseTwo, weight, revpot );
	
	// calibrate both neurons to spike at 5Hz
	nrnOne.calibrate(200, 40, 500);
	nrnTwo.calibrate(200, 40, 500);
	
	// create two excitatory synapses from each neuron
	revpot = -80.0, weight = 0.1;
	Synapse one2Two(&time, &nrnOne, weight, revpot, "excitatory-1");
	Synapse two2One(&time, &nrnTwo, weight, revpot, "excitatory-2");
	
	// add each as simulus to the other neuron respectively
	nrnOne.addStimulus( &two2One );
	nrnTwo.addStimulus( &one2Two );	
	
	// create two extimators for mean and sample
	ProcessEstimator estNrn1(EST_SAMPLE, &nrnOne, &time, 2000);
	ProcessEstimator estNrn2(EST_SAMPLE, &nrnTwo, &time, 2000);
	ConditionalEstimator estNoise1(EST_DIFF|EST_MEAN, &noiseOne, &nrnOne, &time, 100, 100);
	ConditionalEstimator estNoise2(EST_DIFF|EST_MEAN, &noiseTwo, &nrnOne, &time, 100, 100);
	
	// run the simulation
	time.run( 1e6 );
	
	// create a display
	Display dspNrn("data/CoupledNeuronsSample");
	
	Function t = input("t");
	Function v1 = 2.0 * exp(t) * (t < cnst(0.0)) + 5.0;
	Function v2 = 2.0 * exp(0.5*t) * (t < cnst(0.0)) + 5.0;
	
	// send the recorded sample of the first neuron to the display, in a graph at the top
	dspNrn.setMode( DSP_TOP );
	dspNrn << estNrn1.getEstimate(EST_SAMPLE);
	
	// send the recorded sample of the second to the display, in a separate graph at the bottom
	dspNrn.setMode( DSP_BOTTOM );
	dspNrn << estNrn2.getEstimate(EST_SAMPLE);
	
	// show to user, then save as png
	dspNrn << showdsp
		<< savepng;
	
	// create other display
	Display dspNoise("data/CoupledNeuronsRsc");
	
	// send the recorded conditional mean to the display
	dspNoise.setMode( DSP_TOP );
	dspNoise << estNoise1.getEstimate(EST_MEAN)
		<< v1
		<< v2;
		
	// send the recorded conditional mean to the display
	dspNoise.setMode( DSP_BOTTOM );
	dspNoise << estNoise2.getEstimate(EST_MEAN)
		<< (v1 - v2)/5.0 + 5.0;
	
	// show to user
	dspNoise << showdsp << savepng;

	// show configuration of neurons to user
	cout << noiseOne.getConfiguration() << endl << endl
		<< noiseTwo.getConfiguration() << endl << endl
		<< nrnOne.getConfiguration() << endl << endl
		<< nrnTwo.getConfiguration() << endl << endl;
};
