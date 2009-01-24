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

#ifndef __IF_NEURON_HXX
#define __IF_NEURON_HXX

#include "neuron.hxx"
#include "noises.hxx"

using namespace std;

const Property NRN_THRESHOLD = 5;
const Property NRN_SPIKEHEIGHT = 6;

/// class implementing a simple integrate-and-fire neuron
/*! This class implements an integrate-and-fire neuron. The neuron can be used with conductances, synapses or simple stochastic input. It is a passive stochastic variable, therefore both step() and next() must be used. It inherits from Differentiable as well as from Event. Ito- and Stratonovitch integrals may be used (use setIto() or setStrat() from Differentiable), and can be used as a trigger for ConditionalEstimator. */

class IfNeuron : public Neuron
{
private:
	bool bSpike; // is there a spike currently?
	bool bSpikeNext; // will there be a spike after the next update?
	
protected:
	double dSpikeHeight; // height of a spike. This is cosmetical, really.
	double dTheta; // threshold
	
	/// Construct.
	/*! Default constructor only to be used by derived classes. */
	// IfNeuron::IfNeuron():Neuron(0, 0.0, 0.0){};
	
public:
	/// construct
	/*! Creates a neuron. */
	IfNeuron(
		const Time *time, ///< Address of global ime object
		double v0, ///< Reset potential
		double theta, ///< Threshold potential
		double spikeheight, ///< Hight of a spike in mV
		double tau, ///< Membrane time constant
		double v_rest ///< Resting potential
	);
	
	/// destruct
	~IfNeuron();
	
	/// Calibrate the neuron
	/*! Set the neuron to a specific reponse rate. This is achieved by internally adjusting the threshold.
	If you do not give a NoiseSource object, all inputs to the neuron must be active for this to work. If the neuron receives passive processes (like correlated noise from a NoiseSource), you *must* give the address of the NoiseSource object driving them. */
	void calibrate(
		int isi, ///< Desired inter-spike interval, given in time steps.
		int runs, ///< Number of runs used to estimate threshold. 50 is a good compromise.
		int maxtime, ///< Maximum time for one run to take. 2*isi is a good compromise
		NoiseSource *noises = 0 ///< Address of a NoiseSource object.
	);
	
	/// Get a parameter
	virtual double dParameter(const Property &);
	
	/// Set a parameter
	virtual void setParameter(const Property &, double);
	
	bool bEvent();
	
	/// step
	virtual void next();
	virtual void step();
};

#endif
