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
/*! This class implements functions common to all neurons - adding and removing of stimuli. A neuron is implemented as differential equation and inherits from Differentiable. The neuron can be used with conductances, synapses or simple stochastic input. This class is meant as base class for neurons; the constructor is protected. */

class Neuron : public Differentiable, public Event
{
protected:
	/// Construct.
	/*! This only calls the Differentiable constructor. */
	Neuron(const Time *time, double v0, double dv0);

public:
	/// add a stimulus
	/*! This adds a stimulus without a reversal potential, i.e. the value is added purely onto the membrane potential. The variable must have the unit mA * ms. */
	int addStimulus( StochVariable *integrator );
	
	/// add a stimulus
	/*! This adds a stimulus without a reversal potential, but the value is multiplied by the given weight before being added onto the membrane potential. The weight must be a dimensionless scalar, and the integrator must have the unit mA ** ms. */
	int addStimulus( StochVariable *integrator, double weight );
	
	/// add a stimulus
	/*! This adds a stimulus with a reversal potential. The value is multiplied by the given weight and by the distance of the voltage from the reversal potential before being added onto the membrane potential. */
	int addStimulus( StochVariable *integrator, double weight, double revpot );

	/// Add a synapse.
	/*! \param synapse The synapse to be added.
	\returns The index of the integrator/integrand added. Use this number if you want to remove the synapse again.
	This adds a stimulus which is a synapse. The synapse should have the unit mV * mS. */
	int addStimulus( Synapse *synapse );
			
	/// remove a stimulus
	/*! This removes the stimulus number n from the differential equation */
	void removeStimulus( int n );	
};

#endif
