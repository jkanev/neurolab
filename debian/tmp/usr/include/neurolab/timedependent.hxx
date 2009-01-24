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

#include "physical.hxx"

/// just returns dt
class Time: public Physical
{
public:
	double dt;
	
	Time(double timestep) {
		dt = timestep; 
		uPhysicalUnit.set(0, 0,0,1,0,0,0,0); // ms
		physicalDescription = "time";
	};
	
	virtual ~Time(){};
	
	virtual string getPhysicalName() {
		return "time";
	};
	
/*	double dParameter( const Property& p ) {
		if(p==TIME_DT)
			return dt;
		else
			return StochVariable::dParameter(p);
	}*/
};

/// Time dependent objects.
/*! All objects which depend on time, especially on the size of the simulation  time, must derive from this class. */
class TimeDependent
{
private:
	bool bTimeExternal;
	
protected:
	const class Time *xTime;
public:
	/// Construct.
	/*! \param  time Time object on which this object is dependent
	If 0 is given here, a new time object with time step dt=1.0 will be created. In this case the object will be independent of all changes made to the time settings otherwise in the program. This must only be used for objects, which are not dependent on the current time settings, but inherit from this class anyway (some special forms of Stochastic variables, like Product). */
	TimeDependent( const class Time *time ) {
		xTime = time;
		bTimeExternal = true;
		if(!xTime) {
			xTime = new class Time(1.0);
			bTimeExternal = false;
			cout << "creating time independent object" << endl;
		}
	};
	
	~TimeDependent(){
		if( !bTimeExternal && xTime )
			delete xTime;
		};
	const class Time *getTime() const { return xTime; };
};

#endif
