#include "../h/neurolab"

using namespace std;

// physical values
const double ms = 0.001;
const double s = 1.0;
const double Hz = 1.0;
const double mV = 1.0;

/// Main function.
int main(int argc, char *argv[])
{
	// generate condensed Poisson process
	double dt = 0.05*ms;
	Time time(dt);
	double w = 0.005;
	double n = 80;
	double lambda = 5*Hz;
	double lambda_rc = 0.5;
	double n_rc = 0.1;
	WienerCpp cpp( &time, w, n, lambda, n_rc, lambda_rc );
	cpp.setActive();
		
	// build neuron
	double v0 = -65;
	double theta = -54;
	double tau = 1.0 / 5.0;
	double vRest = -70;
	IfNeuron nrn = IfNeuron(&time, v0, theta, tau, vRest);
	nrn.setPassive();
	nrn.addStimulus(&cpp);
	nrn.calibrate(50, 100);
	cout << '.' << flush;
	cout << '.' << flush;

	Display meanDsp("mean");
	meanDsp.setMode( DSP_3D_COLOR );
	Display varianceDsp("variance");	
	varianceDsp.setMode( DSP_3D_COLOR );
	
	IntervalEstimator isi(EST_MEAN | EST_VAR, &nrn);
	isi.setProperty( EST_DIST_MIN, 0 );
	isi.setProperty( EST_DIST_MAX, 1000 );
	int bins = 200;
	isi.setProperty( EST_DIST_BINS, bins );
	
	Matrix mean(50,50);
	Matrix variance(50,50);
	
	for(int a=0; a<50; a++) {
		for(int b=0; b<50; b++) {
			cout << '.' << flush;
			nrn.init();
			for( int i=0; i<2000000; i++) {
				nrn.proceedToNextState(); nrn.prepareNextState(); isi.proceedToNextState();
			}
			mean[a][b] = isi.mResult( EST_MEAN ).to_d();
			mean.setName( "mean of interspike intervals" );
			variance[a][b] = isi.mResult( EST_VAR ).to_d();
			variance.setName( "variance of interspike intervals" );
			isi.init();
		}
	}

	meanDsp << mean;
	varianceDsp << variance;
	meanDsp.show();
	varianceDsp.show();
	return 0;
	
	
	// estimator and display
	ConditionalEstimator cndNrnCpp(EST_EVENTS | EST_SAMPLE | EST_MEAN, &cpp, (Event*)&nrn, 60, 50);
	
/*	// simulate
	double t;
	srand(1);
	for(int i=0; i<8000; i++) {
		for(t=0.0; t<=20*ms; t+=dt) {
			nrn.proceedToNextState();
			nrn.prepareNextState();
			cndNrnCpp.proceedToNextState();
		}
		nrn.init();
		cpp.init();
	}
	cout << '.' << flush;

	cout << isi.mResult(EST_MEAN).to_d() << endl;
*/

/* // test formula of wiener_cpp
	ofstream datafile("test.data");
	for(double a=0.0; a<=0.1; a+=0.005) {
		for(double b=0.0; b<=0.1; b+=0.005) {
			cpp.setProperty( CPP_LAMBDA_RC, a );
			cpp.setProperty( CPP_N_RC, b);
			datafile << cpp.getProperty( WIENER_VARIANCE ) << ' ';
		}
		datafile << endl;
	}
*/

	
	// display
	Display dsp("ifneuron");
	dsp << cndNrnCpp.mResult(EST_MEAN);
	dsp << cndNrnCpp.mResult(EST_EVENTS);
	dsp.show();	
	
	// finish
	return 0;
}
