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

#include <neurolab/neuron.hxx>
#include <neurolab/noises.hxx>

using namespace std;

/// class implementing a simple integrate-and-fire neuron
/** This class implements an integrate-and-fire neuron. The neuron can be used with conductances, synapses or simple stochastic input. Ito and Stratonovitch integrals may be used (use setIto() or setStratonovich() from DifferentialEquation), and can be used as a trigger for ConditionalEstimator. The function is \f[ dV_t = \frac{1}{C} (v_L - V_t) g_L dt + \sum_i \frac{1}{C} w_i (v_i - V_t) dG^i_t \f] when conductances are used, or \f[ dV_t = \frac{1}{C} (v_L - V_t) g_L dt + \frac{1}{C} w dG^i_t \f] when currents are used. \f$ V_t \f$ is the membrane voltage, \f$ C \f$ is the membrane capacity which is always set to \f$ 1 \mu F \f$, \f$ v_L \f$ is the leak reversal potential, \f$ g_L \f$ is the leak conductance. \f$ w_i, v_i, dG^i \f$ are weight, reversal potential and conductance of a stimulating synapse. */

class IfNeuron : public SpikingNeuron
{
private:
	DifferentialEquation ifneuronMembrane; // the membrane equation
	
protected:
	double ifneuronSpikeHeight; // height of a spike. This is cosmetical, really.
	double ifneuronTheta; // threshold
	
	/// Construct.
	/** Default constructor only to be used by derived classes. */
	// IfNeuron::IfNeuron():Neuron(0, 0.0, 0.0){};
	
public:
	/// construct
	/** Creates a neuron. */
	IfNeuron(
		Time *time, ///< Address of global ime object
		double v0, ///< Reset potential
		double theta, ///< Threshold potential
		double spikeheight, ///< Hight of a spike in mV
		double tau, ///< Membrane time constant
		double v_rest, ///< Resting potential
		const string& name="", ///< name of object
		const string& type="Integrate-and-Fire Neuron" ///< type of object, only use this when calling from a deriving class
	);
	
	/// destruct
	~IfNeuron();
	
	/// Calibrate the neuron
	/** Set the neuron to a specific reponse rate. This is achieved by internally adjusting the threshold.
	If you do not give a NoiseSource object, all inputs to the neuron must be active for this to work. If the neuron receives passive processes (like correlated noise from a NoiseSource), you *must* give the address of the NoiseSource object driving them. */
	void calibrate(
		int isi, ///< Desired inter-spike interval, given in time steps.
		int runs, ///< Number of runs used to estimate threshold. 50 is a good compromise.
		int maxtime, ///< Maximum time for one run to take. 10*isi is a good compromise
		NoiseSource *noises = 0, ///< Address of a NoiseSource object.
		double increment = 1.1, ///< factor used for increasing step size when direction is kept
		double decrement = 0.8 ///< factor used for decreasing step size when direction is changed
	);
	
	/// Get parameter.
	/** In a derived class, override this to handle every parameter you implement. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter.  */
	virtual string getParameter (
		const string& name   ///< name of parameter
	)  const;
		
	/// Set parameter.
	/** Sets the value of a parameter using strings. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter. */
	virtual void setParameter (
		const string& name,   ///< name of parameter
		const string& value   ///< value of parameter (used with operator<<)
	);
		
	/// add a stimulus
	/** This adds a stimulus without a reversal potential, i.e. the value is added purely onto the membrane potential. The variable must have the unit mA * ms. */
	int addStimulus( StochasticVariable *integrator );
	
	/// add a stimulus
	/** This adds a stimulus without a reversal potential, but the value is multiplied by the given weight before being added onto the membrane potential. The weight must be a dimensionless scalar, and the integrator must have the unit mA ** ms. */
	int addStimulus( StochasticVariable *integrator, double weight );
	
	/// add a stimulus
	/** This adds a stimulus with a reversal potential. The value is multiplied by the given weight and by the distance of the voltage from the reversal potential before being added onto the membrane potential. */
	int addStimulus( StochasticVariable *integrator, double weight, double revpot );

	/// Add a synapse.
	/** \param synapse The synapse to be added.
	\returns The index of the integrator/integrand added. Use this number if you want to remove the synapse again.
	This adds a stimulus which is a synapse. The synapse should have the unit mV * mS. */
	virtual int addStimulus( Synapse *synapse );

	/// Remove a stimulus.
	/** This removes the nth stimulus from the differential equation */
	virtual void removeStimulus( int n );
	
	/// Return whether a spike is happening.
	virtual bool hasEvent();
	
	/// Set the next value of the process.
	/** This includes both the value of the neuron and the membrane, which is a separate object.. */
	virtual void setNextValue(double d) { stochNextValue = d; ifneuronMembrane.setNextValue(d); };
	
	/// Set the current value of the process.
	/** This includes both the value of the neuron and the membrane, which is a separate object.. */
	virtual void setCurrentValue(double d) { stochCurrentValue = d; ifneuronMembrane.setCurrentValue(d); };
	
	/// step
	virtual void prepareNextState();
	virtual void proceedToNextState();
};

#endif
