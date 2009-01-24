
#include "../h/neurolab"

int main(int argc, char *argv[])
{
	double end = 10;
	double a = -0.1, b = 1.0;
	
	for (double dt = 0.1; dt > 0.0005; dt /= 10) {
		
		Time timeSettings(dt);
		int n = int(end/dt);
		TimeProcess time(&timeSettings);
		
		// wiener process
		Wiener noise( &timeSettings );
		noise.setMean( 0 );
		noise.setPassive();
		
		// Ito: dXt = m Xt dt + s Xt dWt
		DifferentialEquation diffOne(&timeSettings, 1.0, 0.0);
		diffOne.addTerm( new Product(b), &noise );
		diffOne.addTerm( new Product(a), &time );
		diffOne.setIto();
		
		// Stratonovich: m Xt dt + s Xt o dWt
		DifferentialEquation diffTwo(&timeSettings, 1.0, 0.0);
		diffTwo.addTerm( new Product(b), &noise );
		diffTwo.addTerm( new Product(a), &time );
		diffTwo.setStratonovich();
			
		// estimators for samples
		ProcessEstimator estOne(EST_SAMPLE, &diffOne, n);
		ProcessEstimator estTwo(EST_SAMPLE, &diffTwo, n);
		
		// matrixes for explicit expressions
		Matrix xplOne(n,2);
		xplOne.setName("Xt = exp( (a - 1/2b^2)t + bWt ) (Ito, explicit)");
		Matrix xplTwo(n,2);
		xplTwo.setName("Xt = exp( at + bWt ) (Stratonovich, explicit)");
		
		// run
		for( int i=0; i<n; i++ ) {
				
			xplOne[i][0] = time.getCurrentValue();
			xplOne[i][1] = exp((a - 0.5*b*b)*time.getCurrentValue() + b*noise.getCurrentValue());
			xplTwo[i][0] = time.getCurrentValue();
			xplTwo[i][1] = exp(a*time.getCurrentValue() + b*noise.getCurrentValue());
			
			time.prepareNextState();
			noise.prepareNextState();
			diffOne.prepareNextState();
			diffTwo.prepareNextState();
			
			time.proceedToNextState();
			noise.proceedToNextState();			
			diffOne.collect();
			diffTwo.proceedToNextState();
			
			estOne.collect();
			estTwo.collect();	
		}
		
		// results
		Matrix sampleOne = estOne.mResult(EST_SAMPLE).setName("dXt = a Xt dt + b Xt dWt (Ito)");
		Matrix sampleTwo = estTwo.mResult(EST_SAMPLE).setName("dXt = a Xt dt + b Xt o dWt (Stratonovich)");
		
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
		Matrix errorOne(sampleOne - xplOne);
		for(int i=0; i<n; i++) errorOne[i][0] = sampleOne[i][0];
		dspThree << errorOne.setName("error Ito");
		dspThree.setMode(DSP_BOTTOM);
		Matrix errorTwo(sampleTwo - xplTwo);
		for(int i=0; i<n; i++) errorTwo[i][0] = sampleTwo[i][0];
		dspThree << errorTwo.setName("error Stratonovich")
			<< showdsp << savepng;
	}
		
}
