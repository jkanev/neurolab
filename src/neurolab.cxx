
#include "../h/processes.hxx"
#include "../h/matrix.hxx"
#include "../h/estimator.hxx"
#include "../h/scalarestimator.hxx"
#include "../h/intervalestimator.hxx"
#include "../h/conditionalestimator.hxx"
#include "../h/processestimator.hxx"
#include "../h/display.hxx"
#include "../h/ifneuron.hxx"
#include "../h/wiener.hxx"

using namespace std;

int main(int argc, char **argv)
{
	// timestep
	double dt = 0.1; // in ms

	// two neurons
	double v0 = -60, vRest = -70,  tau = 5.0, threshold = -45, spikeheight = -20;
	IfNeuron nrnIto(  dt, v0, threshold, spikeheight, tau, vRest );
	nrnIto.setIto();
	IfNeuron nrnStrat(  dt, v0, threshold, spikeheight, tau, vRest );
	nrnStrat.setStratonovich();
	
	// a conductance
	double revpot = 10, weight = 0.01, mean = 7.5;
	Wiener stimulus( dt );
	stimulus.setProperty( WIENER_MEAN, mean );
	stimulus.setPassive(); // A passive stimulus doesn't proceed in time by itself, but each time step proceedToNextState() and prepareNextState() have to be called. By calling proceedToNextState() and prepareNextState() manually we ensure that both neurons receive *exactly* the same stimulus. Otherwise the stimulus would generate the next stochastic value by itself each time it is accessed by the neuron.
	nrnIto.addStimulus( &stimulus, weight, revpot );
	nrnStrat.addStimulus( &stimulus, weight, revpot );
	
	// calibrate the neuron to spike with an isi of 50ms
	// cout << "calibrating the neuron. this may take a while..." << endl;
	// nrnIto.calibrate(int(50/dt), 100, 20000);

	// testers
	IntervalEstimator estIsiIto(dt, EST_MEAN | EST_VAR, &nrnIto); // measures mean and variance of ISI
	IntervalEstimator estIsiStrat(dt, EST_MEAN | EST_VAR, &nrnStrat); // measures mean and variance of ISI
	ProcessEstimator estIto( dt, EST_SAMPLE, &nrnIto, 1000 );
	ProcessEstimator estStrat( dt, EST_SAMPLE, &nrnStrat, 1000 );
	
	// four matrixes for recording isi mean and variance
	Matrix mMeanIsiIto(40,2); // 2x20 matrix for the mean isi over var, ito version
	Matrix mMeanIsiStrat(40,2); // 2x20 matrix for the variance of isi over var, ito version
	Matrix mVarIsiIto(40,2); // 2x20 matrix for the mean isi over var, strat version
	Matrix mVarIsiStrat(40,2); // 2x20 matrix for the mean isi over var, strat version
	
	// simulate neuron
	for(int i=0; i<40; i++) {
		
		// reset all estimators before each run
		estIsiIto.init();
		estIsiStrat.init();
		
		// set stimulus variance
		double var = double(i+0.01)*50;
		stimulus.setProperty( WIENER_VARIANCE, var );
		
		// run for 3000 spikes
		for( int timeout=0, spikes=0; 
		spikes<3000 && timeout<30000; 
		timeout++, spikes+=nrnIto.hasEvent()) {
			
			// Proceed the stimulus. This is only necessary with passive StochVars.
			stimulus.proceedToNextState();
			stimulus.prepareNextState();
			
			// Proceed the neurons.
			nrnIto.proceedToNextState(); // calculate next values for ito neuron
			nrnStrat.proceedToNextState(); // calculate next values for strat neuron
			nrnIto.prepareNextState(); // proceed ito neuron
			nrnStrat.prepareNextState(); // proceed strat neuron
			
			// proceed the estimators
			estIsiIto.proceedToNextState(); // measure mean and var of ito neuron
			estIsiStrat.proceedToNextState(); // measure mean and var of strat neuron
			
			// record sample trace
			if(i==2) {
				estIto.proceedToNextState();
				estStrat.proceedToNextState();
			}
			
			// reset timeout
			if(nrnIto.hasEvent()) {
				timeout = 0;
				cout << "\r\t run/spikes:  " << i << "/" << spikes << " of 40/3000     \t" << flush;
			}
		}
		
		// save measurements into matrixes
		mMeanIsiIto[i][0] = var;
		mMeanIsiIto[i][1] = 1000 / estIsiIto.mResult(EST_MEAN).to_d();
		mMeanIsiStrat[i][0] = var;
		mMeanIsiStrat[i][1] = 1000 / estIsiStrat.mResult(EST_MEAN).to_d();
		mVarIsiIto[i][0] = var;
		mVarIsiIto[i][1] = 1000 / estIsiIto.mResult(EST_VAR).to_d();
		mVarIsiStrat[i][0] = var;
		mVarIsiStrat[i][1] = 1000 / estIsiStrat.mResult(EST_VAR).to_d();
	}
	cout << endl;

	// a display for our results
	Display dsp("data/ItoVsStratMean"); // the string "ItoVsStrat" is used as base for the data files. Plots can be viewed again after this program has finished by calling "gnuplot ItoVsStrat.gnuplot". The data will reside in files called "ItoVsStrat.X.data", where X is a number.
	
	// display the main curves
	dsp << mMeanIsiIto.setName("response mean of neuron using Ito");
	dsp << mMeanIsiStrat.setName("response mean of neuron using Stratonovich");
	
	// display the variance curves in the top left corner
	dsp.setMode( DSP_TOP_LEFT );
	dsp << mVarIsiIto.setName("Var. - Ito");
	dsp << mVarIsiStrat.setName("Var. - Strat.");
	
	// display
	dsp.show();
	dsp.save();
	
	// another display for the voltage traces
	Display dsp2("data/ItoVsStratVoltage");
	dsp2 << estIto.mResult(EST_SAMPLE).setName("voltage sample of Ito neuron");
	dsp2 << estStrat.mResult(EST_SAMPLE).setName("voltage sample of Stratonovich neuron");
	dsp2.show();
	dsp2.save();
	
	return 0;
}

