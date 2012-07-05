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

#include "../h/ifneuron.hxx"
#include "../h/processes.hxx"
#include "../h/matrix.hxx"

IfNeuron::IfNeuron(Time *time, double v_0, double theta, double spikeheight, double tau, double v_rest, const string& name, const string& type)
	: SpikingNeuron(time, name, type), ifneuronMembrane(time, v_0, 0.0, "V")
{
	// parameter accessors
	addParameter("threshold");
	addParameter("spike-height");
	addParameter("membrane");
	
	// write descriptions
	physicalDescription = "voltage";
	stochDescription = "LIF Neuron membrane";
	
	// assign values
	ifneuronSpikeHeight = spikeheight;
	ifneuronHasSpikeNow = false;
	ifneuronHasSpikeNext = false;
	ifneuronTheta = theta;	
	ifneuronMembrane.setUnit( Unit("m","V") * Unit("u","F") );
	physicalUnit = Unit("m","V");
	
	// add decay terms
	ifneuronMembrane.addTerm( new VoltageDependance(1.0/tau, v_rest, "leak"), new TimeProcess(xTime) );
}

IfNeuron::~IfNeuron()
{}

void IfNeuron::prepareNextState()
{
	// create next value of differential equation
	ifneuronMembrane.prepareNextState();
	stochNextStateIsPrepared = ifneuronMembrane.isNextStatePrepared();
	stochNextValue = ifneuronMembrane.getNextValue();
	
	// proceed if calculation was successful
	if (stochNextStateIsPrepared) {
		if (ifneuronHasSpikeNext) {
			double d = stochCurrentValue; // == dSpikeHeight
			ifneuronMembrane.init();
			stochCurrentValue = d;
			ifneuronHasSpikeNext = false;
		} else {
			if (stochNextValue>ifneuronTheta) {
				stochNextValue = ifneuronSpikeHeight;
				ifneuronHasSpikeNext = true;
			}
		}
	}
}

void IfNeuron::proceedToNextState()
{
	SpikingNeuron::proceedToNextState();
	ifneuronMembrane.proceedToNextState();
	ifneuronHasSpikeNow = ifneuronHasSpikeNext;
}

bool IfNeuron::hasEvent()
{
	return ifneuronHasSpikeNow;
}

void IfNeuron::calibrate(int isi, int spikes, int maxtime, NoiseSource *noises)
{
	double stepsize = 1.0; // starting stepsize
	int steps = 20; // multiply stepsize with this
	ifneuronTheta = ifneuronMembrane.getStartingValue();
	int direction = 1; // current direction 1-up, -1-down

	// new interval estimator, measuring time in units
	IntervalEstimator estimator(EST_MEAN, this, xTime);
	
	NullStream devnull;
	
	for( int k=0; k<1000; k++) {
		
		// test fpt
		xTime->run( spikes, this, maxtime, devnull );
		
		// adjust threshold
		double mean = estimator.mResult(EST_MEAN).to_d() / xTime->dt;
		if( mean==0.0 ) mean = 2*isi;
		if(mean < isi)
			if (direction == 1) {
				// mean too large and direction up - decrease stepsize and turn
				stepsize *= 0.8;
				direction *= -1;
			} else
				// mean too large and direction down - increase stepsize and go on
				stepsize *= 1.1;
		else
			if (direction == -1) {
				// mean too small and direction down - decrease stepsize and turn
				stepsize *= 0.8;
				direction *= -1;
			} else
				// mean too small and direction up - increase stepsize and go on
				stepsize *= 1.1;
		ifneuronTheta -= direction * stepsize * steps;
		
		// note to the user
		cout << "\rerror: " << stepsize << " threshold: " << ifneuronTheta << " spike rate: " << 1.0/(mean*xTime->dt) << "Hz      " << flush;
		
		// early stopping
		if( stepsize<0.001 ) {
			cout << "\rcalibrated neuron after " << k << " steps.\t\t\t\t\t" << endl;
			break;
		}
	}
	
	init();
}

string IfNeuron::getParameter(const string& name) const
{
	stringstream param;
	if (name == "threshold")
		param << ifneuronTheta;
	else if (name=="spike-height")
		param << ifneuronSpikeHeight;
	else if (name=="membrane")
		param << ifneuronMembrane.getConfiguration();
	else
		return SpikingNeuron::getParameter(name);
		
	return param.str();
}

void IfNeuron::setParameter(const string& name, const string& value)
{
	stringstream setting;
	if (name == "threshold") {
		setting << value;
		setting >> ifneuronTheta;
		cout << "setting threshold to " << ifneuronTheta << endl;
	}
	else if (name == "spike-height") {
		setting << value;
		setting >> ifneuronSpikeHeight;
		cout << "setting spike height to " << ifneuronSpikeHeight << endl;
	}
	else if (name== "membrane") {
		// !! include boost regex library and take parameter apart here !!
		ifneuronMembrane.setParameter(name, value);
	}
	else
		SpikingNeuron::setParameter(name, value);
}

int IfNeuron::addStimulus( StochasticVariable *stochvar )
{
	return ifneuronMembrane.addTerm( new Scalar( 1.0, "1.0" ), stochvar );
}

int IfNeuron::addStimulus( StochasticVariable *stochvar, double weight )
{
	stringstream name;
	if (weight > 0)
		name << "excitatory";
	else if (weight < 0)
		name << "inhibitory";
	else
		name << "shunting";
	
	return ifneuronMembrane.addTerm( new Scalar( weight, name.str() ), stochvar );
}

int IfNeuron::addStimulus( StochasticVariable *stochvar, double weight, double revPot )
{
	return ifneuronMembrane.addTerm( new VoltageDependance(weight, revPot, "channel"), stochvar );
}

int IfNeuron::addStimulus( Synapse *synapse )
{
	return ifneuronMembrane.addTerm( synapse, new TimeProcess(synapse->getTime()) );
}

void IfNeuron::removeStimulus( int n)
{
	ifneuronMembrane.rmTerm(n);
}
