/* Copyright Information
__________________________________________________________________________

Copyright (C) 2011 Jacob Kanev

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

#include "stochastic.hxx"

#ifndef __TIME_DELAY_HXX
#define __TIME_DELAY_HXX

using namespace std;
/// Class implementing time delay
/** A stochastic variable that takes the same value as a given source variable
 * with a configured amount of time steps delay */
template<class T>
class TimeDelay : public T
{
	private:
		T *delaySource;
		Ring<double> delayData;
		
	public:
		
		/// for base classes like construct( time* )
		TimeDelay( T *source, uint samplesDelay, Time *time )
		: T( time, "time delayed "+source->getName(), "Time Delay" ),
		delayData(samplesDelay+1)
		{
			delaySource = source;
		}
		
		/// for base classes like construct( double, time* )
		TimeDelay( T *source, double param1, uint samplesDelay, Time *time )
		: T( param1, time, "time delayed "+source->getName(), "Time Delay" ),
		delayData(samplesDelay+1)
		{
			delaySource = source;
		}
		
		virtual void prepareNextState()
		{
			if (delaySource->isNextStatePrepared() ) {
				delayData.next( delaySource->getNextValue() );
				T::stochNextValue = delayData[1];
				T::stochNextStateIsPrepared = true;
			}
		}
};

#endif
