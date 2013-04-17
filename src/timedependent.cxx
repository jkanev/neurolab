//
// C++ Implementation: timedependent
//// Description: 
//
//
// Author: Jacob Kanev <j_kanev@arcor.de>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution

#include "../h/timedependent.hxx"
#include "../h/stochastic.hxx"



//__________________________________________________________________________________________
// run multiple simulations

void Time::runNested(unsigned long long events, StochasticEventGenerator *eventSource, unsigned long long maxSteps, unsigned long long runs, DataCollector *runHelper, ostream &log )
{
	vector<unsigned long long> runNumbers;
	runNumbers.push_back(0);
	for (unsigned long long r=0; r<runs; ++r) {
		runNumbers[0] = r;
		if (runHelper)
			runHelper->beforeRun(runNumbers);
		run(events, eventSource, maxSteps, log);
		if (runHelper)
			runHelper->afterRun(runNumbers);
	}
}


//__________________________________________________________________________________________
// run multiple simulations

void Time::runNested( unsigned long long steps, unsigned long long runs, DataCollector *runHelper, ostream &log )
{
	vector<unsigned long long> runNumbers;
	runNumbers.push_back(0);
	for (unsigned long long r=0; r<runs; ++r) {
		runNumbers[0] = r;
		if (runHelper)
			runHelper->beforeRun(runNumbers);
		run(steps, log);
		if (runHelper)
			runHelper->afterRun(runNumbers);
	}
}


//__________________________________________________________________________________________
// run time

void Time::run(unsigned long long steps, ostream &log, bool init)
{
	// starting note
	log << "\rstarting simulation: " 
			<< steps
			<< " steps.         \t"
			<< endl;
	
	// initialise time objects
	timePassed = 0.0;
	if (init) {
		for (uint i=0; i<timeObjects.size(); ++i)
			timeObjects[i]->init();
		for (uint i=0; i<timeEstimators.size(); ++i)
			timeEstimators[i]->init();
	}
	unsigned long long lastPercentage = 0;
	for (unsigned long long s=0; s<steps; ++s) {

		// perform time step, throw error if unsuccessful
		if (!step()) {
			log << "\rerror running simulation: some circular dependencies couldn't be resolved" << endl;
			return;
		}
		
		// collect values in all estimators
		for (int i=timeEstimators.size()-1; i+1; --i)
			timeEstimators[i]->collect();

		// advance all objects in time
		for (int i=timeObjects.size()-1; i+1; --i)
			timeObjects[i]->proceedToNextState();

		unsigned long long currentPercentage = ( s * 100ULL ) / steps;
		if (currentPercentage != lastPercentage) {
			log << "\rrunning simulation: " 
				<< currentPercentage
				<< "%         \t"
				<< flush;
			lastPercentage = currentPercentage;
		}
	}
};


//__________________________________________________________________________________________
// run time

void Time::run(unsigned long long events, StochasticEventGenerator *eventSource, unsigned long long maxSteps, ostream &log, bool init)
{
	// starting note
	log << "\rstarting simulation: " 
			<< events << " events from "
			<< eventSource->getName()
			<< ".         \t"
			<< endl;
	
	// initialise time objects
	if (init) {
		for (uint i=0; i<timeObjects.size(); ++i)
			timeObjects[i]->init();
		for (uint i=0; i<timeEstimators.size(); ++i)
			timeEstimators[i]->init();
	}
	
	unsigned long long lastPercentage = 0;
	for (unsigned long long e=0, s=0; e<events && s<maxSteps; ++s) {

		// perform time step, throw error if unsuccessful
		if (!step()) {
			log << "\rerror running simulation: some circular dependencies couldn't be resolved" << endl;
			return;
		}
		
		// collect values in all estimators
		for (int i=timeEstimators.size()-1; i+1; --i)
			timeEstimators[i]->collect();

		// advance all objects in time
		for (int i=timeObjects.size()-1; i+1; --i)
			timeObjects[i]->proceedToNextState();

		// count events
		if (eventSource->hasEvent()) {
			++e;
			s = 0;
		}
		
		// note to user
		unsigned long long currentPercentage = ( e * 100ULL ) / events;
		if (currentPercentage != lastPercentage) {
			log << "\rrunning simulation: " 
					<< currentPercentage
					<< "%         \t"
					<< flush;
			lastPercentage = currentPercentage;
		}
	}
};


//__________________________________________________________________________________________
// perform one time step

bool Time::step()
{
	// saving flags for update success
	bool someObjectsUpdated = true;
	bool allObjectsUpdated = false;
		
	// while some objects are waiting, but some can still be updated
	while (someObjectsUpdated && !allObjectsUpdated) {
		someObjectsUpdated = false;
		allObjectsUpdated = true;
		
		// try to update all waiting objects
		for (int i=timeObjects.size()-1; i+1; --i) {
			if (!timeObjects[i]->isNextStatePrepared()) {
				
				// update object
				timeObjects[i]->prepareNextState();
				
				// record success of updating
				if (timeObjects[i]->isNextStatePrepared())
					someObjectsUpdated = true;
				else
					allObjectsUpdated = false;
			}
		}
	}
	
	if (allObjectsUpdated)
		timePassed += dt;
	
	return allObjectsUpdated;
}


//__________________________________________________________________________________________
// add object to time

void Time::add( class TimeDependent *object )
{
	// find object in already registered objects
	bool found = false;
	for (uint i=0; !found && i<timeObjects.size(); ++i)
		if (timeObjects[i] == object)
			found = true;

	// add if not found
	if (!found)
		timeObjects.push_back( object );
};

void Time::add( class Estimator *object )
{
	// find object in already registered objects
	bool found = false;
	for (uint i=0; !found && i<timeEstimators.size(); ++i)
		if (timeEstimators[i] == object)
			found = true;

	// add if not found
	if (!found)
		timeEstimators.push_back( object );
};

//__________________________________________________________________________________________
// remove object from time

void Time::remove( class TimeDependent *object )
{
	// find object in already registered objects
	int n = -1;
	for (uint i=0; !(n+1) && i<timeObjects.size(); ++i)
		if (timeObjects[i] == object)
			n = i;

	// remove if found
	if (n+1)
		timeObjects.erase( timeObjects.begin() + n );
};

void Time::remove( class Estimator *object )
{
	// find object in already registered objects
	int n = -1;
	for (uint i=0; !(n+1) && i<timeEstimators.size(); ++i)
		if (timeEstimators[i] == object)
			n = i;

	// remove if found
	if (n+1)
		timeEstimators.erase( timeEstimators.begin() + n );
};

//__________________________________________________________________________________________
// construct

TimeDependent::TimeDependent( class Time *time )
{
	xTime = time;
	bTimeExternal = true;
	if(!xTime) {
		xTime = new class Time(1.0);
		bTimeExternal = false;
	}

	xTime->add( this );
};


//__________________________________________________________________________________________
// destroy

TimeDependent::~TimeDependent()
{
	xTime->remove(this);
	if( !bTimeExternal && xTime )
		delete xTime;
}
