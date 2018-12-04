#ifndef EVENT_RECORDER_H
#define EVENT_RECORDER_H

#include "stochastic.hxx"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

/*! Class for recording and re-playing of event trains.
    Can record a series of events from any StochasticEventGenerator and save them in a file.
    Can read a file and play its contents. Both ascii and binary formats are available.
*/

enum EventFileType {
	ASCII,     ///< ascii files with single event times in floating point of scientific notation separated by newline or whitespace
	BINARY8LE     ///< binary files with 8 byte integer time stamps in little endian
};

class EventPlayer : public StochasticEventGenerator
{
public:

	/// Create player that can play a file
	EventPlayer( Time* time, string filename, double resolution = 1.0, EventFileType fileType = ASCII );
	
	/// Copy constructur
	EventPlayer( const EventPlayer &eventPlayer);

	/// Get number of events in buffer
	uint getNumberEvents();

	/// Whether an event is present currently
	virtual bool hasEvent() { return playerCurrentValue>0; };

	/// Whether an event is present currently
	virtual uint getEventAmount() { return playerCurrentValue; };

	/// Proceed one time step.
	/*! This method can be overridden to implement the proceeding of one time step. This makes new information available at the current time. (See also proceedToNextState()). The default just writes stochNextValue into stochCurrentValue.  */
	virtual void proceedToNextState();
	
	/// Calculate next value (preparing next step).
	/*! This method should be overridden to implement the calculation of the next value, using information wich is available at the current time. */
	virtual void prepareNextState();
	
	/// Reset all time dependent values.
	virtual void init();
	
	/// Returns the time point of the first event
	double getStartTime();

	/// Returns the time point of the first event
	double getEndTime();

private:
	string playerFileName; ///< stores the name of the opened file, for copy constructor reasons
	vector<double> playerBuffer;
	uint playerBufferPosition;
	uint playerCurrentValue; ///< number of events present in the current cycle
	uint playerNextValue;   ///< whether an event is present in the next cycle

};

#endif
