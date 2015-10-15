
#include "../h/neurolab"

int main(int argc, char *argv[])
{
	double end = 10;
	double a = -0.1, b = 1.0;
	
	for (double dt = 0.1; dt > 0.00005; dt /= 10) {
		Time time(dt);
		int n = int(end/dt);
		TimeProcess t(&time);
		cout << n << endl;		
		
		// wiener process
		Wiener noise( &time );
		noise.setMean( 0 );
		
		// Ito: dXt = m Xt dt + s Xt dWt
		DifferentialEquation diffOne(&time, 1.0, 0.0);
		diffOne.addTerm( new Product(b), &noise );
		diffOne.addTerm( new Product(a), &t );
		diffOne.setIto();
		
		// Stratonovich: m Xt dt + s Xt o dWt
		DifferentialEquation diffTwo(&time, 1.0, 0.0);
		diffTwo.addTerm( new Product(b), &noise );
		diffTwo.addTerm( new Product(a), &t );
		diffTwo.setStratonovich();
			
		// estimators for samples
		ProcessEstimator estOne(EST_SAMPLE, &diffOne, &time, n);
		ProcessEstimator estTwo(EST_SAMPLE, &diffTwo, &time, n);
		ProcessEstimator estNoise(EST_SAMPLE, &noise, &time, n);
		ProcessEstimator estTime(EST_SAMPLE, &t, &time, n);
		
		// run
		time.run(n);
		
		// results
		Matrix sampleOne = estOne.getEstimate(EST_SAMPLE).setName("dX_t = a X_t dt + b X_t dW_t (Itô)");
		Matrix sampleTwo = estTwo.getEstimate(EST_SAMPLE).setName("dX_t = a X_t dt + b X_t o dW_t (Stratonovich)");
		Matrix sampleNoise = estNoise.getEstimate(EST_SAMPLE).setName("Wt");
		Matrix sampleTime = estTime.getEstimate(EST_SAMPLE).setName("t");
		
		// calculate explicit expressions
		Matrix xplOne(n,2);
		xplOne.setName("X_t = exp((a - ½ b²) t + b W_t) (explicit, Itô)");
		Matrix xplTwo(n,2);
		xplTwo.setName("X_t = exp(a t + b W_t) (explicit, Stratonovich)");
		for (int i=0; i<n; ++i) {
			xplOne[i][0] = sampleNoise[i][0].to_d();
			xplOne[i][1] = exp((a - 0.5*b*b)*sampleNoise[i][0].to_d() + b*sampleNoise[i][1].to_d());
			xplTwo[i][0] = sampleNoise[i][0].to_d();
			xplTwo[i][1] = exp(a*sampleNoise[i][0].to_d() + b*sampleNoise[i][1].to_d());
		}
		
		// display
		stringstream dtStringOne;
		dtStringOne << "data/TestStratOne" << '_' << dt;
		Display dspOne(dtStringOne.str().c_str());
		dspOne << sampleOne << sampleTwo
			<< showdsp << savepng;
		
		stringstream dtStringTwo;
		dtStringTwo << "data/TestStratTwo" << '_' << dt;
		Display dspTwo(dtStringTwo.str().c_str());
		dspTwo.setMode(DSP_TOP);
		dspTwo << sampleOne << xplOne;
		dspTwo.setMode(DSP_BOTTOM);
		dspTwo << sampleTwo << xplTwo
			<< showdsp << savepng;
		
		stringstream dtStringThree;
		dtStringThree << "data/TestStratThree" << '_' << dt;
		Display dspThree(dtStringThree.str().c_str());
		dspThree.setMode(DSP_TOP);
		Matrix errorOne(n,2);
		for(int i=0; i<n; i++) {
			errorOne[i][0] = sampleOne[i][0].to_d();
			errorOne[i][1] = 100.0 * (sampleOne[i][1].to_d() - xplOne[i][1].to_d()) / xplOne[i][1].to_d();
		}
			dspThree << errorOne.setName("relative error Ito (%)");
		dspThree.setMode(DSP_BOTTOM);
		Matrix errorTwo(n,2);
		for(int i=0; i<n; i++) {
			errorTwo[i][0] = sampleTwo[i][0].to_d();
			errorTwo[i][1] = 100 * (sampleTwo[i][1].to_d() - xplTwo[i][1].to_d()) / xplTwo[i][1].to_d();
		}
		dspThree << errorTwo.setName("relative error Stratonovich (%)")
			<< showdsp << savepng;
	}
		
}
