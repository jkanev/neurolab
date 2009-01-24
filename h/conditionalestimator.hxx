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

#ifndef CONDITIONAL_ESTIMATOR_H
#define CONDITIONAL_ESTIMATOR_H

#include "estimator.hxx"
#include "processes.hxx"
#include "timedependent.hxx"

/// estimates conditional mean, var, etc. of a stochastic process
/**
	The conditional estimator records conditional probabilities. It links to a stochastic variable, and an event (both of which are given in the constructor). The variable and the state of the event are recorded each time prepareNextState() is called. If the event is true at a timestep, then the object will go on recording for a number of time steps, and then take all the properties. This enables you to take for instance the mean of a process at 20 time steps before to 10 time step after the occurance of a special event. In that case the 20 time steps would be given as 'pre', the 10 time steps as 'post' to the constructor. The main purpose of this class (at the time it was written) was the recording of spike-triggered averages in neurons.
*/
class ConditionalEstimator: public Estimator
{
private:
	StochasticEventGenerator *condTrigger; ///< event triggering record action
	StochasticProcess *condCondition; ///< condition
	unsigned int condCountdown; ///< countdown after an event has occured
	Ring<double> condSampleRing; // current sample
	Ring<bool> condEventRing; ///< ring with circling events
	Ring<double> condConditionRing; ///< ring with circling conditions
	double *condSamples; // recording sample
	double *aOne;    // recording first moment
	double *aTwo;    // recording second moment
	double *aThree;  // recording third moment
	double *aEvents; // recording events (auto-correlation)
	double **aDist;  // record distribution
	int nDist;      // length of aDist
	double dDistOffset;     // offset of distribution
	double dDistScale;     // scale of distribution
	int nPre;     // number of timesteps before event
	int nPost;     // number of timesteps after event
	double aDistRange[2]; // range of distribution
	int aRejectionInterval[2]; // interval for event rejection
	bool bRejection;
	int nUpdateCorrection;
	
	bool hasAdditionalEvents(); // returns true if there is more than one event inside the rejection interval
public:

	/// Construct.
	/** This version constructs an object which records (and adds) samples of a stochastic process, depending on whether an event has occured: result = { stochvar(t), t in [pre,post] | condition(0) }. */
	ConditionalEstimator(
		const Property& property, ///< a collection of things to record
		StochasticProcess *stochvar,  ///< the address of the stochastic variable to record
		StochasticEventGenerator *condition, ///< the address of the event on which the recording is conditioned
		Time *time, ///< the global time object
		int pre, ///< number of time steps before the event 
		int post, ///< number of time steps after the event 
		int updateCorrection=0 ///< optional; defaults to 0. number of time steps to shift the result;  this is to correct different update cascades
	);
	
	/// Construct.
	/** This version constructs an object which records (and adds) samples of a stochastic process, multiplied with the value of another process: result = { stochvar(t)*condition(0), t in [pre,post] }. */
	ConditionalEstimator(
		const Property& property, ///< a collection of things to record
		StochasticProcess *stochvar,  ///< the address of the stochastic variable to record
		StochasticProcess *condition, ///< the address of the process on which the recording is conditioned
		Time *time, ///< the global time object
	int pre, ///< number of time steps before the event 
		int post, ///< number of time steps after the event 
		int updateCorrection=0 ///< optional; defaults to 0. number of time steps to shift the result;  this is to correct different update cascades
	);
	
	/// Add rejection interval
	/** Special. You will hardly ever need this. If a rejection interval is set, traces of the process where additional events fall into the interval -pre - post are rejected. (this changes the meausured process \f[X_t\f] to \f[ Y_\tau = \{ X_{t+\tau} | c(t+\tau) \hat \not c(t+s), s \in [-pre,post]/0\}\f] */
	void setRejection(
		 int pre, ///< start of rejection interval, counted from the event (t=0)
		int post ///< end of rejection interval, counted from the event (t=0)
	);
	
	/// Remove rejection
	/** Sets the estimator to normal mode again. See setRejection(). */
	void removeRejection();
	
	/// set parameter value
	virtual void setParameter( const Property& p, double value );
	
	/// receive next data point
	virtual void collect();
		
	/// get some property
	virtual Matrix mResult(const Property&);
	
	/// initialize
	void init();
	
	/// destruct
	virtual ~ConditionalEstimator();
};

#endif
