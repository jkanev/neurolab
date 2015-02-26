#include "../h/seriesestimator.hxx"
#include "../h/graph.hxx"

//________________________________________________________________________________
// construct

SeriesEstimator::SeriesEstimator (const Property &property, StochasticEventGenerator *source, StochasticEventGenerator *trigger, Time *time,  int preEvents, int postEvents)
: Estimator(0, time), seriesRecords(preEvents+postEvents), seriesTriggers(10000)
{
	seriesTrigger = trigger;
	seriesSource = source;
	nEstimate = property;
	estimatorPre = preEvents;
	estimatorPost = postEvents;
	
	// adjust properties
	if (nEstimate & EST_CUR)
		nEstimate = nEstimate | EST_VAR;
	if (nEstimate & EST_VAR)
		nEstimate = nEstimate | EST_MEAN;
	if (nEstimate & EST_MEAN)
		nEstimate = nEstimate | EST_SAMPLE;
	if (nEstimate & EST_DENS)
		nEstimate = nEstimate | EST_SAMPLE;
	
	// init density estimator values
	if (nEstimate & EST_DENS) {
		// time from -200 to 200ms
		estimatorDistRange[0] = -0.2 / estimatorTime->dt;
		estimatorDistRange[1] = 0.2 / estimatorTime->dt;

		// bin size equals 2dt
		estimatorDistLength = 0.2 / estimatorTime->dt;
		
		estimatorDistOffset = double(estimatorDistRange[0]);
		estimatorDistScale = (double(estimatorDistRange[1]) - double(estimatorDistRange[0])) / double(estimatorDistLength);
	}
	
	// create arrays
	if (nEstimate & EST_SAMPLE)
		estimatorSample = new long double[estimatorPre+estimatorPost];
	else
		estimatorSample = 0;
	if (nEstimate & EST_MEAN)
		estimatorOne = new long double[estimatorPre+estimatorPost];
	else
		estimatorOne = 0;
	if (nEstimate & EST_VAR)
		estimatorTwo = new long double[estimatorPre+estimatorPost];
	else
		estimatorTwo = 0;
	if (nEstimate & EST_CUR)
		estimatorThree = new long double[estimatorPre+estimatorPost];
	else
		estimatorThree = 0;
	if (nEstimate & EST_DENS) {
		estimatorDist = new long double *[estimatorPre+estimatorPost];
		for (int i=0; i<estimatorPre+estimatorPost; ++i)
			estimatorDist[i] = new long double[estimatorDistLength];
	} else
		estimatorDist = 0;

	// initialise all arrays
	init();
}

SeriesEstimator::~SeriesEstimator()
{
	if (estimatorSample)
		delete[] estimatorSample;
	if (estimatorOne)
		delete[] estimatorOne;
	if (estimatorTwo)
		delete[] estimatorTwo;
	if (estimatorThree)
		delete[] estimatorThree;
	if (estimatorDist) {
		for (int i=0; i<estimatorPre+estimatorPost; ++i)
			delete[] estimatorDist[i];
		delete[] estimatorDist;
	}
}

//_________________________________________
// initialise
void SeriesEstimator::init()
{
	nSamples = 0;
	if(nEstimate & EST_SAMPLE)
		for(int i=0; i<estimatorPre+estimatorPost; i++)
			estimatorSample[i] = 0.0;
	if(nEstimate & EST_MEAN)
		for(int i=0; i<estimatorPre+estimatorPost; i++)
			estimatorOne[i] = 0.0;
	if(nEstimate & EST_VAR)
		for(int i=0; i<estimatorPre+estimatorPost; i++)
			estimatorTwo[i] = 0.0;
	if(nEstimate & EST_CUR)
		for(int i=0; i<estimatorPre+estimatorPost; i++)
			estimatorThree[i] = 0.0;
	if(nEstimate & EST_DENS) {
		for( int i=0; i<estimatorPre+estimatorPost; i++ )
			for( int j=0; j<estimatorDistLength; j++ )
				estimatorDist[i][j] = 0.0;
	}
	seriesRecords.clear();
	seriesTriggers.clear();
};

//________________________________________________________________________________
// eat next data point

void SeriesEstimator::collect()
{
	// advance time-count for records and triggers
	seriesRecords += 1;
	seriesTriggers += 1;
	for (uint i=0; i<seriesTrigger->getEventAmount(); i++) {
		// move new trigger into queue
		seriesTriggers << 0;
	}
	
	for (uint events = seriesSource->getEventAmount(); events; --events) {
		// move new event into ring
		seriesRecords.next(0);
		
		// record differences
		while (seriesTriggers.size()
			&& seriesRecords[estimatorPre] >= seriesTriggers.first()
			&& seriesRecords[estimatorPre+1] < seriesTriggers.first())
		{
			uint triggerTime;   // how long ago this trigger was
			seriesTriggers >> triggerTime;
			
			if (seriesRecords.isInitialized()) {
				for (int i = 0; i<estimatorPre+estimatorPost; ++i) {
					estimatorSample[estimatorPre+estimatorPost-1-i]
						= double(triggerTime) - double(seriesRecords[-i]); // time-difference of event to this trigger
				}
				processCurrentSample();
			}
		}
	}
}

//________________________________________________________________________________
// add new samples to moment estimates

void SeriesEstimator::processCurrentSample()
{
	++nSamples;
	if( nEstimate & EST_MEAN )
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			estimatorOne[i] += estimatorSample[i];
		}
	if( nEstimate & EST_VAR )
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			double d = estimatorSample[i];
			estimatorTwo[i] += d*d;
		}
	if( nEstimate & EST_CUR )
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			double d = estimatorSample[i];
			estimatorThree[i] += d*d*d;
		}
	if( nEstimate & EST_DENS )
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			int bin = (int) floor( (estimatorSample[i] - estimatorDistOffset) / estimatorDistScale + 0.5); // round
			if( (bin < estimatorDistLength) && (bin >= 0) )
				estimatorDist[ i ][ bin ]++;
		}
}

Matrix SeriesEstimator::getSample()
{
	Graph a(estimatorPre+estimatorPost);
	a.setName("sample");
	
	if(nEstimate & EST_SAMPLE) {
		if( seriesSource ) {
			a.setName( "" );
//			a.setPhysical(*seriesSource);
			a.setPhysical(0, *estimatorTime );
//			a.setPhysical(1, *seriesSource);
		}
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			a[i][0] = i - estimatorPre;
			a[i][1] = estimatorSample[i];
		}
	}		
		
	return a;
}

Matrix SeriesEstimator::getMean()
{
	double samples = (nSamples!=0.0) ? double(nSamples) : 1.0;
	Graph a(estimatorPre+estimatorPost);

	if(nEstimate & EST_MEAN) {
		a.setName("conditional mean");
		if( seriesSource ) {
			a.setName( "mean of" );
//			a.setPhysical(*seriesSource);
//			a.setPhysical(0, *xTime );
//			a.setPhysical(1, *seriesSource);
		}
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			a[i][0] = i - estimatorPre;
			a[i][1] = estimatorTime->dt *estimatorOne[i] / samples;
		}
	}
	return a;
}

Matrix SeriesEstimator::getVariance()
{
	double samples = (nSamples!=0.0)? double(nSamples): 1.0;
	Graph a(estimatorPre+estimatorPost);
	
	if (nEstimate & EST_VAR) {
		a.setName("conditional variance");
		for(int i=0; i<estimatorPre+estimatorPost; i++) {
			a[i][0] = i - estimatorPre;
			double mean = estimatorOne[i] / samples;
			a[i][1] = (estimatorTwo[i] / samples - mean*mean);
		}
	}
	return a;
}
	
Matrix SeriesEstimator::getDistribution()
{
	double samples = (nSamples!=0.0)? double(nSamples): 1.0;
	
	int timeSize = estimatorPre+estimatorPost;
	Matrix a(timeSize, estimatorDistLength, 3);
	a.setName("conditional density");
	
	if ( nEstimate & EST_DENS ) {
		for ( int j=0; j<estimatorDistLength; j++)
			for ( int i=0; i<timeSize; i++ ) {
				a[i][j][0] = i - estimatorPre;
				a[i][j][1] = estimatorTime->dt * ((double(j) * estimatorDistScale) + estimatorDistOffset);
				a[i][j][2] = estimatorDist[i][j] / samples;
			}
	}
	return a;
}
