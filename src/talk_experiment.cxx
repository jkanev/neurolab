
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

// physical values
const double ms = 0.001;
const double s = 1.0;
const double Hz = 1.0;
const double mV = 1.0;

/// Prototype for plotting function for different regimes
int plot(double dt, double n, double w, double lambda, double n_rc, double lambda_rc, double, double, double, double, string);

/// Main function.
int main(int argc, char *argv[])
{
	// synaptic parameters
	double dt = 0.5*ms;
	double w = 0.0028;
	double n = 1000;
	double lambda = 5*Hz;
	double lambda_rc = 0.5;
	double n_rc = 0.4;
	
	// neural parameters
	double v0 = -65;
	double theta = -54.5;
	double tau = 1.0;
	double vRest = -70;

	// subthreshold, high variance
	n_rc = 0.5; lambda_rc = 0.5;
	plot(dt, n, w, lambda, n_rc, lambda_rc, v0, theta, tau, vRest, "subHiCv");
	// subthreshold, low variance
	n_rc = 0.1; lambda_rc = 0.5;
	plot(dt, n, w, lambda, n_rc, lambda_rc, v0, theta, tau, vRest, "subLoCv");
	// superthreshold, high variance
	w = 0.0035;
	n = 1000;
	n_rc = 0.5; lambda_rc = 0.1;
	plot(dt, n, w, lambda, n_rc, lambda_rc, v0, theta, tau, vRest, "superHiCv");
	// superthreshold, low variance
	n_rc = 0.1; lambda_rc = 0.1;
	plot(dt, n, w, lambda, n_rc, lambda_rc, v0, theta, tau, vRest, "superLoCv");
}

int plot(double dt, double n, double w, double lambda, double n_rc, double lambda_rc,
		double v0, double theta, double tau, double vRest, string fileBase)
{
	// generate condensed Poisson process
	WienerCpp cpp( dt, w, n, lambda, n_rc, lambda_rc ); // used cpp with syn-fire wave
	WienerCpp cppBg( dt, w, n, lambda, 0.0, 0.0 ); // reference cpp with no coincidences
	cpp.setActive();
		
	// build neuron
	IfNeuron nrn = IfNeuron(dt, v0, theta, tau, vRest);
	nrn.setPassive();
	nrn.addStimulus(&cpp);
//	nrn.calibrate(50, 100);

	ConditionalEstimator estRsc( dt, EST_DIFF | EST_MEAN | EST_VAR, &cpp, (Event*)&nrn, 500, 500);
		
	// theoretical mean
	Function mean( "mu(x)", "((theta - vInf)*exp(-abs(x)) + vInf)" );
	Function dmean( "dmu(x)", "-(theta-vInf)*exp(-abs(x))" );
	Variable vInf = { "vInf", vRest + cpp.getProperty( WIENER_MEAN ) };
	mean.setVariable(vInf);
	dmean.setVariable(vInf);
	mean.setVariable(vInf);
	Variable v0v = { "v0", v0 };
	mean.setVariable(v0v);
	dmean.setVariable(v0v);
	Variable thetav = { "theta", theta };
	mean.setVariable(thetav);
	dmean.setVariable(thetav);
	
	// theoretical std.dev.
	Function stddev( "sigma(x)", "sqrt(0.5 * ssigma * (1-exp(-2*abs(x))))" );
	Function dstddev( "dsigma(x)", "-exp(-2*abs(x)) * sqrt( ssigma / (2 * (1-exp(-2*abs(x)))) )" );
	Variable sigma = {"ssigma", cpp.getProperty( WIENER_VARIANCE )};
	stddev.setVariable( sigma );
	dstddev.setVariable( sigma );
	
	// borders of the equal dist.
	Function a( "a(x)", "sqrt(3.0) * sigma(x)" );
	a.setTerm( stddev );
	
	Function da( "da(x)", "sqrt(3.0) * dsigma(x)" );
	da.setTerm( dstddev );
	
	// adjusted voltage
	Function meanAdjusted( "a_mu(x)", "(theta<=(mu(x)+a(x))) *  (2.0/3.0*(mu(x) - a(x)) + theta/3.0) + (theta>(mu(x)+a(x))) * (mu(x) - a(x)**2 / 3.0 / (theta - mu(x)))" );
	meanAdjusted.setVariable( thetav );
	meanAdjusted.setTerm( mean );
	meanAdjusted.setTerm( a );
	
	Function dmeanAdjusted( "da_mu(x)", "(theta<=(mu(x)+a(x))) * 2.0/3.0*(dmu(x) - da(x)) + (theta>(mu(x)+a(x))) * ((1.0 - a(x)**2 / 3.0 / (theta - mu(x))**2)*dmu(x) - 1.0/3.0/(theta - mu(x))*2.0*a(x)*da(x))");
	dmeanAdjusted.setTerm( mean );
	dmeanAdjusted.setTerm( dmean );
	dmeanAdjusted.setTerm( a );
	dmeanAdjusted.setTerm( da );

	// rsc complete
	Function rsc( "rsc(x)", "(x<=0)*(da_mu(x) - (vr - a_mu(x))) / mu + (x>0)" );
	Variable mu = {"mu", cpp.getProperty(WIENER_MEAN)};
	rsc.setVariable( mu );
	Variable vr = { "vr", vRest };
	rsc.setVariable(vr);
	rsc.setTerm( meanAdjusted );
	rsc.setTerm( dmeanAdjusted );
	
	// rsc complete with clean mean
	Function rscClean( "rsc_clean(x)", "(dmu(x) - (vr - mu(x))) / mu" );
	rscClean.setVariable( mu );
	rscClean.setVariable(vr);
	rscClean.setTerm( mean );
	rscClean.setTerm( dmean );
	
	// theoretical std.dev. for background noise
	Function stddevBg( "sigma_bg(x)", "sqrt(0.5 * ssigma_bg * (1-exp(-2*abs(x))))" );
	Function dstddevBg( "dsigma_bg(x)", "-exp(-2*abs(x)) * sqrt( ssigma_bg / (2 * (1-exp(-2*abs(x)))) )" );
	Variable sigmaBg = {"ssigma_bg", cppBg.getProperty( WIENER_VARIANCE )};
	stddevBg.setVariable( sigmaBg );
	dstddevBg.setVariable( sigmaBg );
	
	// borders of the equal dist. for background noise only
	Function aBg( "a_bg(x)", "(sqrt(3.0) * sigma_bg(x)) / mu" );
	aBg.setTerm( stddevBg );
	
	Function daBg( "da_bg(x)", "sqrt(3.0) * dsigma_bg(x)" );
	daBg.setTerm( dstddevBg );
	
	// adjusted voltage for background noise only
	Function meanAdjustedBg( "a_mu_bg(x)", "(theta<=(mu(x)+a_bg(x))) *  (2.0/3.0*(mu(x) - a_bg(x)) + theta/3.0) + (theta>(mu(x)+a_bg(x))) * (mu(x) - a_bg(x)**2 / 3.0 / (theta - mu(x)))" );
	meanAdjustedBg.setVariable( thetav );
	meanAdjustedBg.setTerm( mean );
	meanAdjustedBg.setTerm( aBg );
	
	Function dmeanAdjustedBg( "da_mu_bg(x)", "(theta<=(mu(x)+a_bg(x))) * 2.0/3.0*(dmu(x) - da_bg(x)) + (theta>(mu(x)+a_bg(x))) * ((1.0 - a_bg(x)**2 / 3.0 / (theta - mu(x))**2)*dmu(x) - 1.0/3.0/(theta - mu(x))*2.0*a_bg(x)*da_bg(x))");
	dmeanAdjustedBg.setTerm( dmean );
	dmeanAdjustedBg.setTerm( daBg );

	// rsc complete for background noise only
	Function rscBg( "rsc_bg(x)", "(x<=0) * (da_mu_bg(x) - (vr - a_mu_bg(x))) / mu + (x>0)" );
	rscBg.setVariable(mu);
	rscBg.setVariable(vr);
	rscBg.setTerm( meanAdjustedBg );
	rscBg.setTerm( dmeanAdjustedBg );
	
	
	// display
	Display dspRsc(string("data/"+fileBase+"test").c_str());
	Display dspRsc2(string("data/"+fileBase+"split").c_str());
	// simulate
/*	Matrix allResults(21,1001,2);
	allResults.setName( "changing relative coincidence width" );
	int nAllResults = 0;
	for(n_rc = 0; n_rc<1.0; n_rc += 0.05) {
		cpp.setProperty( CPP_LAMBDA_RC, n_rc );
		estRsc.init();
*/		stringstream name;
		name << "n_rc = " << n_rc;
		cout << name.str() << endl;
		for(int i=0; i<100; i++) {
			for(int t=0; t<100000; t++) {
				nrn.step();
				nrn.next(); 
				estRsc.step();
			}
			cpp.init();
			cout << "\r\t" << i+1 << "%\t" << flush;
		}
		cout << endl;
		Matrix m = estRsc.mResult( EST_MEAN );
		m.transpose();
		m[1].multiply( 1/mu.value  );
		m.transpose();
		string matrixName;
		name >> matrixName;
		m.setName( matrixName.c_str() );
/*		dspRsc2 << m;
		allResults[ nAllResults++ ] = m;
	}
	for( int i=0; i<10; i++ )
		allResults[i] = allResults[i+1] - allResults[i];
	allResults[10] = allResults[10] - allResults[10];
	dspRsc.setMode( DSP_3D );
	dspRsc << allResults;
*/
	dspRsc << rsc << m;
	dspRsc.show();
	dspRsc2 << rsc << rscBg << rscClean;
	dspRsc2.setMode( DSP_2D_BARS );
	dspRsc2.show();
	
	return 0;

}
