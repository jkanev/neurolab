#include "../h/neurolab"

using namespace std;

class Simulation : public DataCollector
{
public:
	
	// Setup matrixes
	Simulation() : itoMean(50,2), stratoMean(50,2), itoVar(50,2), stratoVar(50,2)
	{
		itoMean.setPhysical( Physical("isi mean", "", "") );
		itoMean.setPhysical( 0, Physical("stim. variance", Unit("m","S")*Unit("m","S")) );
		itoMean.setPhysical( 1, Physical("isi mean","","") );
		
		stratoMean.setPhysical( Physical("isi mean", "", "") );
		stratoMean.setPhysical( 0, Physical("stim. variance", Unit("m","S")*Unit("m","S")) );
		stratoMean.setPhysical( 1, Physical("isi mean","","") );
	
		itoVar.setPhysical( Physical("isi variance", "", "") );
		itoVar.setPhysical( 0, Physical("cond. variance",Unit("m","S")*Unit("m","S")) );
		itoVar.setPhysical( 1, Physical("isi variance","","") );
	
		stratoVar.setPhysical( Physical("isi variance", "", "") );
		stratoVar.setPhysical( 0, Physical("cond. variance",Unit("m","S")*Unit("m","S")) );
		stratoVar.setPhysical( 1, Physical("isi variance","","") );
	}
		
	// Set stimulus variance
	virtual void beforeRun(const vector<unsigned long long>& step)
	{
		double var = 2.0 * pow(10.0, double(step[0])/double(50)*3.0); // 50 steps, from 0 to 3
		stimulus->setVariance(  var );	
	};
	
	// Save measurements into matrixes
	virtual void afterRun(const vector<unsigned long long>& step)
	{
		unsigned long long n = step[0];
		double var = 2.0 * pow(10.0, double(step[0])/double(50)*3.0); // 50 steps, from 0 to 3
		itoMean[n][0] = var;
		itoMean[n][1] = 1000 / estIto->mResult(EST_MEAN).to_d();
		stratoMean[n][0] = var;
		stratoMean[n][1] = 1000 / estStrat->mResult(EST_MEAN).to_d();
		itoVar[n][0] = var;
		itoVar[n][1] = 1000 / estIto->mResult(EST_VAR).to_d();
		stratoVar[n][0] = var;
		stratoVar[n][1] = 1000 / estStrat->mResult(EST_VAR).to_d();
	};
		
	// Matrices to record data
	Matrix itoMean;
	Matrix stratoMean;
	Matrix itoVar;
	Matrix stratoVar;
	
	// Stimulus
	Wiener *stimulus;
	
	// Estimators
	IntervalEstimator *estIto;
	IntervalEstimator *estStrat;
};
	
int main(int argc, char **argv)
{
	// timestep
	double dt = 0.01; // in s
	Time time(dt);
	
	// two neurons
	double v0 = -60, vRest = -70,  tau = 1, threshold = -45, spikeheight = 10;
	IfNeuron nrnIto(  &time, v0, threshold, spikeheight, tau, vRest, "Ito Neuron" );
	nrnIto.setParameter("membrane integration-mode", "ito");
	IfNeuron nrnStrat(  &time, v0, threshold, spikeheight, tau, vRest, "Stratonovich Neuron" );
	nrnStrat.setParameter("membrane integration-mode", "stratonovitch");
	
	// a conductance
	double revpot = 10, weight = 0.01, mean = 3.0;
	Wiener stimulus( &time );
	stimulus.setUnit(Unit("m","S") * Unit("m","s"));
	stimulus.setMean( mean );
	stimulus.setVariance( 2.0 );
	nrnIto.addStimulus( &stimulus, weight, revpot );
	nrnStrat.addStimulus( &stimulus, weight, revpot );
	
	// calibrate the neuron to spike with an isi of 5 Hz
	nrnIto.calibrate(int(1.0/5.0/time.dt), 1000, 100000);
	nrnStrat.calibrate(int(1.0/5.0/time.dt), 1000, 100000);

	// testers
	IntervalEstimator estIsiIto(EST_MEAN | EST_VAR, &nrnIto, &time); // measures mean and variance of ISI
	IntervalEstimator estIsiStrat(EST_MEAN | EST_VAR, &nrnStrat, &time); // measures mean and variance of ISI
	ProcessEstimator estIto(EST_SAMPLE, &nrnIto, &time, 300 );
	ProcessEstimator estStrat(EST_SAMPLE, &nrnStrat, &time, 300 );
	
	// matrixes
	Simulation dataCollector;
	dataCollector.estIto = &estIsiIto;
	dataCollector.estStrat = &estIsiStrat;
	dataCollector.stimulus = &stimulus;
	
	// run simulations
	time.runNested(10000, &nrnIto, 1e12, 50,&dataCollector);

	// a display for our results
	Display dsp("data/ItoVsStratMean"); // the string "ItoVsStrat" is used as base for the data files. Plots can be viewed again after this program has finished by calling "gnuplot ItoVsStrat.gnuplot". The data will reside in files called "ItoVsStrat.X.data", where X is a number.
	
	// display the mean curves
	dsp << setmode( DSP_TOP )
		<< dataCollector.itoMean.setName("response mean of neuron using Ito")
		<< dataCollector.stratoMean.setName("response mean of neuron using Stratonovich");
	
	// display the variance curves
	dsp << setmode( DSP_BOTTOM )
		<< dataCollector.itoVar.setName("response variance of neuron using Ito")
		<< addsetting("logscale", "y")
		<< dataCollector.stratoVar.setName("response variance of neuron using Stratonovich")
		<< savedsp;
		
	// another display for the voltage traces
	Display dsp2("data/ItoVsStratVoltage");
	dsp2 << estIto.mResult(EST_SAMPLE).setName("voltage sample of Ito neuron")
		<< estStrat.mResult(EST_SAMPLE).setName("voltage sample of Stratonovich neuron")
		<< savedsp;
	
	return 0;
}

