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

#include "../h/wiener.hxx"

#include <sstream>

//____________________________________________________________________________
//
//  wiener base
//

Wiener::Wiener(Time *time, const string& name, const string& type)
	: StochasticVariable( time, name, type )
{
	wienerMean = 0.0;
	wienerStdDev = 1.0;
	wienerSqrtDt = sqrt(xTime->dt);
	addParameter("mean");
	addParameter("variance");
	init();
}

void Wiener::init()
{
	stochCurrentValue = 0.0;
	prepareNextState();
}


void Wiener::prepareNextState()
{
	wienerDiff = wienerStdDev * wienerSqrtDt * dRandN() + wienerMean;
	stochNextValue = stochCurrentValue + wienerDiff;
	stochNextStateIsPrepared = true;
}

double Wiener::getDelta() 
{
	return wienerDiff;
}

void Wiener::setMean(double d)
{
	wienerMean = d;
}

void Wiener::setVariance(double d)
{
	wienerStdDev = sqrt(d);
}

void Wiener::setStdDev(double d)
{
	wienerStdDev = d;
}

double Wiener::getMean()
{
	return wienerMean;
}

double Wiener::getVariance()
{
	return wienerStdDev * wienerStdDev;
}

double Wiener::getStdDev()
{
	return wienerStdDev;
}

string Wiener::getParameter(const string& name) const
{
	stringstream param;
if (name == "mean")
		param << wienerMean / (wienerSqrtDt * wienerSqrtDt);
	else if (name == "variance")
		param << wienerStdDev*wienerStdDev;
	else
		param << StochasticVariable::getParameter(name);
		
	return param.str();
}


void Wiener::setParameter(const string& name, const string& value)
{
	// read value
	stringstream param;
	param << value;
	double d;
	param >> d;
	
	// set in class
	if (name == "mean")
		wienerMean = d * (wienerSqrtDt * wienerSqrtDt);
	else if (name == "variance")
		wienerStdDev = sqrt(d);
	else
		StochasticVariable::setParameter(name, value);
}



//____________________________________________________________________________
//
//  Condensed Poisson Process
//

WienerCpp::WienerCpp( Time *time, double w, double n, double lambda, double n_rc, double lambda_rc ) : Wiener( time ) 
{
	condW = w;
	condLambda = lambda;
	condN = n;
	condCoincWidth = n_rc;
	condCoincRate = lambda_rc;
	addParameter("number-of-processes");
	addParameter("single-weight");
	addParameter("single-rate");
	addParameter("coincidence-width");
	addParameter("coincidence-rate");
	init();
}

WienerCpp::~WienerCpp() {}

// set mean and variance
void WienerCpp::init()
{
	wienerMean = condW * condN * condLambda * wienerSqrtDt * wienerSqrtDt;
	wienerStdDev = condN * condW*condW * condLambda;
	wienerStdDev *= (1 + condCoincWidth*condCoincRate*(condN*condCoincWidth - 1));
	wienerStdDev = sqrt(wienerStdDev);
	Wiener::init();
}

string WienerCpp::getParameter(const string& name) const
{
	stringstream param;
		
	if (name == "single-weight")
		param << condW;
	else if (name == "number-of-processes")
		param << condN;
	else if (name == "single-rate")
		param << condLambda;
	else if (name == "coincidence-width")
		param << condCoincWidth;
	else if (name == "coincidence-rate")
		param << condCoincRate;
	else
		param << Wiener::getParameter( name );

	return param.str();
}


void WienerCpp::setParameter(const string& name, const string& value)
{
	stringstream param;
	param << value;
		
	if (name == "single-weight")
		param >> condW;
	else if (name == "number-of-processes")
		param >> condN;
	else if (name == "single-rate")
		param >> condLambda;
	else if (name == "coincidence-width")
		param >> condCoincWidth;
	else if (name == "coincidence-rate")
		param >> condCoincRate;
	else
		Wiener::getParameter( name );
}

