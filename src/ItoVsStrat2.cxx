
#include "../h/neurolab"

using namespace std;

int main(int argc, char **argv)
{
	// timestep
	Time time(0.0001);

	// two neurons
	double v0 = -65, theta = -3.5, vRest = -80,  tau = 20.0 / 1000.0, spikeheight = -5;
	IfNeuron nrnStrat(&time, v0, theta, spikeheight, tau, vRest, "Stratonovich neuron");
	nrnStrat.setParameter("membrane integration-mode", "stratonovich");
	IfNeuron nrnIto(&time, v0, theta, spikeheight, tau, vRest, "Ito neuron");
	nrnIto.setParameter("membrane integration-mode", "ito");
	
	// a stimulus
	Wiener stimulus( &time );
	stimulus.setMean(1.0);
	nrnIto.addStimulus( &stimulus, 0.1, 0.0 );
	nrnStrat.addStimulus( &stimulus, 0.1, 0.0 );
	
	int n = 20;
	
	// testers
	IntervalEstimator estIsiIto(EST_MEAN | EST_VAR, &nrnIto, &time); // measures mean and variance of ISI
	IntervalEstimator estIsiStrat(EST_MEAN | EST_VAR, &nrnStrat, &time); // measures mean and variance of ISI
	ProcessEstimator estIto( EST_SAMPLE, &nrnIto, &time, 1000 );
	ProcessEstimator estStrat( EST_SAMPLE, &nrnStrat, &time, 1000 );
	
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
		
		// set stimulus variance
		double var = 2.0 * pow(10.0, double(i)/double(n)*6.0-3.0); // 50 steps, from -3 to 3
		stimulus.setVariance( var );
	
		// calibrate neuron to spike at 50 Hz
		nrnIto.calibrate(1.0/50/time.dt, 50, 10*50/time.dt);
		nrnStrat.calibrate(1.0/50/time.dt, 50, 10*50/time.dt);
		
		// run for 3000 spikes
		time.run( 3000, &nrnIto, 300000);
		
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
	dsp0 << mMeanIsiIto.setName("Itô")
		<< mMeanIsiStrat.setName("Stratonovich")
		<< showdsp;
	
	// display the variance curves
	Display dsp1("data/ItoVsStratVar2");
	dsp1.setMode(DSP_AXIS_LOG);
	dsp1 << mVarIsiIto.setName("Itô")
		<< mVarIsiStrat.setName("Stratonovich")
		<< showdsp;
	
	// another display for the voltage traces
	Display dsp2("data/ItoVsStratAngle2");
	dsp2 << estIto.mResult(EST_SAMPLE).setName("Itô neuron membrane")
		<< estStrat.mResult(EST_SAMPLE).setName("Stratonovich neuron membrane")
		<< showdsp;
	
	return 0;
}

