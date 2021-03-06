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


#include "../h/intervalestimator.hxx"
#include <math.h>

//________________________________________________________________________
//
//  interval estimator
//
	
IntervalEstimator::IntervalEstimator(const Property& property, StochasticEventGenerator *event, Time *time)
	: ScalarEstimator( property, 0, time )
{
	xTime = time;
	nTime = -1;
	pEvent = event;
}

IntervalEstimator::~IntervalEstimator()
{}

void IntervalEstimator::collect()
{
	++nTime;     // quicker than nTime++
	if( pEvent->hasEvent() ) {
		estimate( xTime->dt * double(nTime) );
		nTime = 0;
	}
}

void IntervalEstimator::init()
{
	ScalarEstimator::init();
	nTime = -1;
}
