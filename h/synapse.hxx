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

#ifndef __SYNAPSE_HXX
#define __SYNAPSE_HXX

#include "stochastic.hxx"
#include "differentiable.hxx"

using namespace std;

/// A synapse.
/** This class implements a simple synapse. Main feature is the transformation of digital signals from an Event object (usually a neuron) to analog values of a StochasticProcess object, which then are part of the membrane equation of another neuron. Since a synapse usually connects from one single neuron to another single neuron, this class is set to be active as default, i.e. is forwarded automatically and you don't have to use proceedToNextState() and prepareNextState() inside your program. */
class Synapse: public StochasticFunction
{
private:
	Synapse(const Synapse&) : StochasticFunction(0) {}; // do not copy me!
	
protected:
	StochasticEventGenerator *simplePreNeuron;
	double dWeight;
	double dRevPot;
	
public:
	/// Construct.
	/** \param e The object providing the events (spikes), i.e. the pre-synaptic neuron.
	This will construct a simple synapse, which only has the values 1.0 (just after a pre-synaptic spike) or 0.0 (otherwise). */
	Synapse(Time *time, StochasticEventGenerator *e, double weight, double revPot, const string& name="", const string& type="synapse");
	
	/// Return current value.
	virtual double calculateCurrentValue();
	
	/// Return next value.
	virtual double calculateNextValue();
	
	/// Destruct.
	~Synapse();
	
	/// Set parameter.
	/** Sets the value of a parameter using strings. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter. */
	virtual void setParameter (
		const string& name,   ///< name of parameter
		const string& value   ///< value of parameter (used with operator<<)
	);
	
	/// Get parameter.
	/** In a derived class, override this to handle every parameter you implement. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter.  */
	virtual string getParameter (
		const string& name   ///< name of parameter
	);
};

/// A synapse.
/** This class implements a  synapse which is characterized by a differential equation. Main feature is the transformation of digital signals from an Event object (usually a neuron) to analog values of a StochasticProcess object, which then are part of the membrane equation of another neuron. It also possible to use a noise source (such as a Wiener process) as input. This depends on the constructor you use. Since a synapse usually connects from one single neuron to another single neuron, this class is set to be active as default, i.e. is forwarded automatically and you don't have to use proceedToNextState() and prepareNextState() inside your program. */
class SimpleSynapse: public Synapse
{
private:
	double simplePeakCnd;
	double simpleTimeConstant;
	
protected:
	/// Differential equation.
	/** The rate equation. */
	DifferentialEquation differential;
	
	/// Noise source
	/** only used when the noise constructor was used */
	StochasticVariable *stochastic;
	
public:	
	/// Construct.
	/** Will construct a synapse, which will set its value to the peak conductance just after the pre-synaptic spike, and decay the value according to the given time constant. At the next pre-synaptic spike the value will be set to the peak conductance again. */
	SimpleSynapse (
		Time *time,   ///< pointer to main time object
		StochasticEventGenerator *e,   ///<The object providing the events (spikes), i.e. the pre-synaptic neuron.
		double weight,   ///< Weight of the synapse.
		double revPot,   ///< Reversal potential of synapse.
		double peak,   ///< Peak conductance of synapse.
		double tau   ///< Decay time constant.
	);
	
	/// Construct.
	/** Will construct a synapse, which will set its value to the peak conductance just after the pre-synaptic spike, and decay the value according to the given time constant. At the next pre-synaptic spike the value will be set to the peak conductance again. */
	SimpleSynapse (
		Time *time,   ///< pointer to main time object
		StochasticVariable *s,   ///< The object providing the stimulus.
		double weight,   ///< Weight of the synapse.
		double revPot,   ///< Reversal potential of synapse.
		double tau   ///< Decay time constant.
	);
	
	StochasticVariable *getVoltageDependance() {
		return &differential;
	}
	
	/// Calculate next state.
	virtual void prepareNextState();
	
	/// Apply nex time step.
	virtual void proceedToNextState();
	
	/// Calculate next value.
	virtual double calculateNextValue();
	
	/// Calculate current value.
	virtual double calculateCurrentValue();
	
	/// Destruct.
	~SimpleSynapse();
	
	/// Set parameter.
	/** Sets the value of a parameter using strings. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter. */
	virtual void setParameter (
		const string& name,   ///< name of parameter
		const string& value   ///< value of parameter (used with operator<<)
	);
	
	/// Get parameter.
	/** In a derived class, override this to handle every parameter you implement. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter.  */
	virtual string getParameter (
		const string& name   ///< name of parameter
	);
};


#endif
