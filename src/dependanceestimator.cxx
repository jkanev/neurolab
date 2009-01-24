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

#include "../h/dependanceestimator.hxx"
#include "../h/graph.hxx"
#include "../h/stochastic.hxx"

//__________________________________________________________________________
// construct
DependanceEstimator::DependanceEstimator(const Property &property, StochasticProcess *src, StochasticProcess *base, Time *time, double begin, double end, int bins, double distBegin, double distEnd, int distBins)
: Estimator(src, time)
{
	nEstimate = property;
	if (nEstimate & EST_CUR)
		nEstimate = nEstimate | EST_VAR;
	if (nEstimate & EST_VAR)
		nEstimate = nEstimate | EST_MEAN;
	xBase = base;
	dBegin = begin;
	dEnd = end;
	getIncrement = (end - begin) / double(bins);
	dependanceBins = bins;
	dDistBegin = distBegin;
	dDistEnd = distEnd;
	dDistDelta = (distEnd - distBegin) / double(distBins);
	aSamples = new double[dependanceBins];
	if (nEstimate & EST_MEAN)
		aOne = new double[dependanceBins];
	else aOne = 0;
	if (nEstimate & EST_VAR)
		aTwo = new double[dependanceBins];
	else aTwo = 0;
	if (nEstimate & EST_CUR)
		aThree = new double[dependanceBins];
	else aThree = 0;
	if (nEstimate & EST_DENS) {
		aDist = new double*[dependanceBins];
		for (int i=0; i<dependanceBins; ++i)
			aDist[i] = new double[distBins];
	}
	else aDist = 0;
	
	init();
}

//__________________________________________________________________________
// destroy
DependanceEstimator::~DependanceEstimator()
{
	if (aSamples)
		delete[] aSamples;
	if (aOne)
		delete[] aOne;
	if (aTwo)
		delete[] aTwo;
	if (aThree)
		delete[] aThree;
	if (aDist) {
		for (int i=0; i<dependanceBins; ++i)
			delete[] aDist[i];
		delete[] aDist;
	}
}

//_________________________________________
// initialize
void DependanceEstimator::init()
{
	nSamples = 0;
	for(int i=0; i<dependanceBins; i++)
		aSamples[i] = 0.0;
	if(nEstimate & EST_MEAN)
		for(int i=0; i<dependanceBins; i++)
			aOne[i] = 0.0;
	if(nEstimate & EST_VAR)
		for(int i=0; i<dependanceBins; i++)
			aTwo[i] = 0.0;
	if(nEstimate & EST_CUR)
		for(int i=0; i<dependanceBins; i++)
			aThree[i] = 0.0;
	if(nEstimate & EST_DENS) {
		int distBins = int(ceil((dDistEnd - dDistBegin) / dDistDelta));
		for( int i=0; i<dependanceBins; i++ )
			for( int j=0; j<distBins; j++ )
				aDist[i][j] = 0.0;
	}
};

//__________________________________________________________________________
// eat one time value
void DependanceEstimator::collect()
{
	// find appropriate bin
	double base, source;
	if (nEstimate & EST_DIFF)
		source = pSource->getIncrement();
	 else
		source = pSource->getCurrentValue();
	if (nEstimate & EST_BASE_DIFF)
		base = xBase->getIncrement();
	else
		base = xBase->getCurrentValue();
	int n = int(floor((base - dBegin) / getIncrement));
	
	// check if base value is in bounds
	if (n<0 || n>=dependanceBins)
		return;
		
	// record sample
	++nSamples;
	++aSamples[n];
	if (nEstimate & EST_MEAN)
		aOne[n] += source;
	if (nEstimate & EST_VAR)
		aTwo[n] += source*source;
	if (nEstimate & EST_CUR)
		aThree[n] += source*source*source;
	if (nEstimate & EST_DENS) {
		if (source < dDistBegin || source > dDistDelta)
			return;
		int m = int (floor((source - dDistBegin) / dDistDelta));
		++aDist[n][m];
	}
}

//__________________________________________________________________________
// create result
Matrix DependanceEstimator::mResult(const Property &p)
{
	if(p & nEstimate & EST_MEAN) {
		Graph a(dependanceBins);
		a.setName("conditional mean");
		if( pSource ) {
			a.setName( "mean of " + pSource->getName() + " given " + xBase->getName());
			a.setPhysical(*pSource);
			a.setPhysical(0, *xBase );
			a.setPhysical(1, *pSource);
		}
		for(int i=0; i<dependanceBins; ++i) {
			a[i][0] = dBegin + double(i) * getIncrement;
			a[i][1] = aSamples[i] ? aOne[i] / aSamples[i] : 0.0;
		}
		return a;
	}
	else if(p & nEstimate & EST_VAR) {
		Graph a(dependanceBins);
		a.setName("conditional variance");
		for(int i=0; i<dependanceBins; ++i) {
			a[i][0] = dBegin + double(i) * getIncrement;
			double mean = aOne[i] / aSamples[i];
			a[i][1] = aSamples[i] ? (aTwo[i] / aSamples[i] - mean*mean) : 0.0;
		}
		return a;
	}
	else if( p & nEstimate & EST_DENS ) {
		int distBins = int(ceil((dDistEnd - dDistBegin) / dDistDelta));
		Matrix a(dependanceBins, distBins, 3);
		a.setName("conditional density");
		for(int j=0; j<distBins; j++)
			for( int i=0; i<dependanceBins; i++ ) {
				a[i][j][0] = dBegin + getIncrement*double(i);
				a[i][j][1] = dDistBegin + dDistDelta*double(j);
				a[i][j][2] = aSamples[i] ? aDist[i][j] / aSamples[i] : 0.0;
			}
		return a;
	}
	return Matrix();
}

