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

#include "ring.hxx"
#include "stochastic.hxx"
#include "matrix.hxx"
#include "parametric.hxx"

#ifndef __ESTIMATOR_HXX
#define __ESTIMATOR_HXX


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

const Property EST_DIST_MIN = 1;
const Property EST_DIST_MAX = 2;
const Property EST_DIST_BINS = 3;
//@}


/// Base class for estimation of various stochastic properties.
/*! Estimates various properties of stochastic
    variables (like mean, variance, density).
    An estimator receives a source of the type Stochastic, of which
    it receives the data during every step (every time step() is called). */
class Estimator : public Parametric
{
protected:
	Property nEstimate;  ///< what to record
	Stochastic *pSource; ///< the source of data
	uint nSamples; ///< number of samples recorded
public:
	Estimator(Stochastic *src) { pSource = src; }; ///< Constructor
	int getSampleCount() { return nSamples; };
	virtual ~Estimator(){}; ///< Destructor
	virtual void step() = 0; ///< Eat the next data point.
	virtual Matrix mResult(const Property&) = 0; ///< Return an estimation.
};

#endif
