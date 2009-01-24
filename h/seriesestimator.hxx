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

#ifndef SERIES_ESTIMATOR_HXX
#define SERIES_ESTIMATOR_HXX

#include "queue.hxx"
#include "estimator.hxx"
#include "stochastic.hxx"

class SeriesEstimator : public Estimator
{
protected:
	Ring<uint> seriesRecords;   ///< how many timesteps ago did process events happen
	Queue<uint> seriesTriggers;  ///< how many timesteps ago did trigger events happen
	
	StochasticEventGenerator *seriesSource; ///< source of data
	StochasticEventGenerator *seriesTrigger; ///< event triggering record action
	double *estimatorSample; // recording sample
	double *estimatorOne;    // recording first moment
	double *estimatorTwo;    // recording second moment
	double *estimatorThree;  // recording third moment
	double **estimatorDist;  // record distribution
	int estimatorDistLength;      // length of aDist
	double estimatorDistOffset;     // offset of distribution
	double estimatorDistScale;     // scale of distribution
	int estimatorPre;     // number of timesteps before event
	int estimatorPost;     // number of timesteps after event
	double estimatorDistRange[2]; // range of distribution
	int estimatorUpdateCorrection;
	
	void processCurrentSample();
	void init();
public:
	
	void collect();
	
	/// Construct Object
	/** Constructs a SeriesEstimator. */
	SeriesEstimator (
		const Property &property, ///< a collection of things to record
		StochasticEventGenerator *src, ///< source variable
		StochasticEventGenerator *trigger, ///< trigger
		Time *time, ///< global time object
		int preEvents, ///< events to record before trigger
		int postEvents ///< events to record before trigger
	);
	
	/// Destroy
	~SeriesEstimator();
	
	Matrix getSample();
	
	Matrix getMean();
	
	Matrix getVariance();
	
	Matrix getStdDev();
	
	Matrix getDistribution();
	
	
	virtual Matrix mResult(const Property&) { return Matrix(); } ///< Return an estimation.
};

#endif
