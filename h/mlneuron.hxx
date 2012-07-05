/* Copyright Information
__________________________________________________________________________

Copyright (C) 2012 Jacob Kanev

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

#ifndef __ML_NEURON_HXX
#define __ML_NEURON_HXX

#include "neuron.hxx"
#include "noises.hxx"

using namespace std;

/// class implementing a calcium channel for a Morris-Lecar
/** This class implements a Morris-Lecar neuron calcium channel \f[ (v_{Ca} - V_t) g_{Ca} M(t) \f] with \f[M(t) = 1+\exp\left(-2\frac{v_1-V_t}{v_2}\right)\f]. */

class MlCalciumChannel : public StochasticFunction
{
private:
	double calcVCa;
	double calcGCa;
	double calcV1;
	double calcV2;
public:
	/// Construct with object name.
	MlCalciumChannel(double, double, const string& name="", const string& type="Calcium Channel");
	
	/// Destroy.
	virtual ~MlCalciumChannel(){};
	
	/// Create parameter string.
	string getParameter(const string&) const;
	
	/// Set parameter from string.
	void setParameter(const string& name, const string& value);
	
	/// Generate next value.
	virtual double calculateCurrentValue();
	
	/// Generate next value.
	virtual double calculateNextValue();
};


/// class implementing a potassium channel for a Morris-Lecar
/** This class implements a Morris-Lecar potassium calcium channel \f[ (v_{K} - V_t) g_{K} N(t) \f] with \f[dN_t = \left(N_t-1-\exp\left(-2\frac{v_3-V_t}{v_4}\right)\right) \cosh \frac{V-v_3}{2v_4}\f]. */

class MlPotassiumChannel : public StochasticFunction
{
private:
	double potassVK;
	double potassGK;
	double potassV3;
	double potassV4;
	DifferentialEquation potassNt;
public:
	/// Construct with object name.
	MlPotassiumChannel(double, double, const string& name="", const string& type="Potassium Channel");
	
	/// Destroy.
	virtual ~MlPotassiumChannel(){};
	
	/// Create parameter string.
	string getParameter(const string&) const;
	
	/// Set parameter from string.
	void setParameter(const string& name, const string& value);
	
	/// Generate next value.
	virtual double calculateCurrentValue();
	
	/// Generate next value.
	virtual double calculateNextValue();
};


/// class implementing a Morris-Lecar neuron
/** This class implements a Morris-Lecar neuron. The neuron can be used with conductances, synapses or simple stochastic input. Ito and Stratonovitch integrals may be used (use setIto() or setStratonovich() from DifferentialEquation), and can be used as a trigger for ConditionalEstimator. The function is \f[ dV_t = \frac{1}{C} (v_L - V_t) g_L dt + (v_{Ca} - V_t) g_{Ca} M(t) dt + (v_{K} - V_t) g_{K} N(t) dt + ... \f] with \f[M(t) = 1+\exp\left(-2\frac{v_1-V_t}{v_2}\right)\f] \f[ dN_t = \left(N_t-1-\exp\left(-2\frac{v_3-V_t}{v_4}\right)\right) \cosh \frac{V-v_3}{2v_4}\f]. \f$ V_t \f$ is the membrane voltage, \f$ C \f$ is the membrane capacity which is always set to \f$ 1 \mu F \f$, \f$ v_L \f$ is the leak reversal potential, \f$ g_{L,K,Ca} \f$ are peak conductances of the leak, potassium and calcium channel, M(t) and N(t). \f$ w_i, v_i, dG^i \f$ are weight, reversal potential and conductance of a stimulating synapse. */

class MlNeuron : public SpikingNeuron
{
private:
	bool mlneuronHasSpikeNow; // is there a spike currently?
	bool mlneuronHasSpikeNext; // will there be a spike after the next update?
	DifferentialEquation mlneuronMembrane; // the membrane equation dV_t
	DifferentialEquation mlneuronRelaxation; // relaxation constant dN_t
	
protected:
	double mlneuronTheta; // threshold, used for detecting when a spike has occured
	
	/// Construct.
	/** Default constructor only to be used by derived classes. */
	// MlNeuron::MlNeuron():Neuron(0, 0.0, 0.0){};
	
public:
	/// construct
	/** Creates a neuron. */
	MlNeuron(
		Time *time, ///< Address of global ime object
		double v0, ///< Reset potential
		double theta, ///< Threshold potential
		double spikeheight, ///< Hight of a spike in mV
		double tau, ///< Membrane time constant
		double v_rest, ///< Resting potential
		const string& name="", ///< name of object
		const string& type="Morris-Lecar Neuron" ///< type of object, only use this when calling from a deriving class
	);
	
	/// destruct
	~MlNeuron();
	
	/// Calibrate the neuron
	/** Set the neuron to a specific reponse rate. This is achieved by internally adjusting the threshold.
	If you do not give a NoiseSource object, all inputs to the neuron must be active for this to work. If the neuron receives passive processes (like correlated noise from a NoiseSource), you *must* give the address of the NoiseSource object driving them. */
	void calibrate(
		int isi, ///< Desired inter-spike interval, given in time steps.
		int runs, ///< Number of runs used to estimate threshold. 50 is a good compromise.
		int maxtime, ///< Maximum time for one run to take. 2*isi is a good compromise
		NoiseSource *noises = 0 ///< Address of a NoiseSource object.
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
	
	/// step
	virtual void prepareNextState();
	virtual void proceedToNextState();
};

#endif
