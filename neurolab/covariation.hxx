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

#ifndef COVARIATION_ESTIMATOR_HXX
#define COVARIATION_ESTIMATOR_HXX

/// Covariation process.
/** The covariation process of two stochastic processes \f$X_t\f$ and \f$Y_t\f$ is defined as \f[  \left[X,Y\right]_t = \int X_t dY_t + \int Y_t dX_t - X_tY_t. \f] For deterministic processes this equals zero. The mean of this process is zero, if both processes \f$X_t\f$ and \f$Y_t\f$ are uncorrelated. The mean can be tested using a ProcessEstimator. A covariation process can be used as integrator in differential equations. The Covariation class estimates the covariation process numerically, by using \f$\left[X,Y\right]_t = \int dX_t dY_t \f$. */
class Covariation: public StochasticProcess
{
private:
	StochasticProcess *xFirst;
	StochasticProcess *xSecond;
	
public:
	/// Construct.
	/** This constructs a covariation prcoess of thwo stochastic processes. The resulting process is active by default, but can be set passive. */
	Covariation(StochasticProcess *x, StochasticProcess *y, string name="", const string& type="Covariation Process") : StochasticProcess(x->getTime(), name, type) {
		// default name if "" is given
		if (getName()=="")
			setName("covariation process of " + x->getName() + " and " + y->getName());
		xFirst = x;
		xSecond = y;
	};
	
	/// Destruct.
	~Covariation() {};
	
	/// Calculate next value.
	/** If the process is active, this is called automatically. If it is passive, call proceedToNextState() first, and then prepareNextState(), to complete a full step.. */
	void prepareNextState() {
		stochNextValue += xFirst->getIncrement() * xSecond->getIncrement();
		stochNextStateIsPrepared = true;
	};
};

#endif
