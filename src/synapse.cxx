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

#include "../h/synapse.hxx"
#include "../h/processes.hxx"
#include <sstream>


Synapse::Synapse(Time *time, StochasticEventGenerator *e, double weight, double revPot, const string& name, const string& type)
	: StochasticFunction(time, name, type)
{
	dWeight = weight;
	dRevPot = revPot;
	stochCurrentValue = 0.0;
	stochNextValue = 0.0;
	simplePreNeuron = e;
	physicalUnit = Unit("m","V");
	stochDescription = "synapse";
	addParameter( "reversal-potential" ); 
	addParameter( "weight" );
	addParameter( "pre-synaptic-neuron" );
}


SimpleSynapse::SimpleSynapse(Time *time, StochasticEventGenerator *e, double weight, double revPot, double peak, double tau)
	: Synapse(time, e, weight, revPot), differential(time, peak, 0.0)
{
	simplePreNeuron = e;
	simplePeakCnd = peak;
	simpleTimeConstant = tau;
	differential.addTerm(new Product(-1.0/simpleTimeConstant), new TimeProcess(time));
	physicalUnit = Unit("m","V") * Unit("m","S") * Unit("", "s");
	addParameter("time-constant");
	addParameter("peak-conductance");
}

SimpleSynapse::SimpleSynapse(Time *time, StochasticVariable *noiseSource, double weight, double revPot, double tau)
	: Synapse(time, 0, weight, revPot), differential(time, 0.0, 0.0)
{
	simplePreNeuron = 0;
	simplePeakCnd = 0.0;
	simpleTimeConstant = tau;
	differential.addTerm(new Product(-1.0/simpleTimeConstant, "X(t)/tau"), new TimeProcess(time));
	differential.addTerm(new Scalar(1.0, "1.0"), noiseSource);
	physicalUnit = Unit("m","V") * Unit("m","S") * Unit("", "s");
	addParameter("time-constant");
	addParameter("peak-conductance");
}


Synapse::~Synapse()
{
}

SimpleSynapse::~SimpleSynapse()
{
}

double Synapse::calculateCurrentValue()
{
	if( simplePreNeuron->hasEvent() )
		return dWeight * (dRevPot - stochCurrentValue) / xTime->dt;
	else
		return 0.0;
}

double Synapse::calculateNextValue()
{
	if( simplePreNeuron->hasEvent() )
		return dWeight * (dRevPot - stochNextValue) / xTime->dt;
	else
		return 0.0;
}

void SimpleSynapse::proceedToNextState()
{
	stochCurrentValue = stochNextValue;
	differential.proceedToNextState();
}

void SimpleSynapse::prepareNextState()
{
	if( simplePreNeuron && simplePreNeuron->hasEvent() ) {
		differential.init();
		stochNextStateIsPrepared = true;
	} else {
		differential.prepareNextState();
		stochNextStateIsPrepared = differential.isNextStatePrepared();
	}
}

double SimpleSynapse::calculateCurrentValue()
{
	return dWeight * (dRevPot - stochCurrentValue) * differential.getCurrentValue();
}

double SimpleSynapse::calculateNextValue()
{
	return dWeight * (dRevPot - stochNextValue) * differential.getNextValue();
}


void Synapse::setParameter( const string& name, const string& value )
{
	stringstream param;
	param << value;
	if (name == "reversal-potential")
		param >> dRevPot;
	else if (name == "weight")
		param >> dWeight;
	else
		StochasticFunction::setParameter(name, value);
}


string Synapse::getParameter( const string& name ) const
{
	stringstream param;
	if (name == "reversal-potential")
		param << dRevPot;
	else if (name == "weight")
		param << dWeight;
	else if (name == "pre-synaptic-neuron")
		param << simplePreNeuron->getName() + ": "  + simplePreNeuron->getType();
	else
		param << StochasticFunction::getParameter(name) const;
		
	return param.str();
}

void SimpleSynapse::setParameter( const string& name, const string& value )
{
	stringstream param;
	param << value;
	if (name == "time-constant")
		param >> simpleTimeConstant;
	else if (name == "peak-conductance")
		param >>simplePeakCnd;
	else
		Synapse::setParameter(name, value);
}


string SimpleSynapse::getParameter( const string& name ) const {
	stringstream param;
	if (name == "time-constant")
		param << simpleTimeConstant;
	else if (name == "peak-conductance")
		param << simplePeakCnd;
	else
		return Synapse::getParameter(name) const;
	
	return param.str();
}
