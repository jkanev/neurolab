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

#ifndef __NEURON_HXX
#define __NEURON_HXX

#include "processes.hxx"
#include "differentiable.hxx"
#include "stochastic.hxx"
#include "intervalestimator.hxx"
#include "synapse.hxx"

using namespace std;

/// class implementing a simple integrate-and-fire neuron
/** This class implements functions common to all neurons - adding and removing of stimuli. A neuron is implemented as differential equation and inherits from DifferentialEquation. The neuron can be used with conductances, synapses or simple stochastic input. This class is meant as base class for neurons; the constructor is protected. */

class Neuron
{
protected:
	/// Construct.
	/** This only calls the DifferentialEquation constructor. */
	Neuron(string name, string type) {};

public:
	/// Add a synapse.
	/** \param synapse The synapse to be added.
	\returns The index of the integrator/integrand added. Use this number if you want to remove the synapse again.
	This adds a stimulus which is a synapse. The synapse should have the unit mV * mS. */
	virtual int addStimulus( Synapse *synapse ) = 0;
			
	/// remove a stimulus
	/** This removes the stimulus number n from the differential equation */
	virtual void removeStimulus( int n ) = 0;
};

class SpikingNeuron: public Neuron, public StochasticEventGenerator
{
	protected:
	/// Construct.
	SpikingNeuron(Time *time, string name, string type="spiking_neuron") : Neuron(name, type), StochasticEventGenerator(time, name, type) {};
};

#endif
