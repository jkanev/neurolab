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

/// Detects whether a value is above a threshold
/*! Simple threshold detector. Receives a stochastic variable and a 
threshold value at construction time,
and has an event whenever the current value of the variable exceeds
the threshold. */

#ifndef THRESHOLD_DETECTOR_HXX
#define THRESHOLD_DETECTOR_HXX

#include <neurolab/stochastic.hxx>

class ThresholdDetector : public StochasticEventGenerator
{
private:
	StochasticVariable *detectorSource;
	double detectorThreshold;
	bool detectorBelow;
	
public:
	/// Construction
	
	ThresholdDetector( StochasticVariable *source, double threshold, class Time *time, bool below=false, const string& name="", const string& type="Event Generator" )
	: StochasticEventGenerator(time, name, type)
	{
		detectorSource = source;
		detectorThreshold = threshold;
		detectorBelow = below;
	};
	
	/// Destruction
	virtual ~ThresholdDetector() {}
	
	/// has event?
	bool hasEvent() {
		if (detectorBelow)
			return (detectorSource->getCurrentValue() < detectorThreshold);
		else
			return (detectorSource->getCurrentValue() > detectorThreshold);
	};
	
	/// get amount of events
	uint getEventAmount() {
		return hasEvent() ? 1 : 0;
	};
};

#endif
