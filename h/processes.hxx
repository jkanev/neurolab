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

#ifndef _PROCESSES_HH
#define _PROCESSES_HH

#include "differentiable.hxx"
#include "stochastic.hxx"

class StochasticProcess;
class StochasticVariable;
class StochasticFunction;

class TimeProcess: public StochasticVariable
{
public:
	TimeProcess(Time *time, const string& name="t", const string& type="Time Process") : StochasticVariable(time, name, type) {
		physicalUnit = time->getUnit();
	};
	
	TimeProcess(double dt) : StochasticVariable(0) {
		physicalUnit = Unit("","s");
		if( xTime) delete xTime;
		xTime = new Time(dt);
	};
	
	~TimeProcess() {};
	
	virtual double d() {
		return xTime->dt;
	};
	
	virtual double getIncrement() {
		return xTime->dt;
	}
	
	virtual void prepareNextState() {
		stochNextValue = stochCurrentValue + xTime->dt;
		stochNextStateIsPrepared = true;
	}
};

/// returns a scalar
class Scalar: public StochasticFunction
{
private:
	double scalarValue;
public:

	/// Construct.
	/** Construct a scalar with a description. */
	Scalar(double scalar, const string& name, const string& type="Scalar");

	/// Construct.
	/** Construcr a scalar with a description and a physical unit. */
	Scalar(double scalar, string unitPrefix, string unitSymbol, const string& name, const string& type="Scalar");

	/// Destroy.
	virtual ~Scalar(){};

	/// Get parameter.
	/** Implements "value". */
	virtual string getParameter(const string& name) const;

	/// Set parameter.
	/** Implements "value". */
	virtual void setParameter(const string& name, const string& value);
	
	virtual double calculateCurrentValue() {
		return scalarValue;
	};
	
	virtual double calculateNextValue() {
		return scalarValue;
	};
};

/// A product of the function input and a scalar.
class Product: public StochasticFunction
{
protected:
	double productFactor;

public:
	
	/// Create.
	Product(double factor, const string& name="", const string& type="Product");
	
	/// Destroy.
	virtual ~Product(){};
	
	/// Return current product value.
	virtual double calculateCurrentValue();
			
	/// Return next product value.
	virtual double calculateNextValue();
	
	/// Get parameter.
	/** Implements "value". */
	virtual string getParameter(const string& name) const;

	/// Set parameter.
	/** Implements "value". */
	virtual void setParameter(const string& name, const string& value);
};

/// returns the product of Xt*Xt and a scalar
class Square: public Product
{
public:
	Square(double scalar) : Product(scalar) {};
	virtual ~Square(){};
	
	/// Returns the value at the next time step.
	virtual double calculateNextValue() {
		return stochNextValue*stochNextValue*productFactor;
	};
	
	/// Returns the value at the current time step.
	virtual double calculateCurrentValue() {
		return stochCurrentValue*stochCurrentValue*productFactor;
	};
};

/// returns a product of a difference of Xt
/** A functional which implements a*(b - X_t) */
class VoltageDependance: public StochasticFunction
{
private:
	double dWeight;
	double dReversal;
	double getCurrentValue;
public:
	
	/// Construct with object name.
	VoltageDependance(double, double, const string& name="", const string& type="Weighted Difference");
	
	/// Destroy.
	virtual ~VoltageDependance(){};
	
	/// Create parameter string.
	string getParameter(const string&) const;
	
	/// Set parameter from string.
	void setParameter(const string& name, const string& value);
	
	/// Generate next value.
	virtual double calculateCurrentValue();
	
	/// Generate next value.
	virtual double calculateNextValue();
};

/// Returns delta peaks with a static rate.
/** Poisson process: steps forward when using either () or (double) operators. */
class Poisson: public StochasticEventGenerator
{
private:
	double dRate;
public:
	
	/// Create Poisson process.
	Poisson(double p, Time *time, const string& name="", const string& type="Poisson process");
	
	/// Destroy Poisson process.
	~Poisson(){};
	
	/// Whether event is present.
	virtual bool hasEvent();
	
	/// Whether event is present.
	virtual uint getEventAmount();
	
	/// Calculate next time value.
	virtual void prepareNextState();
	
	/// Get parameter.
	virtual string getParameter(const string& name) const;
	
	/// Set parameter.
	virtual void setParameter(const string& name, const string& value);
};



#endif
