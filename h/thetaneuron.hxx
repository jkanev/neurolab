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
/** Implements the function 1 - cos(x) */
class ThetaMembrane: public StochasticFunction
{
public:
	ThetaMembrane(Time *time, const string& name="", const string& type="Theta Membrane") : StochasticFunction(time, name, type) {};
	~ThetaMembrane(){};
	
	/// Return current value.
	double calculateCurrentValue() {
		return 1.0 - cos(stochCurrentValue);
	}
	
	/// Return next value.
	double calculateNextValue() {
		return 1.0 - cos(stochNextValue);
	}
};

/// Theta neuron stimulus term.
/** Implements the function 1 + cos(x) */
class ThetaStimulus: public StochasticFunction
{
public:
	ThetaStimulus(Time *time, const string& name="", const string& type="Theta Stimulus") : StochasticFunction(time, name, type) {};
	~ThetaStimulus(){};
	
	/// Return current value.
	double calculateCurrentValue() {
		return 1.0 + cos(stochCurrentValue);
	}
	
	/// Return next value.
	double calculateNextValue() {
		return 1.0 + cos(stochNextValue);
	}
};

/// A Theta Neuron.
/** This class implements a theta neuron. The differential equation for the theta neuron has the following form:
\f[ d\Theta_t = (1-\cos\Theta_t)dt + (1+\cos\Theta_t)dG_t, \f]
where
\f[dG_t = \mu + \sigma dW_t.\f] */

class ThetaNeuron: public SpikingNeuron
{
private:
	DifferentialEquation thetaMembrane;	

public:
	/// Construct.
	ThetaNeuron(Time *time, const string& name="", const string& type="Theta Neuron");
	
	/// Destruct.
	~ThetaNeuron();
	
	/// Add stimulus.
	void addStimulus(StochasticVariable *integrator);
	
	/// Next step
	virtual void prepareNextState();
};

#endif
