
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

#include "../h/eventmultiplexer.hxx"

// Construct
EventMultiplexer::EventMultiplexer(Time* time, const bool& mode, const string& name, const string& type)
	: StochasticEventGenerator(time, name, type)
{
    multiMode = mode;
}

// add source
bool EventMultiplexer::addSource( StochasticEventGenerator *source )
{
	uint sizeBefore = multiSources.size();
	multiSources.push_back(source);

	return multiSources.size() == sizeBefore+1;
}

// remove source
bool EventMultiplexer::removeSource( StochasticEventGenerator *source )
{
	bool success = false;
	for (uint i=0; i<multiSources.size(); i++) {
		if (multiSources[i] == source) {
			success = true;
			multiSources[i] = 0;
		}
	}
	
	return success;
}

// prepare next state
void EventMultiplexer::prepareNextState()
{
    if (!stochNextStateIsPrepared) {
        stochNextValue = stochCurrentValue;
        eventNextValue = false;
        if (multiMode) {
            for (uint i=0; i<multiSources.size(); i++) {
                if (multiSources[i] && multiSources[i]->hasEvent()) {
                    stochNextValue += 1.0;
                    eventNextValue = true;
                }
            }
        } else {
            uint amount = 0;
            for (uint i=0; i<multiSources.size(); i++) {
                if (multiSources[i] && multiSources[i]->hasEvent())
                    ++amount;
            }
            if (amount == multiSources.size()) {
                stochNextValue += 1.0;
                eventNextValue = true;
            }
        }
        stochNextStateIsPrepared = true;
    }
}

// proceed one time steop
void EventMultiplexer::proceedToNextState()
{
    if (stochNextStateIsPrepared) {
        multiEventAmount = uint(stochNextValue - stochCurrentValue);
        stochCurrentValue = stochNextValue;
        eventCurrentValue = eventNextValue;
        stochNextStateIsPrepared = false;
    }
}


// get number of events
uint EventMultiplexer::getEventAmount()
{
	return multiEventAmount;
}


