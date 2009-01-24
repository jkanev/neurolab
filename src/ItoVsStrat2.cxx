
#include "../h/neurolab"

using namespace std;

int main(int argc, char **argv)
{
	// timestep
	Time time(0.1);
	time.setUnit(Unit("m","s")); // in ms

	// two neurons
	ThetaNeuron nrnIto(  &time );
	nrnIto.setIto();
	ThetaNeuron nrnStrat(  &time );
	nrnStrat.setStratonovich();
	
	// a stimulus
	Wiener stimulus( &time );
	stimulus.setParameter( WIENER_MEAN, 0.0 );
	stimulus.setPassive(); // A passive stimulus doesn't proceed in time by itself, but each time step proceedToNextState() and prepareNextState() have to be called. By calling proceedToNextState() and prepareNextState() manually we ensure that both neurons receive *exactly* the same stimulus. Otherwise the stimulus would generate the next stochastic value by itself each time it is accessed by the neuron.
	nrnIto.addStimulus( &stimulus );
	nrnStrat.addStimulus( &stimulus );
	
	// calibrate the neuron to spike with an isi of 50ms
	// cout << "calibrating the neuron. this may take a while..." << endl;
	// nrnIto.calibrate(int(50/xTime->dt), 100, 20000);

	int n = 20;
	
	// testers
	IntervalEstimator estIsiIto(&time, EST_MEAN | EST_VAR, &nrnIto); // measures mean and variance of ISI
	IntervalEstimator estIsiStrat(&time, EST_MEAN | EST_VAR, &nrnStrat); // measures mean and variance of ISI
	ProcessEstimator estIto( EST_SAMPLE, &nrnIto, 1000 );
	ProcessEstimator estStrat( EST_SAMPLE, &nrnStrat, 1000 );
	
	// four matrixes for recording isi mean and variance
	Matrix mMeanIsiIto(n,2); // 2x20 matrix for the mean isi over var, ito version
	mMeanIsiIto.setPhysical( Physical("isi mean", "", "") );
	mMeanIsiIto.setPhysical( 0, Physical("stim. variance","","") );
	mMeanIsiIto.setPhysical( 1, Physical("isi mean","","") );
	
	Matrix mMeanIsiStrat(n,2); // 2x20 matrix for the variance of isi over var, ito version
	mMeanIsiStrat.setPhysical( Physical("isi mean", "", "") );
	mMeanIsiStrat.setPhysical( 0, Physical("stim. variance", "","") );
	mMeanIsiStrat.setPhysical( 1, Physical("isi mean","","") );
	
	Matrix mVarIsiIto(n,2); // 2x20 matrix for the mean isi over var, strat version
	mVarIsiIto.setPhysical( Physical("isi variance", "", "") );
	mVarIsiIto.setPhysical( 0, Physical("cond. variance","","") );
	mVarIsiIto.setPhysical( 1, Physical("isi variance","","") );
	
	Matrix mVarIsiStrat(n,2); // 2x20 matrix for the mean isi over var, strat version
	mVarIsiStrat.setPhysical( Physical("isi variance", "", "") );
	mVarIsiStrat.setPhysical( 0, Physical("cond. variance","","") );
	mVarIsiStrat.setPhysical( 1, Physical("isi variance","","") );
	
	// simulate neuron
	cout << "starting simulation"  << endl;
	for(int i=0; i<n; i++) {
		
		// reset all estimators before each run
		estIsiIto.init();
		estIsiStrat.init();
		
		// set stimulus variance
		double var = 2.0 * pow(10.0, double(i)/double(n)*6.0-3.0); // 50 steps, from -3 to 3
		stimulus.setParameter( WIENER_VARIANCE, var );
		
		// run for 3000 spikes
		for( int timeout=0, spikes=0; 
		spikes<50 && timeout<30000; 
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
			if(i==n/2) {
				estIto.proceedToNextState();
				estStrat.proceedToNextState();
			}
			
			// reset timeout
			if(nrnIto.hasEvent()) {
				timeout = 0;
				cout << "\r\t run/spikes:  " << i << "/" << spikes << " of 50/3000     \t" << flush;
			}
		}
		
		// save measurements into matrixes
		mMeanIsiIto[i][0] = var/2.0;
		mMeanIsiIto[i][1] = 1.0 / estIsiIto.mResult(EST_MEAN).to_d();
		mMeanIsiStrat[i][0] = var/2.0;
		mMeanIsiStrat[i][1] = 1.0 / estIsiStrat.mResult(EST_MEAN).to_d();
		mVarIsiIto[i][0] = var/2.0;
		mVarIsiIto[i][1] = estIsiIto.mResult(EST_VAR).to_d() / estIsiIto.mResult(EST_MEAN).to_d();
		mVarIsiStrat[i][0] = var/2.0;
		mVarIsiStrat[i][1] = estIsiStrat.mResult(EST_VAR).to_d() / estIsiStrat.mResult(EST_MEAN).to_d();
	}
	cout << endl;

	// a display for the mean
	Display dsp0("data/ItoVsStratMean2"); // the string "ItoVsStrat" is used as base for the data files. Plots can be viewed again after this program has finished by calling "gnuplot ItoVsStrat.gnuplot". The data will reside in files called "ItoVsStrat.X.data", where X is a number.
	dsp0.setMode(DSP_AXIS_LOG);
	dsp0 << mMeanIsiIto;
	dsp0 << mMeanIsiStrat;
	// dsp0.save();
	dsp0.show();
	
	// display the variance curves
	Display dsp1("data/ItoVsStratVar2");
	dsp1.setMode(DSP_AXIS_LOG);
	dsp1 << mVarIsiIto;
	dsp1 << mVarIsiStrat;
	// dsp1.save();
	dsp1.show();
	
	// another display for the voltage traces
	Display dsp2("data/ItoVsStratAngle2");
	dsp2 << estIto.mResult(EST_SAMPLE);
	dsp2 << estStrat.mResult(EST_SAMPLE);
	dsp2.show();
	dsp2.save();
	
	return 0;
}

