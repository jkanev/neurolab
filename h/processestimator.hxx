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

#ifndef PROCESS_ESTIMATOR
#define PROCESS_ESTIMATOR

#include "estimator.hxx"
#include "processes.hxx"
#include "timedependent.hxx"

using namespace std;

/// Estimates mean, var, etc. of a stochastic process.
/** Records a sample from a given process from the start for a given time period. After that period, recording is stopped, until init() is called. This is useful, when the beginning of processes has to be estimated, and these processes are not events. */
class ProcessEstimator: public Estimator
{
private:
	int nEstimate;  // what to record
	unsigned int nCurrent;     // #timesteps after last init
	unsigned int nLength;     // amount of timesteps to take
	double *aSample; // last sample
	double *aOne;    // recording first moment
	double *aTwo;    // recording second moment
	double *aThree;  // recording third moment
	int nDist;      // length of aDist
	double aDistRange[2]; // range of dist
	double dDistOffset; // offset of dist (helper variable)
	double dDistScale; // scale of dist (helper variable)
	double **aDist;  // record distribution
public:

	/// initialise to take next process sample
	virtual void init();
	
	/// receive next data point
	virtual void collect();
	
	/// get a property
	virtual Matrix mResult(const Property&);
	
	/// construct
	ProcessEstimator(const Property&, StochasticProcess *, Time *, int length);
	
	/// destruct
	virtual ~ProcessEstimator();
};

#endif
