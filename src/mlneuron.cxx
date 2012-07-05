
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

#include "../h/mlneuron.hxx"
#include "../h/processes.hxx"
#include "../h/matrix.hxx"

MlCalciumChannel::MlCalciumChannel(double weight, double reversal, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	physicalUnit = Unit("m","V");
	addParameter("reversal-potential");
	addParameter("weight");
}

string MlCalciumChannel::getParameter(const string& name) const
{
	stringstream param;
/*	
	if (name=="reversal-potential") 
		param << dReversal;
	else if (name=="weight")
		param << dWeight;
	else
		param << StochasticFunction::getParameter(name);
*/		
	return param.str();
}

void MlCalciumChannel::setParameter(const string& name, const string& value)
{
}

double MlCalciumChannel::calculateCurrentValue()
{
	return 0;
}

double MlCalciumChannel::calculateNextValue()
{
	return 0;
}


MlPotassiumChannel::MlPotassiumChannel(double weight, double reversal, const string& name, const string& type)
	: StochasticFunction(0, name, type)
{
	physicalUnit = Unit("m","V");
	addParameter("reversal-potential");
	addParameter("weight");
}

string MlPotassiumChannel::getParameter(const string& name) const
{
	stringstream param;

	return param.str();
}

void MlPotassiumChannel::setParameter(const string& name, const string& value)
{
}

double MlPotassiumChannel::calculateCurrentValue()
{
	return 0;
}

double MlPotassiumChannel::calculateNextValue()
{
	return 0;
}


MlNeuron::MlNeuron(Time *time, double v_0, double theta, double spikeheight, double tau, double v_rest, const string& name, const string& type)
	: SpikingNeuron(time, name, type), mlneuronMembrane(time, v_0, 0.0, "V")
{
	// parameter accessors
	addParameter("threshold");
	addParameter("spike-height");
	addParameter("membrane");
	
	// write descriptions
	physicalDescription = "voltage";
	stochDescription = "LIF Neuron membrane";
	
	// assign values
	mlneuronHasSpikeNow = false;
	mlneuronHasSpikeNext = false;
	mlneuronTheta = theta;	
	mlneuronMembrane.setUnit( Unit("m","V") * Unit("u","F") );
	physicalUnit = Unit("m","V");
	
	// add decay terms
	mlneuronMembrane.addTerm( new VoltageDependance(1.0/tau, v_rest, "leak"), new TimeProcess(time) );
	mlneuronMembrane.addTerm( new MlCalciumChannel(0.0, 0.0, ""), new TimeProcess(time) );
	mlneuronMembrane.addTerm( new MlPotassiumChannel(0.0, 0.0, ""), new TimeProcess(time) );
}

MlNeuron::~MlNeuron()
{}

void MlNeuron::prepareNextState()
{
}

void MlNeuron::proceedToNextState()
{
	SpikingNeuron::proceedToNextState();
}

bool MlNeuron::hasEvent()
{
	return false;
}

void MlNeuron::calibrate(int isi, int spikes, int maxtime, NoiseSource *noises)
{
/*	double stepsize = 1.0; // starting stepsize
	int steps = 20; // multiply stepsize with this
	ifneuronTheta = mlneuronMembrane.getStartingValue();
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
*/}

string MlNeuron::getParameter(const string& name) const
{
	return "";
}

void MlNeuron::setParameter(const string& name, const string& value)
{
}

int MlNeuron::addStimulus( StochasticVariable *stochvar )
{
	return 1;
}

int MlNeuron::addStimulus( StochasticVariable *stochvar, double weight )
{
	return 1;
}

int MlNeuron::addStimulus( StochasticVariable *stochvar, double weight, double revPot )
{
	return 1;
}

int MlNeuron::addStimulus( Synapse *synapse )
{
	return 1;
}

void MlNeuron::removeStimulus( int n)
{
}
