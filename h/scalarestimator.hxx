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

#ifndef __SCALAR_ESTIMATOR_H
#define __SCALAR_ESTIMATOR_H

#include "estimator.hxx"

/// estimates mean, variance, etc. of a scalar stochastic variable
class ScalarEstimator: public Estimator
{
protected:
	double dSample; // last sample
	double dOne;    // recording first moment
	double dTwo;    // recording second moment
	double dThree;  // recording third moment
	double *aDist;  // record distribution
	int nDist;      // length of aDist
	double aDistRange[2]; // range of dist
	double dDistOffset; // offset of dist (helper variable)
	double dDistScale; // scale of dist (helper variable)
	void estimate(double); // estimate from one data point
public:
	virtual void collect(); ///< Eat next piece of data
	virtual void init(); ///< reset all values
	virtual Matrix getEstimate(const Property&); ///< return result of estimation
	ScalarEstimator(const Property&, StochasticProcess *, Time *); ///< Constructor
	void setProperty( const Property&, double ); ///< set distribution-related properties
	virtual ~ScalarEstimator(); ///< Destructor
};

#endif
