
/* Copyright Information
__________________________________________________________________________

Copyright (C) 2013 Jacob Kanev

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

#ifndef _EVENTMULTIPLEXER_HXX
#define _EVENTMULTIPLEXER_HXX

#include <neurolab/stochastic.hxx>

/*! Mulitplexer for various event sources
 Uses several input processes that generate events and produces a train of response events which is a sum of its inputs (if mode=true) or which is an and-operation on its inputs (if mode=false). Each time point any of the source processes has an event, this class has an event too. If several sources have an event, the getEventAmount() function will return the appropriate number.
 */
class EventMultiplexer : public StochasticEventGenerator
{
public:
	/*! Constructor
	  Create an instance. */
	EventMultiplexer (Time* time, const bool& mode=true, const string& name = "", const string& type = "Event Mulitplexer");

	/*! Destructor
	  Destroy an instance. */
	virtual ~EventMultiplexer() {};
	
	/// The amount of events
	/*! Returns the difference of the value in the next time step to the value in the current time step. */
	virtual uint getEventAmount();
	
	/// Calculate the value for the next time step
    /*! Each event process is a counting process. If any of the source processes has an event (i.e. the value in the next time step is higher than the value in the current time step), this class has an event two (its value in the next time step will be higher than its value in the current time step. */
    virtual void prepareNextState();
	
    /// Proceed to the next time step
    virtual void proceedToNextState();
    
    /// Sets the mode (true - or, false - and)
    void setMode(const bool& mode) { multiMode = mode; };
    
    /// Add a source event source
	/*! Returns true if successful, otherwise false. */
	bool addSource( StochasticEventGenerator * );

	/// Remove a source event source
	/*! Returns true if successful, otherwise false. */
	bool removeSource( StochasticEventGenerator * );

private:
	/// Vector with stochastic processes to collect input events from
	vector< StochasticEventGenerator *> multiSources;
    uint multiEventAmount;
    bool multiMode;
};

#endif
