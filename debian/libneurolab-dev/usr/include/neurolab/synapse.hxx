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

/// Synaptic properties.
const Property SYNAPSE_PEAK_CONDUCTANCE = 0;
const Property SYNAPSE_TIME_CONSTANT = 1;
const Property SYNAPSE_REVERSAL_POTENTIAL = 2;
const Property SYNAPSE_WEIGHT = 3;


/// A synapse.
/*! This class implements a simple synapse. Main feature is the transformation of digital signals from an Event object (usually a neuron) to analog values of a Stochastic object, which then are part of the membrane equation of another neuron. Since a synapse usually connects from one single neuron to another single neuron, this class is set to be active as default, i.e. is forwarded automatically and you don't have to use step() and next() inside your program. */
class Synapse: public StochFunction
{
private:
	Synapse(const Synapse&) : StochFunction(0) {}; // do not copy me!
	
protected:
	Event *pPreSynaptic;
	double dWeight;
	double dRevPot;
	
public:
	/// Construct.
	/*! \param e The object providing the events (spikes), i.e. the pre-synaptic neuron.
	This will construct a simple synapse, which only has the values 1.0 (just after a pre-synaptic spike) or 0.0 (otherwise). */
	Synapse(const Time *time, Event *e, double weight, double revPot);
	
	/// Calculate next value
	virtual void next();
	
	/// Destruct.
	~Synapse();
	
	/// Set a property.
	virtual void setParameter( const Property& p, double d );

	/// Get a property.
	virtual double dParameter( const Property& p );
};

/// A synapse.
/*! This class implements a  synapse which is characterized by a differential equation. Main feature is the transformation of digital signals from an Event object (usually a neuron) to analog values of a Stochastic object, which then are part of the membrane equation of another neuron. It also possible to use a noise source (such as a Wiener process) as input. This depends on the constructor you use. Since a synapse usually connects from one single neuron to another single neuron, this class is set to be active as default, i.e. is forwarded automatically and you don't have to use step() and next() inside your program. */
class ComplexSynapse: public Synapse
{
private:
	double dPeak;
	double dTau;
	
protected:
	/// Differential equation.
	/*! The rate equation. */
	Differentiable differential;
	
	/// Noise source
	/*! only used when the noise constructor was used */
	StochVariable *stochastic;
	
public:	
	/// Construct.
	/*! \param e The object providing the events (spikes), i.e. the pre-synaptic neuron. 
	\param peak The synapse's peak conductance.
	\param tau The decay time constant.
	\param time The time (object pointer).
	This will construct a synapse, which will set its value to the peak conductance just after the pre-synaptic spike, and decay the value according to the given time constant. At the next pre-synaptic spike the value will be set to the peak conductance again. */
	ComplexSynapse(const Time *time, Event *e, double weight, double revPot, double peak, double tau);
	
	/// Construct.
	/*! \param s The object providing the noise input (spikes), i.e. the pre-synaptic stimulus noise. 
	\param peak The synapse's peak conductance.
	\param tau The decay time constant.
	\param time The time (object pointer).
	This will construct a synapse, which will set its value to the peak conductance just after the pre-synaptic spike, and decay the value according to the given time constant. At the next pre-synaptic spike the value will be set to the peak conductance again. */
	ComplexSynapse(const Time *time, StochVariable *s, double weight, double revPot, double tau);
	
	StochVariable *getConductance() {
		return &differential;
	}
	
	/// Calculate next value
	virtual void next();
	
	/// apply nex time step
	virtual void step();
	
	/// Destruct.
	~ComplexSynapse();
	
	/// Set a property.
	virtual void setParameter( const Property& p, double d );

	/// Get a property.
	virtual double dParameter( const Property& p );
};


#endif
