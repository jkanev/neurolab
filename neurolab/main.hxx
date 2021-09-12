/**
\mainpage
\section Introduction
NeuroLab is a class library which offers simulation of stochastic differential (integral) equations, as well as recording and display of data. Its main use is intended to be in the neuroscience field. Please find the source code on GitHub, <a href="https://github.com/jkanev/neurolab">github.com/jkanev/neurolab</a>. Intended audience are researchers who want to run simulations of single or multiple connected neurons, analyse the results, and display / save to various file format.

\section General Principles

Stochastic Variables (StochasticVariable), such as stochastic differential equations (DifferentialEquation), produce data. This data can be recorded (Matrix) by various estimators, which look for mean, variance and/or higher moments. Using a display (Display) the data can then be  displayed, compared to analytical functions (Function), and saved as data (gnuplot format), or postscript file, ready to be included in your paper.

Stochastic differential equations can be interpreted in the Ito or in the Stratonovich sense. The DifferentialEquation class lets you switch between these two versions, even at runtime. 

Data can be recorded conditionally. This means the mean (or any other moment) of a process as function of the time distance to an observed event can be taken. This is done by a ConditionalEstimator, which records the time course of a stochastic variable (StochasticVariable) before and after an observed event (Event).

\section Neural Simulations

A leaky integrate-and-fire neuron is implemented (IfNeuron), which can be calibrated automatically to give a certain response rate (IfNeuron::calibrate()). A neuron is also a SDE, so you may set up small neural networks, having a system of inter-dependent SDEs, where mutual dependencies are allowed as well. Since a neuron produces events (spikes), you can also do spike-triggered averaging.

A neuron may receive its stimulus via three different ways: 

An independent stochastic process. These are objects of the classes Poisson, Wiener, WienerCpp. They can be linked to a neuron with the IfNeuron::addStimulus() method. Each such object may drive one or many neurons.

A dependent stochastic process. A Noises object produces NoiseSource objects which resemble poisson and wiener processes, and which may be correlated. It is even possible to correlate a wiener and a poisson process.

Another neuron. In this case a synapse has to be built, which connects the neurons. Possible classes are Synapse (a simple synapse which transforms the spike events of the pre-synaptic neuron into Zeros and Ones [multiplied by a weight factor]), or SimpleSynapse (a more complex synapse, which is driven by a differential equation).

\section Example

Lets look at a simple example. You want to create two conductance-based leaky integrate-and-fire neurons (called nrnOne and nrnTwo), each inhibiting the other one, and both receiving some excitatory noise (noiseOne and noiseTwo). You then want to record the spike triggered average stimulus of one neuron (you expect some positive exponential spike), and the same average stimulus, but triggered on the spikes of the inhibited neuron (you expect some broader curve, pointing downwards).

You also want both neurons to be spiking with a rate of 5 Hz and want to calibrate that automatically, by adjusting the threshold.

Here's what you do:

@code
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
	
	// create two inhibitory (reversal potential -80 mV) synapses from each neuron
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

	// create two functions v1 and v2 with exponentials for comparison
	Function t = input("t");
	Function v1 = 2.0 * exp(t) * (t < cnst(0.0)) + 5.0;
	Function v2 = 2.0 * exp(0.5*t) * (t < cnst(0.0)) + 5.0;
	
	// send the recorded conditional mean and both theoretical functions to the display
	dspNoise.setMode( DSP_TOP );
	dspNoise << estNoise1.getEstimate(EST_MEAN)
		<< v1
		<< v2;
	
	// send the recorded conditional mean to the display, and the scaled difference of both theoretical functions
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
@endcode


The software runs, and you see following output:

@code
starting simulation: 1000000 steps.         
running simulation: 10%
@endcode

and then

@code
starting simulation: 1000000 steps.         
running simulation: 99%
multiplying by dt: 0.1
displaying function: done.         
displaying function: done.         
multiplying by dt: 0.1
displaying function: done.         
@endcode

Now the following two pictures are shown as dialog windows, and also saved as png to the location "data/CoupledNeuronsRsc.png" and "data/CoupledNeuronsSample.png", as you've defined in the argument to the Display class:

<div style="width: 100%; position: left; float: left">
<b> Step-size=0.01:</b><br>
<img style="position: left; float: left" src="CoupledNeuronsSample.png" width="640px">
<img style="position: left; float: left" src="CoupledNeuronsRsc.png" width="640px"><br>
</div>

At last the configuration is outputted to the terminal, as result of the calls "cout << noiseOne.getConfiguration()...". The complete configuration of the objects is shown, including the thresholds found in the automatic calibration.

@code
"stimulus-1" Wiener Process
	mean: 5
	variance: 0.5


"stimulus-2" Wiener Process
	mean: 5
	variance: 0.5


"neuron-1" Integrate-and-Fire Neuron
	threshold: -10.2635
	spike-height: 20
	membrane: "V" Differential Equation
		equation: d V = f1(V) d X1  + f2(V) d X2  + f3(V) d X3 
		mode: ito
		starting-value: -60
		f1(V): "leak" Weighted Difference
			reversal-potential: -70
			weight: 0.2
		X1: "t" Time Process
		f2(V): "channel" Weighted Difference
			reversal-potential: 10
			weight: 0.1
		X2: "stimulus-1" Wiener Process
		f3(V): "excitatory-2" Synapse
			reversal-potential: -80
			weight: 0.1
			pre-synaptic-neuron: "neuron-2" Integrate-and-Fire Neuron
		X3: "t" Time Process
	
	

"neuron-2" Integrate-and-Fire Neuron
	threshold: -10.2645
	spike-height: 20
	membrane: "V" Differential Equation
		equation: d V = f1(V) d X1  + f2(V) d X2  + f3(V) d X3 
		mode: ito
		starting-value: -60
		f1(V): "leak" Weighted Difference
			reversal-potential: -70
			weight: 0.2
		X1: "t" Time Process
		f2(V): "channel" Weighted Difference
			reversal-potential: 10
			weight: 0.1
		X2: "stimulus-2" Wiener Process
		f3(V): "excitatory-1" Synapse
			reversal-potential: -80
			weight: 0.1
		pre-synaptic-neuron: "neuron-1" Integrate-and-Fire Neuron
		X3: "t" Time Process
		

@endcode

\section Quality

<div style="position: left; float: left">For a short quality check the equations \f[dX_t = aX_tdt + bX_tdW_t\f] (Ito) and \f[dX_t = aX_tdt + bX_t\circ dW_t\f] (Stratonovich) with \f$a=-8.0\f$ and \f$b=0.8\f$ are simulated using the DifferentialEquation class. This equation has an explicit solution, which was plotted on top of the simulated version at different step sizes, relative error in percent shown next to it:</div>

<div style="width: 100%; position: left; float: left">
	<b>Step size = 0.1:</b><br>
	<img style="position: left; float: left" src="TestStratTwo_0.1.png" width="640px">
	<img style="position: left; float: left" src="TestStratThree_0.1.png" width="640px"><br>
	Ito and Stratonovich equations plotted on top of their explicit solutions, and relative error in percent, at a time step of 0.1ms.
</div>

<div style="width: 100%; position: left; float: left">
	<b> Step-size=0.01:</b><br>
	<img style="position: left; float: left" src="TestStratTwo_0.01.png" width="640px">
	<img style="position: left; float: left" src="TestStratThree_0.01.png" width="640px"><br>
	Ito and Stratonovich equations plotted on top of their explicit solutions, and relative error in percent, at a time step of 0.01ms.
</div>

<div style="width: 100%; position: left; float: left">
	<b> Step-size=0.001:</b><br>
	<img style="position: left; float: left" src="TestStratTwo_0.001.png" width="640px">
	<img style="position: left; float: left" src="TestStratThree_0.001.png" width="640px"><br>
	Ito and Stratonovich equations plotted on top of their explicit solutions, and relative error in percent, at a time step of 0.001ms.
</div>

<div style="width: 100%; position: left; float: left">
	<b> Step-size=0.0001:</b><br>
	<img style="position: left; float: left" src="TestStratTwo_0.0001.png" width="640px">
	<img style="position: left; float: left" src="TestStratThree_0.001.png" width="640px"><br>
	Ito and Stratonovich equations plotted on top of their explicit solutions, and relative error in percent, at a time step of 0.0001ms.
</div>
*/
