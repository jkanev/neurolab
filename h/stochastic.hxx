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

#ifndef STOCHASTIC_HXX
#define STOCHASTIC_HXX

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <iostream>
#include <cmath>
#include <vector>

#include "physical.hxx"
#include "parametric.hxx"
#include "timedependent.hxx"

using namespace std;

class Time;

/// A stochastic variable.
/** This class implements a stochastic process, either has a simple process, or as a function of another variable. All forms of stochastic variables should inherit from this class. Main functionality is implemented in the prepareNextState() function, which should be overriden in deriving classes. */
class StochasticProcess: public Parametric, public TimeDependent, public Physical
{
private:
	// private copy constructor to prevent copying
	StochasticProcess( StochasticProcess &s );
	
protected:
	double stochCurrentValue; ///< the current value
	double stochNextValue; ///< the next value (direct future)
	bool stochNextStateIsPrepared; ///< whether prepareNextState() was successful
	string stochDescription; ///< the name of the quantity

public:
	
	/// Create.
	StochasticProcess(class Time *time, const string& name="", const string& type="Stochastic") : Parametric(name, type), TimeDependent(time) { 
		stochCurrentValue = 0.0;
		stochNextValue = 0.0;
		stochDescription = "stochastic process";
		stochNextStateIsPrepared = false;
	};
	
	/// Destroy.
	virtual ~StochasticProcess() {};
	
	/// Proceed one time step.
	/** This method can be overridden to implement the proceeding of one time step. This makes new information available at the current time. (See also proceedToNextState()). The default just writes stochNextValue into stochCurrentValue.  */
	virtual void proceedToNextState() {
		stochCurrentValue = stochNextValue;
		stochNextStateIsPrepared = false;
	};
	
	/// Calculate next value (preparing next step).
	/** This method should be overridden to implement the calculation of the next value, using information wich is available at the current time. The method must set the protected member stochNextStateIsPrepared to true if successful. */
	virtual void prepareNextState() {
		stochNextStateIsPrepared = true;
	};
	
	/// Whether 
	bool isNextStatePrepared() {
		return stochNextStateIsPrepared;
	}
	
	/// Returns the increment.
	/** This is the difference to the next time step. The difference to
	operator()() is that it never proceeds the object's time. */
	virtual double getIncrement() { return stochNextValue - stochCurrentValue; };
	
	/// Returns the value of the process.
	/** This is the current value. The difference to
	operator()(double) is that it never proceeds the object's time. Implementing this function is compulsary.  */
	virtual double getCurrentValue() { return stochCurrentValue; };

	/// Returns the next value of the process.
	/** This is the next value. This is the same as getCurrentValue() + getIncrement(). */
	virtual double getNextValue() { return stochNextValue; };
	
	/// Set the next value of the process.
	/** Used for reflecting boundaries etc.. */
	virtual void setNextValue(double d) { stochNextValue = d; };
	
	/// Set the current value of the process.
	/** Used for reflecting boundaries etc.. */
	virtual void setCurrentValue(double d) { stochCurrentValue = d; };
	
	/// Set stochastic description.
	void setDescription(string s) {
		stochDescription = s;
	};
	
	/// Initialise time-dependent values.
	virtual void init() {
		stochCurrentValue = 0.0; stochNextValue = 0.0;
	};
	
	/// Get the name
	/** Returns the stochastic name */
	string getDescription() {
		return stochDescription;
	};
};

/// A simple stochastic variable.
/** This is the simplest stochastic process available. All stochastic processes which are not parametrised at every time step should inherit from this class.  If you derive from this class, you must provide a prepareNextState() function, which sets stochNextValue by using stochCurrentValue. */
class StochasticVariable: public StochasticProcess
{
private:
	// private copy constructor to prevent copying
	StochasticVariable( StochasticVariable &sv );

public:
	
	/// Create.
	StochasticVariable(class Time *time, const string& name="", const string& type="Stochastic Variable") : StochasticProcess(time, name, type) {};
		
	/// Returns the value at  the current time step.
	/** The content of the protected variable stochCurrentValue is returned. */
	double operator()() { 
		return stochCurrentValue;
	};
	
	/// Returns the increment at  the current time step.
	/** The difference of the protected variables stochNextValue - stochCurrentValue is returned. */
	double d() {
		return stochNextValue - stochCurrentValue;
	}	
};

/// A parametrized stochastic variable.
/** Any function which receives a stochastic variable as input, or has some inherent stochastic behaviour, should derive from this class. The members stochCurrentValue and stochNextValue are used for the input to the function. Deriving classes must proved calculateCurrentValue(), which returns the value based on stochCurrentValue, and calculateNextValue(), which returns the the value based on stochNextValue. */

class StochasticFunction: public StochasticProcess
{
private:
	/// Private copy constructor to prevent copying.
	StochasticFunction( StochasticFunction &sf );

public:
	
	/// Create.
    StochasticFunction(class Time *time, const string& name="", const string& type="Stochastic Function")
        : StochasticProcess(time, name, type) {}

	/// Returns the value at  the current time step.
	/** Same as getCurrentValue(x). */
	double operator()(double x) {
		return getCurrentValue(x);
    }
	
	/// Returns the increment at  the current time step.
	/** Same as getIncrement(x). */
	double d(double x) {
		return getIncrement(x);
    }
	
	/// Returns the value at  the next time step.
	/** Calling this method sets the function input.*/
	virtual double getCurrentValue(double x) {
		stochCurrentValue = x;
		return calculateCurrentValue();
    }
	
	/// Returns the value at  the next time step.
	/** Calling this method sets the function input.*/
	virtual double getNextValue(double x) {
		stochNextValue = x;
		return calculateCurrentValue();
    }
	
	/// Returns the increment at  the current time step.
	/** Calling this method sets the function input. This method does not proceed the object's time. */
	virtual double getIncrement(double xt) {
		stochNextValue = xt;
		return calculateNextValue() - calculateCurrentValue();
    }
	
	/// Calculates the current value based on the current input.
	virtual double calculateCurrentValue() = 0;
	
	/// Calculates the next value based on the current input.
	virtual double calculateNextValue() = 0;
};


/// An object which generates events.
/** Useful for event-triggered averages. */
class StochasticEventGenerator : public StochasticVariable
{
protected:
    bool eventCurrentValue;
    bool eventNextValue;
    
public:
	
	/// Create.
    StochasticEventGenerator(class Time *time, const string& name="", const string& type="Event Generator")
        : StochasticVariable( time, name, type) {}
	
	/// Destroy.
    virtual ~StochasticEventGenerator() {}
	
	/// Whether an event is present.
    virtual bool hasEvent() { return eventCurrentValue; }

	/// The amount of events present.
    virtual uint getEventAmount() { return eventCurrentValue ? 1 : 0; }
    
    /// Proceed one time step
    virtual void proceedToNextState();
};

/// An object which uses the randn function.
/** This class implements a random variable with normal distribution (gaussian). */
class RandN
{
private:
	static uint32_t nState;
	static uint nRefs;
	static boost::random::mt19937_64 *randGenerator;
	static boost::random::uniform_real_distribution<double> dist;
	
	static int nRand; // index into aRand
	static double aRand[2]; // two random variables
	
public:
	
	/// Construct.
	RandN();
	
	/// Destruct.
	~RandN();
	
	/// Retrieve random variable.
	/** This function generates one random variable. The returend values are normally (Gaussian) distributed, with a mean of 0.0 and a variance of 1.0. The method used is the Polar-Masaglia method, which is the quickest known so far. */
	double dRandN();
	
	/// Retrieve random variable.
	/** This function generates one random variable. The returend values are evenly distributed between 0 and 1. */
	double dRandE();
};

#endif
