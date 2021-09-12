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

#include <neurolab/ring.hxx>
#include <neurolab/matrix.hxx>
#include <neurolab/parametric.hxx>

#ifndef __ESTIMATOR_HXX
#define __ESTIMATOR_HXX

/// A type for setting properties estimators.
/** Types for use with any setProperty() or getProperty() function. Properties can be combined using OR (|). Possible values are (defined globally as const): EST_SAMPLE (take a single sample); EST_MEAN (take the mean); EST_VAR (take the variance), EST_CUR (take the curtosis). Additional properties are to set the values for distributions; these cannot be combined by OR with any other value. Use as set/getProperty( Property, double), where the Property can be EST_DIST_MIN (the lowest point of the distribution range), EST_DIST_MAX (the highest point of the distribution range), and EST_DIST_BINS (the number of BINS in the distribution recording). */
typedef int Property;

/// \name Values for Estimator properties:
//@{
const Property EST_SAMPLE = 1;
const Property EST_DENS = 2;
const Property EST_MEAN = 4;
const Property EST_VAR = 8;
const Property EST_CUR = 16;
const Property EST_EVENTS = 32;

const Property EST_VALUE = 0;
const Property EST_DIFF = 64;
const Property EST_BASE_DIFF = 128;

const Property EST_DIST_MIN = 1;
const Property EST_DIST_MAX = 2;
const Property EST_DIST_BINS = 3;
//@}

class StochasticProcess;
class Time;

/// Base class for estimation of various stochastic properties.
/** Estimates various properties of stochastic
    variables (like mean, variance, density).
    An estimator receives a source of the type StochasticProcess, of which
    it receives the data during every step (every time proceedToNextState() is called). */
class Estimator : public Parametric
{
protected:
	Property nEstimate;  ///< what to record
	StochasticProcess *pSource; ///< the source of data
	uint nSamples; ///< number of samples recorded
	
	/// Time process for registering and running.
	Time *estimatorTime;

public:
	
	/// Construct.
	/** Registers the class with the Time class for automatic running. */
	Estimator(StochasticProcess *src, Time *time, const string& name="", const string& type="Estimator") ;
		
	/// Reset all estimation dependent values.
	virtual void init() = 0;

    int getSampleCount() { return nSamples; }
	
	/// Destroy.
	/** Un-Registers the class with the Time class for automatic running. */
	virtual ~Estimator();
	
	virtual void collect() = 0; ///< Eat the next data point.
	virtual Matrix getEstimate(const Property&) = 0; ///< Return an estimation.
};


#endif
