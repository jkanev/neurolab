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

#ifndef SPIKE_ESTIMATOR_HXX
#define SPIKE_ESTIMATOR_HXX

#include <neurolab/estimator.hxx>
#include <neurolab/stochastic.hxx>
#include <neurolab/matrix.hxx>

/// Recorder for the SPIKE distance between two sources
/** Records the Online version of the SPIKE distance (Kreuz et al., JNeurophysiol 109, 1457 (2013)).
 * Both an online sample (time course) as well as an average (1/T integral [.] dt) can be obtained.
*/

class SpikeEstimator : public Estimator
{
protected:

    uint spkALast;     ///< time steps that have passed since the last spike in source A
    uint spkBLast;     ///< time steps that have passed since the last spike in source B
    uint spkDiffAB;      ///< time steps between the last event in A and its closest event in B
    uint spkDiffBA;      ///< time steps between the last event in B and its closest event in A
    uint spkLength;     ///< the length of the estimator
    bool initA;      ///< wether the first spike in A has appeared yet
    bool initB;     ///< whether the first spike in B has appeared yet

    double spkCurrentValue;     ///< the current value of the measure

    StochasticEventGenerator *spkA; ///< source of data
    StochasticEventGenerator *spkB; ///< event triggering record action

    double *spkSample; // recording sample
    double spkOne;    // recording first moment
	
	void processCurrentSample();

    void init();

public:
	
	void collect();
	
	/// Construct Object
    /** Constructs a SpikeEstimator. If the property contains an average, the integral will be returned.*/
    SpikeEstimator (
		const Property &property, ///< a collection of things to record
        StochasticEventGenerator *A, ///< event source A
        StochasticEventGenerator *B, ///< event source B
        Time *time, ///< global time object
        uint length, ///< the size (in time samples) of the estimator
        const string& name="", ///< the name of the estimator
        const string& type="SpikeEstimator" ///< the type, defaults to "SpikeEstimator", please only use when calling from derived classes
    );
	
	/// Destroy
    ~SpikeEstimator();
	
	Matrix getSample();
	
    double getMean();

    virtual Matrix getEstimate(const Property&); ///< Return an estimation.
};

#endif
