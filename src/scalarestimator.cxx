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

#include "../h/scalarestimator.hxx"
#include "../h/stochastic.hxx"

//____________________________________________________________________________
//
//  scalar estimator
//

ScalarEstimator::ScalarEstimator(const Property& estimate, StochasticProcess *src, Time *time)
: Estimator(src, time)
{
	nEstimate = estimate;
	nSamples = 0;
	aDist = 0;
	nDist = 0;
	if(nEstimate & EST_DENS) {
		nDist = 100;
		aDist = new double[nDist];
		aDistRange[0] = -5.0;
		aDistRange[1] = 5.0;
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	}
	init();
}

void ScalarEstimator::collect()
{
	// get the value
	if(nEstimate & EST_DIFF)
		estimate( pSource->getIncrement() );
	else
		estimate( pSource->getCurrentValue() );
}

void ScalarEstimator::setProperty(const Property& p, double d)
{
	if( p == EST_DIST_MIN ) {
		aDistRange[0] = d;
		delete[] aDist;
		aDist = new double[nDist];
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	} else if ( p == EST_DIST_MAX ) {
		aDistRange[1] = d;
		delete[] aDist;
		aDist = new double[nDist];
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	} else if ( p == EST_DIST_BINS ) {
		if( d>0 )
			nDist = (int) d;
		delete[] aDist;
		aDist = new double[nDist];
		dDistOffset = double(aDistRange[0]);
		dDistScale = (double(aDistRange[1]) - double(aDistRange[0])) / double(nDist);
	}
}

// this is separate because it may be called by derived classes...
void ScalarEstimator::estimate(double d)
{
	++nSamples;
	
	// record moments
	if(nEstimate & EST_SAMPLE)
		dSample = d;
	if(nEstimate & EST_MEAN)
		dOne += d;
	if(nEstimate & EST_VAR)
		dTwo += d*d;
	if(nEstimate & EST_CUR)
		dThree += d*d*d;

	// record density
	if(nEstimate & EST_DENS) {
		int bin = (int) floor( (d - dDistOffset) / dDistScale + 0.5); // round
		if( (bin <= nDist) && (bin >= 0) ) {
			aDist[ bin ]++;
		} else {
			;
		}
	}
}


Matrix ScalarEstimator::getEstimate(const Property& p)
{
	double samples = nSamples? double(nSamples): 1.0;
	
	if(p & nEstimate & EST_SAMPLE) {
		Matrix a;
		if(pSource) {
			a.setPhysical(*pSource);
			a.setName("sample of " + pSource->getName() + " (" + pSource->getType() + ")" );
		}
		a = dSample;
		return a;
	}
	else if(p & nEstimate & EST_MEAN) {
		Matrix a;
		if(pSource) {
			a.setPhysical(*pSource);
			a.setName("mean of " + pSource->getName() + " (" + pSource->getType() + ")" );
		}
		a = dOne / samples;
		return a;
	}
	else if(p & nEstimate & EST_VAR) {
		Matrix a;
		double mean = dOne / samples;
		if(pSource) {
			a.setPhysical(*pSource);
			a.setName("variance of " + pSource->getName() + " (" + pSource->getType() + ")" );
		}
		a = (dTwo/samples - mean*mean);
		return a;
	}
	else if(p & nEstimate & EST_DENS) {
		Matrix a(nDist, 2);
		a.setPhysical(0, *(new Physical()));
		if(pSource) {
			a.setPhysical(*pSource);
			a.setName("probability distribution"+ pSource->getName() + " (" + pSource->getType() + ")" );
		}
		for(int i=0; i<nDist; i++) {
			a[i][0] = (double(i) * dDistScale) + dDistOffset;
            a[i][1] = aDist[i] / samples / dDistScale;
		}
		return a;
	}
	cout << "didn't find property" << endl;
	cout << "stored: " << nEstimate << ", requested: " << p << endl;
	return Matrix();
}

ScalarEstimator::~ScalarEstimator()
{
	if(nDist)
		delete[] aDist;
}

void ScalarEstimator::init()
{
	nSamples = 0;
	dSample = 0.0;
	dOne = 0.0;
	dTwo = 0.0;
	dThree = 0.0;
	if(aDist)
		for(int i=0; i<nDist; i++)
			aDist[i] = 0.0;
}
