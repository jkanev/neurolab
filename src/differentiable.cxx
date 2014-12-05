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

#include "../h/differentiable.hxx"


//______________________________________________________________
//
//  create / destroy
//

DifferentialEquation::DifferentialEquation(double x0, double dx0, const string& name, const string& type)
	: StochasticVariable(0, name, type)
{
	eqnX0 = x0;
	eqnTermAmount = 0;
	eqnIntegrands = vector<StochasticFunction *>();
	eqnTermAmount = 0;
	eqnIntegrators = vector<StochasticVariable *>();
	eqnMethodPtr = &DifferentialEquation::stepEulerIto;
	stochCurrentValue = stochNextValue = x0;
	eqnIncrement = dx0;
	addParameter("equation");
	addParameter("mode");
	addParameter("starting-value");
}

DifferentialEquation::DifferentialEquation(Time *time, double x0, double dx0, const string& name, const string& type)
	: StochasticVariable(time, name, type)
{
	eqnX0 = x0;
	eqnTermAmount = 0;
	eqnIntegrands = vector<StochasticFunction *>();
	eqnTermAmount = 0;
	eqnIntegrators = vector<StochasticVariable *>();
	eqnMethodPtr = &DifferentialEquation::stepEulerIto;
	stochCurrentValue = stochNextValue = x0;
	eqnIncrement = dx0;
	addParameter("equation");
	addParameter("mode");
	addParameter("starting-value");
}

DifferentialEquation::~DifferentialEquation()
{
}

//____________________________________________________________________________
//
//  setting the function
//

// append new integrand
int DifferentialEquation::addTerm(StochasticFunction *integrand, StochasticVariable *integrator)
{
	eqnIntegrands.push_back(integrand);
	eqnIntegrators.push_back(integrator);
	if( !eqnTermAmount )
		setUnit( integrand->getUnit() * integrator->getUnit() );
	else
		integrand->getUnit() * integrator->getUnit();
	eqnTermAmount = eqnIntegrands.size();
	
	// add parameter for new integrand
	stringstream param1;
	param1 << "f" << eqnTermAmount << "(" << getName() << ")";
	addParameter( param1.str() );
	
	// add parameter for new integrator
	stringstream param2;
	param2 << "X" << eqnTermAmount;
	addParameter( param2.str() );
	
	return eqnTermAmount-1;
}

// remove integrand n
void DifferentialEquation::rmTerm(int n)
{
	if(n<eqnTermAmount) {
		eqnIntegrands.erase(eqnIntegrands.begin()+n);
		eqnIntegrators.erase(eqnIntegrators.begin()+n);
		
		// add parameter for new integrand
		stringstream param1;
		param1 << "f" << eqnTermAmount << "(" << getName() << ")";
		rmParameter( param1.str() );
		
		// add parameter for new integrator
		stringstream param2;
		param2 << "X" << eqnTermAmount;
		rmParameter( param2.str() );
			
		eqnTermAmount = eqnIntegrands.size();
	}
}

// set integrand
void DifferentialEquation::setTerm(int n, StochasticFunction *integrand, StochasticVariable *integrator)
{
	if(n<eqnTermAmount) {
		delete eqnIntegrands[n];
		eqnIntegrands[n] = integrand;
	}
	if(n<eqnTermAmount) {
		delete eqnIntegrators[n];
		eqnIntegrators[n] = integrator;
	}
	physicalUnit += integrand->getUnit() * integrator->getUnit();
}

// get integrand
StochasticFunction *DifferentialEquation::getIntegrand(int n)
{
	if(n<eqnTermAmount)
		return eqnIntegrands[n];
	else {
		cout << "out of range" << endl;
		return 0;
	}
}

// get integrator
StochasticVariable *DifferentialEquation::getIntegrator(int n)
{
	if(n<eqnTermAmount)
		return eqnIntegrators[n];
	else {
		cout << "out of range" << endl;
		return 0;
	}
}


//______________________________________________________________
//
//  set current value
//    spc: init( x0 )
//    pre: x0 = starting value
//   post: Xt(i+1)=Xt(i)=x0

void DifferentialEquation::init(double x0)
{
	eqnX0 = x0;
	stochCurrentValue = stochNextValue = eqnX0;
	eqnIncrement = 0.0;
}

void DifferentialEquation::init()
{
	stochCurrentValue = stochNextValue = eqnX0;
	eqnIncrement = 0.0;
}

double DifferentialEquation::getStartingValue() const
{
	return eqnX0;
}

//______________________________________________________________
//
//  get current values according to the current int. mode
//

void DifferentialEquation::prepareNextState()
{
	bool allStatesPrepared = true;
	for (int i=0; i<eqnTermAmount; ++i)
		allStatesPrepared = allStatesPrepared && (*eqnIntegrators[i]).isNextStatePrepared();
	
	if (allStatesPrepared) {
		(this->*eqnMethodPtr)();
		stochNextStateIsPrepared = true;
	}
	else
		stochNextStateIsPrepared = false;
}

void DifferentialEquation::stepEulerIto()
{
	// cout << "Ito method" << endl;
	
	// calculate new diff
	eqnIncrement = 0.0;
	for(int i=0; i<eqnTermAmount; ++i) {
		double integrand = (*eqnIntegrands[i])(stochCurrentValue);
		double integrator = (*eqnIntegrators[i]).d();
		eqnIncrement +=  integrand * integrator;
	// 	cout << "integrand " << i << ": " << integrand
	// 		<< " integrator " << i << ": " << integrator << endl;
	}
	
	stochNextValue = stochCurrentValue + eqnIncrement;
	// cout << "DifferentialEquation::stochNextValue = " << stochNextValue << endl;
}

void DifferentialEquation::stepEulerStratonovich()
{
	// cout << "Stratonovich method" << endl;
	
	double integrand = 0.0;
	double integrator = 0.0;
			
	eqnIncrement = 0.0;
	for(int i=0; i<eqnTermAmount; i++) {
		// ito part
		integrand = (*eqnIntegrands[i])( stochCurrentValue );
		integrator = (*eqnIntegrators[i]).d();
		eqnIncrement +=  integrand * integrator;
		// cout << "integrand (std)" << i << ": " << integrand
		// 	<< " integrator (std)" << i << ": " << integrator << endl;
	}
	stochNextValue = stochCurrentValue + eqnIncrement; // start for 1-step fixed point iteration
	
	for( int n=0; n<2; n++ ) {	
		double drift = 0.0;
		for(int i=0; i<eqnTermAmount; i++) {
			integrand = eqnIntegrands[i]->getIncrement( stochNextValue );
			integrator = eqnIntegrators[i]->getIncrement();
			// stratonovitch drift
			drift += integrand * integrator;
			// cout << "integrand (drift)" << i << ": " << integrand
			//	<< " integrator (drift)" << i << ": " << integrator << endl;
		}
		stochNextValue = stochCurrentValue + eqnIncrement + 0.5*drift;
	}
	// cout << "DifferentialEquation::stochNextValue = " << stochNextValue << endl;
}


//______________________________________________________________
//  set and get current integrating mode

bool DifferentialEquation::isIto() const
{
	if (eqnMethodPtr == &DifferentialEquation::stepEulerIto)
		return true;
	else return false;
}

bool DifferentialEquation::isStratonovich() const
{
	if (eqnMethodPtr == &DifferentialEquation::stepEulerStratonovich)
		return true;
	else return false;
}

void DifferentialEquation::setIto()
{
	eqnMethodPtr = &DifferentialEquation::stepEulerIto;
}

void DifferentialEquation::setStratonovich()
{
	eqnMethodPtr = &DifferentialEquation::stepEulerStratonovich;
}


//______________________________________________________________
//  parameter access

string DifferentialEquation::getParameter( const string& name ) const
{
	stringstream param;
	
	// formula
	if (name=="equation") {
		param << "d " << getName() << " = ";
		for (int i=0; i<eqnTermAmount; ++i) {
			if (i) param << " + ";
			param << "f" << i+1 << "(" << getName() << ") ";
			param << "d X" << i+1 << " ";
		}
	}
	
	// first value
	else if (name=="starting-value")
		param << getStartingValue();
	
	// mode
	else if (name=="mode")
		param << (isIto() ? "ito" : "stratonovitch");
	
	// parameters for single terms
	else {
		
		// parameters for integrands
		for (uint i=0; i<eqnIntegrands.size(); ++i) {
			stringstream term;
			term << "f" << i+1 << "(" << getName() << ")";
			if (name == term.str()) {
				param << eqnIntegrands[i]->getConfiguration();
				break;
			}
		}
			
		// parameters for integrators
		for (uint i=0; i<eqnIntegrands.size(); ++i) {
			stringstream term;
			term << "X" << i+1;
			if (name == term.str()) {
				param << "\"" << eqnIntegrators[i]->getName() << "\" " << eqnIntegrators[i]->getType();
				break;
			}
		}
	}
	
	return param.str();
}


//______________________________________________________________
//  parameter access

void DifferentialEquation::setParameter( const string& name, const string& value )
{
	stringstream setting;
	
	// first value
	if (name=="starting-value") {
		double d;
		setting << value;
		setting >> d;
		init(d);
	}
	
	// mode
	else if (name=="mode") {
		if (value=="ito")
			setIto();
		if (value=="stratonovitch")
			setStratonovich();
	}
}
