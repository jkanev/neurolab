
#include "../h/neurolab"

int main( int argc, char **argv) 
{	
	// simulation settings
	double dt = 0.1;
	Time t(dt); // time
	int runs = 5000; // runs
	double length = 200.0;
	int steps = int(length / dt); // steps per run
	
	// set of 5 noises, correlated
	Matrix mix(5,5); // mixing matrix
	
	// values of mean, variance and covariation
	vector<double> mean; // means of processes
	vector<double> sigma; // covariation processes
	for(int i=0; i<5; i++) {
		mean.push_back( double(i) * 0.1 - 0.5);
		sigma.push_back( double(5-i) * 0.4 );
		for(int j=0; j<5; j++)
			if(i==j) mix[i][j] = 0.7;
			else mix[i][j] = 0.2;
	}	
	
	// generate noise source
	NoiseSource noisesource( &t, 5 );
	noisesource.setMixingMatrix( mix );
	
	// get noise processes
	vector<Noise *> noises; // noise processes
	noises.push_back( noisesource.createNoise(0, "Correlated Poisson Process") );
	noises.push_back( noisesource.createNoise(1, "Correlated Wiener Process") );
	noises.push_back( noisesource.createNoise(2, "Correlated Poisson Process") );
	noises.push_back( noisesource.createNoise(3, "Correlated Wiener Process") );
	noises.push_back( noisesource.createNoise(4, "Correlated Poisson Process") );
	
	// three covariation processes to test
	vector<Covariation *> covars;
	covars.push_back( new Covariation(noises[4], noises[1]) ); // poisson-wiener
	covars.push_back( new Covariation(noises[0], noises[4]) ); // poisson-poisson
	covars.push_back( new Covariation(noises[1], noises[3]) ); // wiener-wiener
	
	// set means and variances
	for(int i=0; i<5; i++) {
		noises[i]->setMean( mean[i] );
		noises[i]->setStdDev( sigma[i] );
	}
		
	// etimators for mean, variance and covariation processes
	vector<ProcessEstimator *> est;
	for( int i=0; i<5; i++)
		est.push_back( new ProcessEstimator(EST_SAMPLE | EST_MEAN | EST_VAR, noises[i], &t, steps) );
	for( int i=0; i<3; i++)
		est.push_back( new ProcessEstimator(EST_SAMPLE | EST_MEAN | EST_VAR, covars[i], &t, steps) );
	
	// create display objects
	Display dspSample("data/noisetest_sample");
	Display dspMean("data/noisetest_mean");
	Display dspVar("data/noisetest_var");
	Display dspCovar("data/noisetest_covar");
	Display dspCovarMean("data/noisetest_covar_mean");
	
	// run simulation
	t.run(runs);
	
	// display results
	Function x = input("t");
	for(int i=0; i<5; i++)	
		dspSample << est[i]->mResult(EST_SAMPLE);
	dspSample.show();

	for(int i=0; i<5; i++) {
		Function f = x * cnst(mean[i]);
		f.setXRange(0.0, length, dt);
		dspMean << est[i]->mResult(EST_MEAN) << f;
	}
	dspMean.show();
	
	for(int i=5; i<8; i++)
		dspCovar << est[i]->mResult(EST_SAMPLE);
	dspCovar.show();
	
	vector<double> covar;
	covar.push_back( noisesource.getCovariationCoeff(4, 1) );
	covar.push_back( noisesource.getCovariationCoeff(0, 4) );
	covar.push_back( noisesource.getCovariationCoeff(1, 3 ) );
	for(int i=0; i<3; i++) {
		Function f = x * cnst(covar[i]);
		f.setXRange(0.0, length, dt);
		dspCovarMean << est[5+i]->mResult(EST_MEAN) << f;
	}
	dspCovarMean.show();	
	
	return 0;
}
