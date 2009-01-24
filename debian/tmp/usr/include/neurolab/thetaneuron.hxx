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

#ifndef __THETA_NEURON
#define __THETA_NEURON

#include "ifneuron.hxx"
#include "processes.hxx"

using namespace std;

/// Theta neuron membranedecay  term.
/*! Implements the function 1 - cos(x) */
class ThetaMembrane: public StochFunction
{
public:
	ThetaMembrane(const Time *time) : StochFunction(time) {};
	~ThetaMembrane(){};
	void next() {
		dNextValue = 1.0 - cos(dXt);
		uPhysicalUnit /= Unit("m","s");
	}
};

/// Theta neuron stimulus term.
/*! Implements the function 1 + cos(x) */
class ThetaStimulus: public StochFunction
{
public:
	ThetaStimulus(const Time *time) : StochFunction(time) {};
	~ThetaStimulus(){};
	void next() {
		dNextValue = 1.0 + cos(dXt);
	};
};

/// A Theta Neuron.
/*! This class implements a theta neuron. The differential equation for the theta neuron has the following form: \f[ d\Theta_t = (1-\cos\Theta_t)dt + (1+\cos\Theta_t)dG_t, \f] where \f[dG_t = \mu + \sigma dW_t.\f] */
class ThetaNeuron: public Neuron
{
private:
	Time *xTime;
	bool bSpike;
	
public:
	/// Construct.
	ThetaNeuron(Time *time);
	
	/// Destruct.
	~ThetaNeuron();
	
	/// Add stimulus.
	void addStimulus(StochVariable *integrator);
	
	/// Next step
	virtual void next();
	
	/// Event?
	virtual bool bEvent();
};

#endif
