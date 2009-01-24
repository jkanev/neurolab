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

#ifndef INTERVAL_ESTIMATOR_H
#define INTERVAL_ESTIMATOR_H

#include "scalarestimator.hxx"
#include "processes.hxx"

/// estimates interval distribution, mean, var, etc. of a time series
/** This class gets an Event-pointer, and records mean, variance etc. of inter-event intervals. This is useful for any sort of point process - f.i. a Neuron, which emits spikes as events. */
class IntervalEstimator: public ScalarEstimator
{
private:
	int nTime;
protected:
	StochasticEventGenerator *pEvent;
	const class Time *xTime; // time step size
public:
	IntervalEstimator(const Property&, StochasticEventGenerator *, class Time*); ///< Constructor
	virtual ~IntervalEstimator(); ///< Destructor
	virtual void collect(); ///< Eat next piece of data
	virtual void init(); ///< reset all counters
};

#endif
