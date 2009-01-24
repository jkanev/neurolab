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

#include "../h/thetaneuron.hxx"
//////////////////////////////////////////////////
//   construct
ThetaNeuron::ThetaNeuron(Time *time, const string& name, const string& type)
	: SpikingNeuron(time, name, type), thetaMembrane(time, 3.1415926535, 0.0)
{
	physicalDescription = "voltage";
	stochDescription = "theta neuron";
	thetaMembrane.addTerm( new ThetaMembrane(xTime), new TimeProcess(xTime) );
}

//////////////////////////////////////////////////
//   destruct
ThetaNeuron::~ThetaNeuron()
{}

//////////////////////////////////////////////////
//   add stimulus
void ThetaNeuron::addStimulus(StochasticVariable *dxt)
{
	thetaMembrane.addTerm( new ThetaStimulus(xTime), dxt );
}

//////////////////////////////////////////////////
//   next value
void ThetaNeuron::prepareNextState()
{
	if(thetaSpike) {
		thetaSpike = false;
	} else {
		thetaMembrane.prepareNextState();
		if ( thetaMembrane.getNextValue() >3.1415926535 ) {
			thetaMembrane.setNextValue( -3.1415926535 );
			thetaSpike = true;
		}
		if ( thetaMembrane.getNextValue() < -3.1415926535 ) {
			thetaMembrane.setNextValue( 3.1415926535 );
			thetaSpike = true;
		}
	}
}

//////////////////////////////////////////////////
//   event?
bool ThetaNeuron::hasEvent()
{
	return thetaSpike;
}
