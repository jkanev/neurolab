
/* Copyright Information
 ________________________________________________________________________
 
 Copyright (C) 2016 Jacob Kanev
 
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

#include "../h/eventplayer.hxx"

// Create player that can play a file
EventPlayer::EventPlayer( Time *time, string filename, double resolution, EventFileType fileType)
	: StochasticEventGenerator(time)
{
	if (fileType==BINARY8LE) {
		
		// open file
		fstream stream( filename.c_str(), ios::in | ios::binary);
		
		// read file contents into buffer
		unsigned long int a;
		bool success = true;
		while (success) {
			success = (bool) stream.read((char *)&a, 8);
			if (success)
				playerBuffer.push_back(double(a) * resolution);
		}
	}
	else if (fileType==ASCII) {

		// open file
		fstream stream( filename.c_str(), ios::in );
		
		// read file contents into buffer
		double a;
		while (!stream.eof()) {
			stream >> a;
			playerBuffer.push_back(a * resolution);
		}
	}
}


// Copy constructor
EventPlayer::EventPlayer( const EventPlayer &ep )
	: StochasticEventGenerator(ep.xTime)
{
	playerBuffer = ep.playerBuffer;
	playerBufferPosition = ep.playerBufferPosition;
	playerCurrentValue = ep.playerCurrentValue;
	playerNextValue = ep.playerNextValue;
}


/// Proceed one time step.
/*! This method can be overridden to implement the proceeding of one time step. This makes new information available at the current time. (See also proceedToNextValue()). The default just writes stochNextValue into stochCurrentValue.  */
void EventPlayer::proceedToNextState()
{
	if (stochNextStateIsPrepared) {
		playerCurrentValue = playerNextValue;
		stochCurrentValue = stochNextValue;
        eventCurrentValue = eventNextValue;
		stochNextStateIsPrepared = false;
	}
}

// Calculate next value (preparing next step).
/*! This method should be overridden to implement the calculation of the next value, using information wich is available at the current time. */
void EventPlayer::prepareNextState()
{
	if (!stochNextStateIsPrepared) {
		playerNextValue = 0;
        eventNextValue = false;
		for (; playerBufferPosition < playerBuffer.size() && playerBuffer[playerBufferPosition] <= xTime->timePassed; playerBufferPosition++) {
			playerNextValue++;
			stochNextValue += 1.0;
            eventNextValue = true;
		}
		
		stochNextStateIsPrepared = true;
	}
}

/// Reset all time dependent values.
void EventPlayer::init()
{
	playerBufferPosition = 0;
	playerCurrentValue = 0;
	playerNextValue = 0;
	stochCurrentValue = 0.0;
	stochNextValue = 0.0;
	StochasticEventGenerator::init();
}

/// Get number of events in buffer
uint EventPlayer::getNumberEvents()
{
	return playerBuffer.size();
}

/// Returns the time point of the first event
double EventPlayer::getStartTime()
{
	return playerBuffer[0];
}

/// Returns the time point of the first event
double EventPlayer::getEndTime()
{
	return playerBuffer[ playerBuffer.size() - 1 ];
}

