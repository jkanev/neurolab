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

#include "estimator.hxx"

#ifndef __DEPENDANCE_ESTIMATOR_HXX
#define __DEPENDANCE_ESTIMATOR_HXX


/// Estimating dependencies of conditional variables.
/** Some stochastic variables are dependent on the value of other variables. These
dependancies can be measured with this class. Given two variables \f$ X_t \f$ (source)
and \f$ Y_t \f$ (base), where \f$ p(X_t|Y_t) \neq p(X_t)\f$, this class measures the
distribution \f$ p(X_t|Y_t)\f$, the expectation \f$ E{X_t|Y_t}\f$ and the
variance \f$ Var\{X_t|Y_t\} \f$.  */

class DependanceEstimator : public Estimator
{
private:
	double *aSamples; // array with number of samples per point
	double *aOne; // array with first moments
	double *aTwo; // array with second moments
	double *aThree; // array with third moments
	double **aDist; // array with distribution
	double dBegin; // start of recording range
	double dEnd; // end of recording range
	double getIncrement; // bin width of recording range
	double dDistBegin; // start of distribution recording range
	double dDistEnd; // end of distribution recording range
	double dDistDelta; // bin width of distribution recording range
	int dependanceBins; // size of aOne, aTwo, aSamples and aThree
	
protected:
	StochasticProcess *xBase;
	
public:
	/// Construct Object
	/** Constructs a DependanceEstimator. One stochastic variable (src) will be plotted
	against the other (base). The values baseBegin, baseEnd, baseBins give begin, end and
	resolution of the plot, i.e. the values of the base variable for recording. */
	DependanceEstimator (
		const Property &property, ///< a collection of things to record
		StochasticProcess *src, ///< source variable
		StochasticProcess *base, ///< base variable
		Time *time, ///< the global time object
		double baseBegin, ///< lowest value of base variable
		double baseEnd, ///< hightest value of base variable
		int baseBins, ///< number of bins (resolution) for base variable
		double srcStart = 0.0, ///< (only for recording densities) lowest value of source variable
		double srcEnd = 0.0, ///< (only for recording densities) hightest value of source variable
		int srcDelta = 0 ///< (only for recording densities) number of bins (resolution) for source variable
	);
	
	/// Destructor
	virtual ~DependanceEstimator();
	
	/// Initialise object.
	virtual void init();
	
	/// Eat the next data point.
	virtual void collect();
	
	/// Return an estimation.
	virtual Matrix mResult(const Property&);
};

#endif
