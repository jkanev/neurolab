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

const Property POISSON_DT = 0;
const Property POISSON_RATE = 1;
const Property PSV_POISSON_DT = 2;
const Property PSV_POISSON_RATE = 3;
const Property SC_VALUE = 4;
const Property COND_REVERSAL = 5;
const Property COND_WEIGHT = 6;
const Property TIME_DT = 7;

class Stochastic;
class StochVariable;
class StochFunction;

class TimeProcess: public StochVariable
{
public:
	TimeProcess(const Time *time) : StochVariable(time) {};
	
	TimeProcess(double dt) : StochVariable(0) {
		if( xTime	) delete xTime;
		xTime = new Time(dt);
	};
	
	~TimeProcess() {};
	
	virtual double d() {
		return xTime->dt;
	};
	
	virtual double dDelta() {
		return xTime->dt;
	}
	
	virtual void next() {
		dNextValue = dCurrentValue + xTime->dt;
	}
};

/// returns a scalar
class Scalar: public StochFunction
{
private:
	double dScalar;
public:
	/// Construct.
	/*! Construct a scalar from a double value. */
	Scalar(double scalar) : StochFunction(0) {dCurrentValue = dNextValue = scalar;};
	/// Construct.
	/*! Construct a scalar with a description. */
	Scalar(double scalar, string name);
	/// Construct.
	/*! Construcr a scalar with a description and a physical unit. */
	Scalar(double scalar, string name, string unitPrefix, string unitSymbol);
	virtual ~Scalar(){};
	double d();
	double dParameter(const Property&);
	void setParameter(const Property& n, double p);
};

/// returns the product of Xt and a scalar
class Product: public StochFunction
{
protected:
	double dScalar;
public:
	Product(double scalar) : StochFunction(0) {dScalar = scalar; };
	virtual ~Product(){};
	double dParameter(const Property&);
	void setParameter(const Property& n, double p);
	virtual void next() { dNextValue = dXt*dScalar; };
};

/// returns the product of Xt*Xt and a scalar
class Square: public Product
{
public:
	Square(double scalar) : Product(scalar) {};
	virtual ~Square(){};
	virtual void next() { dNextValue = dXt*dXt*dScalar; };
};

/// returns a product of a difference of Xt
/*! A functional which implements a*(b - X_t) */
class Conductance: public StochFunction
{
private:
	double dWeight;
	double dReversal;
	double dValue;
public:
	Conductance(double, double);
	virtual ~Conductance(){};
	double dParameter(const Property&);
	void setParameter(const Property& n, double p);
	virtual void next();
};

/// Returns delta peaks with a static rate.
/*! Poisson process: steps forward when using either () or (double) operators. */
class Poisson: public StochVariable, public Event
{
private:
	double dEvent;
	double dSum;
	double dRate;
public:
	Poisson(double p, const Time *time);
	~Poisson(){};
	virtual bool bEvent();
	virtual void next();
	virtual double dParameter(const Property&);
	virtual void setParameter(const Property&, double);
};



#endif
