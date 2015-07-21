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

#include "../h/processes.hxx"

#include <cstdlib>
#include <cmath>
#include <sstream>


//////////////////////////////////////////////////////////////////
// scalar

Scalar::Scalar(double d, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	scalarValue = d;
	physicalDescription = name;
	addParameter("value");
}

Scalar::Scalar(double d, string unitPrefix, string unitSymbol, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	scalarValue = d;
	physicalDescription = name;
	physicalUnit = Unit(unitPrefix, unitSymbol);
	addParameter("value");
}

string Scalar::getParameter(const string& name) const
{
	stringstream parameter;
	if (name=="value")
		parameter << scalarValue;
	else
		parameter << StochasticFunction::getParameter(name);

	return parameter.str();
}

void Scalar::setParameter(const string& name, const string& value)
{
	if (name=="value") {
		stringstream parameter;
		parameter << value;
		parameter >> scalarValue;
	} else
		StochasticFunction::setParameter(name, value);
}

//____________________________________________________________________________
//
//  c*x
//

Product::Product(double factor, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	productFactor = factor;
	addParameter("factor");
};

double Product::calculateCurrentValue()
{
	return productFactor * stochCurrentValue;
}

double Product::calculateNextValue()
{
	return productFactor * stochNextValue;
}

string Product::getParameter(const string& name) const
{
	stringstream parameter;
	if (name=="factor")
		parameter << productFactor;
	else
		parameter << StochasticFunction::getParameter(name);

	return parameter.str();
}

void Product::setParameter(const string& name, const string& value)
{
	if (name=="factor") {
		stringstream parameter;
		parameter << value;
		parameter >> productFactor;
	} else
		StochasticFunction::setParameter(name, value);
}


//____________________________________________________________________________
//
//  a*(b - x)
//

VoltageDependance::VoltageDependance(double weight, double reversal, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	dReversal = reversal;
	dWeight = weight;
	stochCurrentValue = 0.0;
	physicalUnit = Unit("m","V");
	addParameter("reversal-potential");
	addParameter("weight");
}

string VoltageDependance::getParameter(const string& name) const
{
	stringstream param;
	
	if (name=="reversal-potential") 
		param << dReversal;
	else if (name=="weight")
		param << dWeight;
	else
		param << StochasticFunction::getParameter(name);
		
	return param.str();
}

void VoltageDependance::setParameter(const string& name, const string& value)
{
	stringstream param;
	param << value;
	
	if (name=="reversal-potential") 
		param >> dReversal;
	else if (name=="weight")
		param >> dWeight;
	else
		StochasticFunction::setParameter(name, value);
}

double VoltageDependance::calculateCurrentValue()
{
	return dWeight * (dReversal - stochCurrentValue);
}

double VoltageDependance::calculateNextValue()
{
	return dWeight * (dReversal - stochNextValue);
}


//____________________________________________________________________________
//  poisson process

Poisson::Poisson(double rate, Time* time, const string& name, const string& type)
	: StochasticEventGenerator(time, name, type)
{
   dRate = rate * xTime->dt;
   addParameter("rate");
}

Poisson::Poisson(Time* time, const string& name, const string& type)
: StochasticEventGenerator(time, name, type)
{
	dRate = 5.0 * xTime->dt;
	addParameter("rate");
}

void Poisson::prepareNextState()
{
	if (!stochNextStateIsPrepared) {
		if( dRandE() < dRate )
			stochNextValue += 1.0;
		stochNextStateIsPrepared = true;
	}
}

bool Poisson::hasEvent()
{
	return stochNextValue-stochCurrentValue>0.5 ? true : false;
}

uint Poisson::getEventAmount()
{
	return hasEvent() ? 1 : 0;
}

string Poisson::getParameter(const string& name) const
{
	stringstream parameter;
	if (name=="rate")
		parameter << dRate/xTime->dt;
	else
		parameter << StochasticEventGenerator::getParameter( name );
	
	return parameter.str();

}

void Poisson::setParameter(const string& name, const string& value)
{
	stringstream parameter;
	parameter << value;
	if (name=="rate") {
		parameter >> dRate;
		dRate *= xTime->dt;
	}
	else
		StochasticEventGenerator::setParameter( name, value );
}



//____________________________________________________________________________
//  regular process

Regular::Regular(double rate, Time *time, const string& name, const string& type)
: StochasticEventGenerator(time, name, type)
{
	regularInterval = long(1.0 / rate / xTime->dt);
	regularCount = regularInterval;
	addParameter("rate");
}

Regular::Regular(Time *time, const string& name, const string& type)
: StochasticEventGenerator(time, name, type)
{
	regularInterval = long(1.0 / 5.0 / xTime->dt);
	regularCount = regularInterval;
	addParameter("rate");
}

void Regular::prepareNextState()
{
	regularCount--;
	if (!regularCount) {
		stochNextValue += 1.0;
		regularCount = regularInterval;
	}
	stochNextStateIsPrepared = true;
}

bool Regular::hasEvent()
{
	return stochNextValue-stochCurrentValue>0.5 ? true : false;
}

uint Regular::getEventAmount()
{
	return hasEvent() ? 1 : 0;
}

string Regular::getParameter(const string& name) const
{
	stringstream parameter;
	if (name=="rate")
		parameter << double(regularInterval) * xTime->dt;
	else
		parameter << StochasticEventGenerator::getParameter( name );
	
	return parameter.str();
	
}

void Regular::setParameter(const string& name, const string& value)
{
	stringstream parameter;
	parameter << value;
	if (name=="rate") {
		double d;
		parameter >> d;
		regularInterval = long(1.0 / d / xTime->dt);
		regularCount = regularInterval;
	}
	else
		StochasticEventGenerator::setParameter( name, value );
}


