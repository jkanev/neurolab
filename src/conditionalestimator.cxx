/* Copyright Information
__________________________________________________________________________

Copyright (C) 2005 Jacob Kanev

This file is part of NeuroLab.

NeuroLab is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
__________________________________________________________________________
*/

#include "../h/conditionalestimator.hxx"
#include "../h/graph.hxx"

//________________________________________
// construct

ConditionalEstimator::ConditionalEstimator(const Property& property, StochasticProcess *stochvar, StochasticEventGenerator *event,Time *time,  int pre, int post, int updateCorrection)
	: Estimator(stochvar, time), condSampleRing(pre+post+1), condEventRing(pre+post+1), condConditionRing(0)
{
	nEstimate = property;
	pSource = stochvar;
	condTrigger = event;
	condCondition = 0;
	nSamples = 0;
	aDist = 0;
	nDist = 0;
	condSamples = 0;
	aOne = 0;
	aTwo = 0;
	aThree = 0;
	aEvents = 0;
	nPre = pre;
	nPost = post;
	nUpdateCorrection = updateCorrection;
	if(nEstimate & EST_SAMPLE)
		condSamples = new double[pre+post+1];
	if(nEstimate & EST_MEAN)
		aOne = new double[pre+post+1];
	if(nEstimate & EST_EVENTS)
		aEvents = new double[pre+post+1];
	if(nEstimate & EST_VAR)
		aTwo = new double[pre+post+1];
	if(nEstimate & EST_CUR)
		aThree = new double[pre+post+1];
	if(nEstimate & EST_DENS) {
		nDist = 100;
		aDist = new double *[pre+post+1];
		for( int i=0; i<pre+post+1; i++ )
			aDist[i] = new double[nDist];
		aDistRange[0] = -5.0;
		aDistRange[1] = 65.0;
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	}
	init();
}


ConditionalEstimator::ConditionalEstimator(const Property& property, StochasticProcess *stochvar, StochasticProcess *condition, Time *time,  int pre, int post, int updateCorrection)
	: Estimator(stochvar, time), condSampleRing(pre+post+1), condEventRing(0), condConditionRing(pre+post+1)
{
	nEstimate = property;
	pSource = stochvar;
	condTrigger = 0;
	condCondition = condition;
	nSamples = 0;
	aDist = 0;
	nDist = 0;
	condSamples = 0;
	aOne = 0;
	aTwo = 0;
	aThree = 0;
	aEvents = 0;
	nPre = pre;
	nPost = post;
	nUpdateCorrection = updateCorrection;
	if(nEstimate & EST_SAMPLE)
		condSamples = new double[pre+post+1];
	if(nEstimate & EST_MEAN)
		aOne = new double[pre+post+1];
	if(nEstimate & EST_EVENTS)
		aEvents = new double[pre+post+1];
	if(nEstimate & EST_VAR)
		aTwo = new double[pre+post+1];
	if(nEstimate & EST_CUR)
		aThree = new double[pre+post+1];
	if(nEstimate & EST_DENS) {
		nDist = 100;
		aDist = new double *[pre+post+1];
		for( int i=0; i<pre+post+1; i++ )
			aDist[i] = new double[nDist];
		aDistRange[0] = -5.0;
		aDistRange[1] = 65.0;
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	}
	init();
}

//________________________________________
// receive next data point
void ConditionalEstimator::setParameter( const Property& p, double value )
{
	// assign parameter
	if (p==EST_DIST_MIN)
		aDistRange[0] = value;
	else if (p==EST_DIST_MAX)
		aDistRange[1] = value;
	else if (p==EST_DIST_BINS)
		nDist = (int)value;
	else return;
	
	// re-generate distribution
	int size = nPre+nPost+1;
	if (aDist) {
		for( int i=0; i<size; i++ )
			if (aDist[i])
				delete[] aDist[i];
		delete[] aDist;
	}
	dDistOffset = double(aDistRange[0]);
	dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	aDist = new double *[size];
	for( int i=0; i<size; i++ )
		aDist[i] = new double[nDist];
}

//________________________________________
// receive next data point
void ConditionalEstimator::collect()
{
	// eat next data point, without stepping stoch. variable
	double rnd = nEstimate & EST_DIFF ? pSource->getIncrement() : pSource->getCurrentValue();
	condSampleRing.next( rnd);
	
	// get next event, and write it into event cycle	
	if (condTrigger) {
		condEventRing.next( condTrigger->hasEvent() );
		
		// if an event has occured nPost steps ago, record all properties
		if (condSampleRing.isInitialized()) {
			if( condEventRing[nPre+1+nUpdateCorrection] && !hasAdditionalEvents() ) { // nPre+1 is equivalent to -nPost in a ring
				++nSamples;
				if( nEstimate & EST_SAMPLE )
					for(int i=0; i<nPre+nPost+1; i++)
						condSamples[i] = condSampleRing[i+1];
				if( nEstimate & EST_MEAN )
					for(int i=0; i<nPre+nPost+1; i++)
						aOne[i] += condSampleRing[i+1];
				if( nEstimate & EST_EVENTS )
					for(int i=0; i<nPre+nPost+1; i++)
						aEvents[i] += condEventRing[i+1];
				if( nEstimate & EST_VAR )
					for(int i=0; i<nPre+nPost+1; i++) {
						double d = condSampleRing[i+1];
						aTwo[i] += d*d;
					}
				if( nEstimate & EST_CUR )
					for(int i=0; i<nPre+nPost+1; i++) {
						double d = condSampleRing[i+1];
						aThree[i] += d*d*d;
					}
				if( nEstimate & EST_DENS )
					for(int i=0; i<nPre+nPost+1; i++) {
						int bin = (int) floor( (condSampleRing[i+1] - dDistOffset) / dDistScale + 0.5); // round
						if( (bin < nDist) && (bin >= 0) )
							aDist[ i ][ bin ]++;
					}
			}
		}
	} 
	else {
		condConditionRing.next( rnd /*condCondition->getIncrement()*/ );
			   
		// record all properties mulitplied with the condition at the time of recording
		if (condSampleRing.isInitialized()) {
			double cnd = condConditionRing[nPre+1-2]; // nPre+1 is equivalent to -nPost in a ring, the '2' accounts for update delays			
			++nSamples;
			if( nEstimate & EST_SAMPLE )
				for(int i=0; i<nPre+nPost+1; i++)
					condSamples[i] = condSampleRing[i+1] * cnd;
			if( nEstimate & EST_MEAN )
				for(int i=0; i<nPre+nPost+1; i++)
					aOne[i] += condSampleRing[i+1] * cnd;
			if( nEstimate & EST_VAR )
				for(int i=0; i<nPre+nPost+1; i++) {
					double d = condSampleRing[i+1];
					aTwo[i] += d*d*cnd;
				}
			if( nEstimate & EST_CUR )
				for(int i=0; i<nPre+nPost+1; i++) {
					double d = condSampleRing[i+1];
					aThree[i] += d*d*d*cnd;
				}
			if( nEstimate & EST_DENS )
				for(int i=0; i<nPre+nPost+1; i++) {
					int bin = (int) floor( (condSampleRing[i]*cnd - dDistOffset) / dDistScale + 0.5 ); // round
					if( (bin <= nDist) && (bin >= 0) )
						aDist[ i ][ bin ]++;
				}
		}
	}
}
	
//________________________________________
// get some property
Matrix ConditionalEstimator::getEstimate(const Property& p)
{
	double samples = (nSamples!=0.0)? double(nSamples): 1.0;
	if(nEstimate & EST_DIFF) {
		cout << "multiplying by dt: " << estimatorTime->dt << endl;
		samples *= estimatorTime->dt;
	}
	if(p & nEstimate & EST_SAMPLE) {
		Graph a(nPre+nPost+1);
		a.setName("sample");
		if( pSource ) {
			a.setName( "conditional sample of " + pSource->getName() + " (" + pSource->getType() + ")");
			a.setPhysical(*pSource);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, *pSource);
		}
		for(int i=0; i<nPre+nPost+1; i++) {
			a[i][0] = estimatorTime->dt * ((double) i - nPre);
			a[i][1] = condSamples[i];
		}
		return a;
	}		
	if(p & nEstimate & EST_MEAN) {
		Graph a(nPre+nPost+1);
		a.setName("conditional mean");
		if( pSource ) {
			a.setName( "conditional mean of " + pSource->getName() + " (" + pSource->getType() + ")");
			a.setPhysical(*pSource);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, *pSource);
		}
		for(int i=0; i<nPre+nPost+1; i++) {
			a[i][0] = estimatorTime->dt * ((double) i - nPre);
			a[i][1] = aOne[i] / samples;
		}
		return a;
	}
	if(p & nEstimate & EST_EVENTS) {
		Graph a(nPre+nPost+1);
		a.setName("conditional auto-correlation");
		if (pSource)
			a.setName("conditional auto-correlation of " + pSource->getName() + " (" + pSource->getType() + ")");
		for(int i=0; i<nPre+nPost+1; i++) {
			a[i][0] = estimatorTime->dt * ((double) i - nPre + 1);
			a[i][1] = aEvents[i] / samples;
		}
		return a;
	}
	else if(p & nEstimate & EST_VAR) {
		Graph a(nPre+nPost+1);
		a.setName("conditional variance");
		if (pSource)
			a.setName("conditional variance of " + pSource->getName() + " (" + pSource->getType() + ")");
		for(int i=0; i<nPre+nPost+1; i++) {
			a[i][0] = estimatorTime->dt * ((double) i - nPre);
			double mean = aOne[i] / samples;
			a[i][1] = (aTwo[i] / samples - mean*mean);
		}
		return a;
	}
	else if( p & nEstimate & EST_DENS ) {
		int timeSize = nPre+nPost+1;
		Matrix a(timeSize, nDist, 3);
		a.setName("conditional density");
		if (pSource)
			a.setName("conditional density of " + pSource->getName() + " (" + pSource->getType() + ")");
		for(int j=0; j<nDist; j++)
			for( int i=0; i<timeSize; i++ ) {
				a[i][j][0] = estimatorTime->dt * ((double) i - nPre);
				a[i][j][1] = (double(j) * dDistScale) + dDistOffset;
				a[i][j][2] = aDist[i][j] / samples;
			}
		return a;
	}
	return Matrix();
}

//________________________________________
// destruct
ConditionalEstimator::~ConditionalEstimator()
{
	if(condSamples) delete[] condSamples;
	if(aOne) delete[] aOne;
	if(aTwo) delete[] aTwo;
	if(aThree) delete[] aTwo;
	if(aEvents) delete[] aEvents;
	if(aDist) {
		for(int i=0; i<nPre+nPost+1; i++)
			delete[] aDist[i];
		delete aDist;
	}
}

//_________________________________________
// initialize
void ConditionalEstimator::init()
{
	bRejection = false;
	nSamples = 0;
	if(nEstimate & EST_MEAN)
		for(int i=0; i<nPre+nPost+1; i++)
			aOne[i] = 0.0;
	if(nEstimate & EST_EVENTS)
		for(int i=0; i<nPre+nPost+1; i++)
			aEvents[i] = 0.0;
	if(nEstimate & EST_VAR)
		for(int i=0; i<nPre+nPost+1; i++)
			aTwo[i] = 0.0;
	if(nEstimate & EST_CUR)
		for(int i=0; i<nPre+nPost+1; i++)
			aThree[i] = 0.0;
	if(nEstimate & EST_DENS) {
		for( int i=0; i<nPre+nPost+1; i++ )
			for( int j=0; j<nDist; j++ )
				aDist[i][j] = 0.0;
	}
	condSampleRing.clear();
	condEventRing.clear();
};

//_________________________________________
// set rejection
void ConditionalEstimator::setRejection(int pre, int post)
{
	aRejectionInterval[0] = pre;
	aRejectionInterval[1] = post;
	bRejection = true;
}

//_________________________________________
// remove rejection
void ConditionalEstimator::removeRejection()
{
	bRejection = false;
};

//_________________________________________
// find out whether there is more than one event in the rejection interval
bool ConditionalEstimator::hasAdditionalEvents()
{
	if (!bRejection)
		return false;
	else {
		int events = -1;
		for (int i=-aRejectionInterval[0]; events<1 && i<aRejectionInterval[1]; ++i)
			events += condEventRing[nPre+1-2+i];
		return events;
	}
};




