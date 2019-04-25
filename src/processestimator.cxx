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

#include "../h/processestimator.hxx"
#include "../h/processes.hxx"
#include "../h/graph.hxx"

//________________________________________
// get next data point
void ProcessEstimator::collect()
{
	// eat next data point, without stepping stoch. variable
	if(nCurrent < nLength) {
		if( nEstimate & EST_DIFF )
			aSample[nCurrent] = pSource->getIncrement();
		else
			aSample[nCurrent] = pSource->getCurrentValue();
		++nCurrent;
	}
}

//________________________________________
// initialize to take next sample
void ProcessEstimator::init()
{
	if(nCurrent>0) {
		// record one sample
		++nSamples;
		if( nEstimate & (EST_MEAN | EST_VAR))
			for(uint i=0; i<nLength; i++)
				aOne[i] += aSample[i];
		if( nEstimate & EST_VAR )
			for(uint i=0; i<nLength; i++) {
				double d = aSample[i];
				aTwo[i] += d*d;
			}
		if( nEstimate & EST_CUR )
			for(uint i=0; i<nLength; i++) {
				double d = aSample[i];
				aThree[i] += d*d*d;
			}
		if( nEstimate & EST_DENS )
            for(uint i=0; i<nLength; i++) {
                ulong bin = ulong( floor( (aSample[i] - dDistOffset) / dDistScale + 0.5) ); // round
                if( bin <= nDist )
					aDist[ i ][ bin ]++;
			}
		nCurrent = 0;	
	}
}
	
//________________________________________
// construct
ProcessEstimator::ProcessEstimator(const Property& property, StochasticProcess *stochvar, Time *time,  uint length, const string& name, const string& type)
    : Estimator(stochvar, time, name, type)
{
	nEstimate = property;
	pSource = stochvar;
	nLength = length;
	nSamples = 0;
	nCurrent = 0;
    aDist = nullptr;
    nDist = 0;
    aOne = nullptr;
    aTwo = nullptr;
    aThree = nullptr;
	aSample = new double[nLength];
	for(uint i=0; i<nLength; ++i)
		aSample[i] = 0.0;
	if(nEstimate & (EST_MEAN | EST_VAR)) {
		aOne = new double[nLength];
		for(uint i=0; i<nLength; ++i)
			aOne[i] = 0.0;
	}
	if(nEstimate & EST_VAR) {
		aTwo = new double[nLength];
		for(uint i=0; i<nLength; ++i)
			aTwo[i] = 0.0;
	}
	if(nEstimate & EST_CUR) {
		aThree = new double[nLength];
		for(uint i=0; i<nLength; ++i)
			aThree[i] = 0.0;
	}
	if(nEstimate & EST_DENS) {
		nDist = 100;
		aDist = new double *[nLength];
		for( uint i=0; i<nLength; i++ ) {
			aDist[i] = new double[nDist];
            for(ulong j=0; j<nDist; ++j)
				aDist[i][j] = 0.0;
		}
		aDistRange[0] = -5.0;
		aDistRange[1] = 65.0;
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	}
}

//________________________________________
// destruct
ProcessEstimator::~ProcessEstimator()
{
	if(aOne) delete[] aOne;
	if(aTwo) delete[] aTwo;
	if(aThree) delete[] aTwo;
	if(aDist) {
		for(uint i=0; i<nLength; i++)
			delete[] aDist[i];
		delete aDist;
	}
}

//________________________________________
// get some property
Matrix ProcessEstimator::getEstimate(const Property& p)
{
	double samples = nSamples? double(nSamples): 1.0;
	if(nEstimate & EST_DIFF)
		samples *= estimatorTime->dt;
	
	if(p & nEstimate & EST_SAMPLE) {
		Graph a(nLength);
		a.setName("process sample");
		if( pSource ) {
			a.setName( "sample of " + pSource->getName() + " (" + pSource->getType() + ")" );
			a.setPhysical(*pSource);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, *pSource);
		}
		for(uint i=0; i<nLength; i++) {
			a[i][0] = estimatorTime->dt * (double) i;
			a[i][1] = aSample[i];
		}
		return a;
	}
	else if(p & nEstimate & EST_MEAN) {
		Graph a(nLength);
		a.setName("process mean");
		if( pSource ) {
			a.setName( "mean of " + pSource->getName() + " (" + pSource->getType() + ")" );
			a.setPhysical(*pSource);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, *pSource);
		}
		for(uint i=0; i<nLength; i++) {
			a[i][0] = estimatorTime->dt * (double) i;
			a[i][1] = aOne[i] / samples;
		}
		return a;
	}
	else if(p & nEstimate & EST_VAR) {
		Graph a(nLength);
		a.setName("process variance");
		if( pSource ) {
			string name = ((Physical *)pSource)->getUnit().getName();
			Unit unit = ((Physical *)pSource)->getUnit();
			Physical p(name, unit*unit);
			a.setName( "variance of " + pSource->getName() + " (" + pSource->getType() + ")" );
			a.setPhysical(p);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, p);
		}
		for(uint i=0; i<nLength; i++) {
			a[i][0] = estimatorTime->dt * (double) i;
			a[i][1] = (aTwo[i]/samples - (aOne[i]/samples*aOne[i]/samples));
		}
		return a;
	}
	else if( p & nEstimate & EST_DENS ) {
		Graph a(nLength, nDist);
		a.setName("process density");
		if( pSource ) {
			a.setName( "density of " + pSource->getName() + " (" + pSource->getType() + ")" );
			a.setPhysical(*pSource);
			a.setPhysical(0, *estimatorTime );
			a.setPhysical(1, *pSource);
			Physical p;
			p.setDescription("probability");
			a.setPhysical(2, p );
		}
        for(uint j=0; j<nDist; j++)
			for( uint i=0; i<nLength; i++ ) {
			a[i][j][0] = (estimatorTime->dt * (double(i) * dDistScale) + dDistOffset);
				a[i][j][1] = aDist[i][j] / samples;
			}
		return a;
	}
	return Matrix();
}

