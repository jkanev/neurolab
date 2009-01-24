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

#ifndef __STOCHASTIC_HXX
#define __STOCHASTIC_HXX

#include <gsl_rng.h> // random numbers from GSL
#include <iostream>
#include <cmath>
#include "physical.hxx"
#include "parametric.hxx"
#include "timedependent.hxx"

using namespace std;


/// A stochastic variable.
/*! This class implements a stochastic process, either has a simple process, or as a function of another variable. It can run in active mode -  the ()-operator will then forward it by one step; or in passive mode - in which case the ()-operator will only retrieve value or delta. The value of a stochastic variable changes with each time step - either when a () operator or the d() function is called. All forms of stochastic variables should inherit from this class. */
class Stochastic: public Parametric, public Physical, public TimeDependent
{
private:
	// private copy constructor to prevent copying
	Stochastic( Stochastic &s );
	
protected:
	bool bActive; ///< wether the process is active
	double dCurrentValue; ///< the current value
	double dNextValue; ///< the next value (direct future)
	string sStochasticName; ///< the name of the quantity

public:
	Stochastic(const class Time *time) : TimeDependent(time) { 
		bActive = true;
		dCurrentValue = 0.0;
		dNextValue = 0.0;
		sStochasticName = "stochastic process";
		sPhysicalName = "stochastic process";
	};
	virtual ~Stochastic() {};
	
	/// Proceed one time step.
	/*! This method can be overridden to implement the proceeding of one time step. This makes new information available at the current time. (See also step()). The default just writes dNextValue into dCurrentValue.  */
	virtual void step() { dCurrentValue = dNextValue; };
	
	/// Calculate next value.
	/*! This method should be overridden to implement the calculation of the next value, using information wich is available at the current time. */
	virtual void next() { };
	
	/// Sets the process to be active.
	/*! An active process is forwarded each time one of the ()-operators is called. This forwarding has to be implemented by the deriving class. */
	virtual void setActive() { bActive = true; };
	
	/// Sets the process to be passive.
	/*! A passive process is not changed when one of the ()-operators is called. */
	virtual void setPassive() { bActive = false; };
	
	/// Returns the increment.
	/*! This is the difference to the next time step. The difference to
	operator()() is that it never proceeds the object's time. */
	virtual double dDelta() { return dNextValue - dCurrentValue; };
	
	/// Returns the value of the process.
	/*! This is the current value. The difference to
	operator()(double) is that it never proceeds the object's time. Implementing this function is compulsary.  */
	virtual double dValue() { return dCurrentValue; };

	/// Returns the next value of the process.
	/*! This is the next value. This is the same as dValue() + dDelta(). */
	virtual double dNext() { return dNextValue; };
	
	/// Set the name
	void setName(string s) {
		sStochasticName = s;
	};
	
	virtual void init() { dCurrentValue = 0.0; dNextValue = 0.0; };
	/// Get the name
	/*! Returns the stochastic name */
	string getName() {
		return sStochasticName;
	};
};

/// A simple stochastic variable.
/*! This is the simplest stochastic process available. All stochastic processes which are not parametrised at every time step should inherit from this class.  If you derive from this class, you must provide a next() function, which sets dNextValue by using dCurrentValue. */
class StochVariable: public Stochastic
{
private:
	// private copy constructor to prevent copying
	StochVariable( StochVariable &sv );

public:
	StochVariable(const class Time *time) : Stochastic(time) {};
		
	/// Returns the value at  the current time step.
	/*! This is mainly a callback function for the Differentiable class. If the process is active this also proceeds the object's time.  Implementing this function is compulsary. */
	virtual double operator()() { 
		double value = dCurrentValue;
		if(bActive) {
			step();
			next();
		}
		return value;
	};
	
	/// Returns the increment at  the current time step.
	/*! This is mainly a callback function for the Differentiable class. If the process is active this also proceeds the object's time. */
	virtual double d() {
		double diff = dNextValue - dCurrentValue;
		if(bActive) {
			step();
			next();
		}
		return diff;
	}	
};

/// A parametrized stochastic variable.
/*! Any function which receives a stochastic variable as input, or has some inherent stochastic behaviour, should derive from this class. At every time step t a value x has to be given. This value x influences the value of the function at time t. Therefore the next future value (dNextValue) and the delta (difference next value - current value) cannot be given without receiving a (hypothetical) future value of x. This behaviour is unsuitable for circular dependencies, which should always be created using StochVariable, or otherwise the ()-operator should be re-implemented. For the same reason this process is active as default.
 
 If you derive from this class, you must provide a next() function, which sets dNextValue by using dCurrentValue and dXt. */

class StochFunction: public Stochastic
{
private:
	/// Private copy constructor to prevent copying.
	StochFunction( StochFunction &sf );

protected:
	/// The current input.
	/*! This should be used by the next() function. */
	double dXt;
	
public:
	StochFunction(const class Time *time) : Stochastic(time){
		bActive = true;
	};
	
	/// Returns the value at  the current time step.
	/*! \param xt The input of the function, at the current time step.
	\returns The value at the current time step.
	Calling this method sets the function input. If the process is active this also proceeds the object's time, after calculating the return value. This method has been written mainly as a callback for the Differentiable class. */
	virtual double operator()(double xt) { 
		dXt = xt;
		if(bActive) {
			// cout << "operator()" << flush;
			next();
			step();
		} 
		return dCurrentValue;
	};
	
	/// Returns the increment at  the current time step.
	/*! \param xt The input of the function at the next (future) time step.
	\returns The difference to the value at the next time step.
	Calling this method does not set the function input. The returned difference is the difference of the current value to the value at the next time step, if the input value xt where given at the next time step. This hypothetical calculation of the future is useful for fixed point iteration in stratonovich SDEs. If the process is active this also proceeds the object's time, after calculating the return value. This method has been written mainly as a callback for the Differentiable class. */
	virtual double d(double xt) {
		double diff;
		double currentXt = dXt;
		dXt = xt;
		next();
		dXt = currentXt;
		diff = dNextValue - dCurrentValue;
		if(bActive)
			step();
		return diff;
	};
	
	/// Returns the value at  the current time step.
	/*! \param xt The input of the function.
	\returns The value at the current time step.
	Calling this method sets the function input. This method does not proceed the object's time. */
	virtual double dValue(double xt) {
		return dCurrentValue; 
	};
	
	/// Returns the increment at  the current time step.
	/*! \param xt The input of the function.
	\returns The difference to the value at the next time step.
	Calling this method sets the function input. This method does not proceed the object's time. */
	virtual double dDelta(double xt) {
		double diff;
		double xtSave = dXt;
		double nextSave = dNextValue;
		dXt = xt;
		next();
		diff  = dNextValue - dCurrentValue;
		dNextValue = nextSave;
		dXt = xtSave;
		return diff;
	};
	

};


/// An object which generates events.
/*! Useful for event-triggered averages. */
class Event
{
public:
	virtual bool bEvent() = 0;
	Event() {};
	virtual ~Event() {};
};

/// An object which uses the randn function.
/*! This class implements a random variable with normal distribution (gaussian). */
class RandN
{
private:
	static unsigned long nState;
	static uint nRefs;
	static gsl_rng *xGenerator;
	static int nRand; // index into aRand
	static double aRand[2]; // two random variables

public:
	
	/// Construct.
	RandN();
	
	/// Destruct.
	~RandN();
	
	/// Retrieve random variable.
	/*! This function generates one random variable. The returend values are normally (Gaussian) distributed, with a mean of 0.0 and a variance of 1.0. The method used is the Polar-Masaglia method, which is the quickest known so far. */
	double dRandN();
};

#endif
