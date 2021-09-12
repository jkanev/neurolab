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


#ifndef __TIME_DEPENDENT
#define __TIME_DEPENDENT

#include <neurolab/physical.hxx>
#include <neurolab/estimator.hxx>

#include <vector>
#include <sys/types.h>

using std::vector;

class StochasticEventGenerator;

/// Class used for piping output into nothingness
class NullStream : public std::ostream
{
public: 
	NullStream(): std::ios(0), std::ostream(0) {}
 };

/// Class used to do user defined data collection in nested runs
class DataCollector
{
	public:
		
		/// Will be executed before each run.
		virtual void beforeRun(const vector<unsigned long long int>& step) = 0;
		
		/// Will be executed after each run.
		virtual void afterRun(const vector<unsigned long long int>& step) = 0;
};

/// Time dependent objects.
/*! All objects which depend on time, especially on the size of the simulation  time, must derive from this class. */
class TimeDependent
{
private:
	bool bTimeExternal;
	
protected:
	class Time *xTime;
public:
	/// Construct.
	/*! \param  time Time object on which this object is dependent
	If 0 is given here, a new time object with time step dt=1.0 will be created. In this case the object will be independent of all changes made to the time settings otherwise in the program. This must only be used for objects, which are not dependent on the current time settings, but inherit from this class anyway (some special forms of StochasticProcess variables, like Product). */
	TimeDependent( class Time *time );
	
	virtual ~TimeDependent();

	/// Proceed one time step.
	/*! This method can be overridden to implement the proceeding of one time step. This makes new information available at the current time. (See also proceedToNextState()). The default just writes stochNextValue into stochCurrentValue.  */
	virtual void proceedToNextState() = 0;
	
	/// Calculate next value (preparing next step).
	/*! This method should be overridden to implement the calculation of the next value, using information wich is available at the current time. */
	virtual void prepareNextState() = 0;
	
	/// Reset all time dependent values.
	virtual void init() = 0;
	
	/// Whether the next state is prepared or not.
	virtual bool isNextStatePrepared() = 0;
	
	/// Return pointer to the time object
	virtual class Time *getTime() const { return xTime; };
};


/// The time.
/** This class is responsable for governing the simulation progress. */
class Time: public Physical
{
private:
	vector<class TimeDependent *> timeObjects;
	vector<class Estimator *> timeEstimators;
	
	/// Proceed time be one step.
	bool step();

public:
	double dt;
	double timePassed;
	
	/// Create time.
	Time(double timestep) {
		dt = timestep; 
		physicalUnit.set(0, 0,0,1,0,0,0,0); // ms
		physicalDescription = "time";
	};
	
	/// Destroy time.
	virtual ~Time(){};
	
	/// Return physick description.
	virtual string getPhysicalDescription() {
		return "time";
	};

	/// Run simulation for all attached objects.
	/** Runs a simulation from a start time to an end time. */
	void run(
		double startTime,     ///< time value to start witch
		double endTime,     ///< time value to stop at
		 ostream &log = cout,   ///< stream for progress messages
		 bool init = true ///< include initialising of all dependent objects
	);

	/// Run simulation for all attached objects.
	/** Runs a simulation for a certain number of time steps. This function does not reset the Time::timePassed value before start. */
	void run (
		  unsigned long long steps,   ///< number of time steps to run
		 ostream &log = cout,   ///< stream for progress messages
		 bool init = true ///< include initialising of all dependent objects
	);

	/// Run simulation for all attached objects.
	/** Runs a simulation for a certain number of events (e.g. spikes from a neuron). As a safeguard a maximum number of time steps must be given, in case the event source fails to deliver events. This function does not reset the Time::timePassed value before start. */
	void run (
		unsigned long long events,   ///< number of events until run is finished
		class StochasticEventGenerator *eventSource,   ///< event source
		unsigned long long maxSteps,   ///< maximum number of time steps, should the event source fail
		ostream &log = cout,   ///< stream for progress messages
		bool init = true ///< include initialising of all dependent objects
	);
	
	/// Run multiple simulations for all attached objects.
	/** Runs a number of simulations for a certain number of time steps. */
	void runNested (
		unsigned long long steps,   ///< number of time stepsfor each run
		unsigned long long runs,   ///< number of time runs
		DataCollector *runRecorder,   ///< object with functions to execute at beginning/end of each run
		ostream &log = cout   ///< stream for progress messages
	);
	
	/// Run multiple simulations for all attached objects.
	/** Runs a number of simulations for a certain number of time steps. */
	void runNested (
		unsigned long long events,   ///< number of events until run is finished
		class StochasticEventGenerator *eventSource,   ///< event source
		unsigned long long maxSteps,   ///< maximum number of time steps, should the event source fail
		unsigned long long runs,   ///< number of time runs
		DataCollector *runRecorder,   ///< object with functions to execute at beginning/end of each run
		ostream &log = cout   ///< stream for progress messages
	);
	
	/// Attach an object.
	void add( class TimeDependent *object );

	/// Attach an object.
	void add( class Estimator *object );

	/// Detach an object.
	void remove( class TimeDependent *object );
	
	/// Detach an object.
	void remove( class Estimator *object );
};

#endif
