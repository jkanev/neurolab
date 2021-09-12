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

#ifndef __FUNCTION_H
#define __FUNCTION_H

#include <neurolab/physical.hxx>
#include <neurolab/matrix.hxx>
#include <cmath>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

// prototypes
class Function;

/// Divide.
/** Creates a Function which is the quotient of the two input functions. */
Function &operator/(Function &left, Function &right);

/// Exponential.
/** Creates a Function which is the exponential of the input function. */
Function &exp(Function &right);

/// Logarithm.
/** Creates a Function which is the logarithm of the input function. */
Function &ln(Function &right);

/// Sine.
/** Creates a Function which is the logarithm of the input function. */
Function &sin(Function &right);


/// Cosine.
/** Creates a Function which is the logarithm of the input function. */
Function &cos(Function &right);

/// Square root.
/** Creates a Function which is the sqare root of the input function. */
Function &sqrt(Function &right);

/// Square.
/** Creates a Function which is the sqare of the input function. */
Function &square(Function &right);

/// Root.
/** \returns A Function which is the nth root of the input.. */
Function &root(Function &left, Function &n);

/// Power.
/** \returns A Function which is the nth power of the input. */
Function &power(Function &left, Function &n);

/// Integral..
/** Creates the undefinite integral of the given integrand. If plotted, you will see the function \f[ \mbox{Function(t)} = \int_0^t f(x) dx\f]. */
Function &integral(Function &integrand, Function &integrator);

/// Increments.
/** Creates the differential df(x) of a given integrand f(x). If plotted, you will see the function \f[ \mbox{Function(t)} = d f(x) \f]. */
Function &increments(Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &greater(Function &left, Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &smaller(Function &left, Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &operator>(Function &left, Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &operator<(Function &left, Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &operator<=(Function &left, Function &right);

/// Comparison.
/** Returns 1.0 at places where the comparison is true, and 0.0 at places where the comparison is false. */
Function &operator>=(Function &left, Function &right);

/// Sign of a number.
/* Returns 1.0 if x>0.0, 0.0 if x=0.0, -1.0 otherwise. */
Function &sign(Function &right);	

/// Appends the function specification string to a stream.
/** The appended string is gnuplot readable, and may contain lots of parentheses, depending on the complexity of the function. */
ostream& operator<<(ostream& os, Function& f);

/// Create input.
/** Creates a function which serves as input for other functions. */
Function &input(string);
Function &inputX();
Function &inputY();

/// Create a function from a constant
Function &cnst(double);

/// Create a function from a constant and a name
Function &cnst(double, string);

/// Create a function from a variable
Function &var(double &);

/// Create a function from a variable and a name
Function &var(double &, string);

/// Create a function from a matrix
Function &xtable(Matrix &, Function &right);

/// An empty function
Function &empty();

typedef enum fcn {
	X_TABLE, VARIABLE, CONSTANT, PLUS, MINUS, NEG, TIMES, OVER, EXP, LN, SIN, COS, SQUARE, SQRT, POWER, ROOT, INCREMENTS, INTEGRAL_X, INTEGRAL_Y, INPUT_X, INPUT_Y, GREATER, GREATER_EQUAL, SMALLER, SMALLER_EQUAL, SIGN, EMPTY
} FunctionType;

/// Class for mathematical functions.
/** This class implements mathematical functions for use with the Display class (which supports only gnuplot output at the moment. Functions are created from Variables using +, -, *, /, exp, ln, sqrt, square, sqrt, power, or root. Note the special variables X and Y. Functions use referencing, and form an execution tree. When any variable in the function changes, the function output changes. Appending a function to a Display (using <<) will append the output with the current variable setting. That way you can append the same function to the same display, with different values of its variables, and the result will be different curves on top of each other. The function can be appended to any string using the << operator.*/
class Function: public Physical {
private:
	// internal values
	int functionUseCount;
	FunctionType eType; ///< the type of this object
	double *xSrc; ///< the source variable
	Function *xLeft; ///< term left of operator
	Function *xRight; ///< term right of operator
	Function *xPlus; ///< additional term
	Matrix *xTable; ///< function table
	double (Function::* xFunction)() const; ///< pointer to execution function
	bool bVerbose; ///< verbosity (whether to cast values to stdout during operation)
	ostream *oLog; ///< stream to append verbosity output to
	
	// cached values
	mutable double dValue; ///< the current value
	mutable double dLastValue; ///< the last value
	mutable bool bValueValid; ///< whether the current value is valid 
	mutable double dXInput; ///< current x input
	mutable double dNextXInput; ///< next x input
	mutable double dYInput; ///< current y input
	mutable double dNextYInput; ///< next y input
		
	// type
	void setType(FunctionType);
	
	// input range
	double dXMin; ///< lower border for input range
	double dXMax; ///< upper border for input range
	double dXStep; ///< step for input range
	double dYMin; ///< lower border for input range
	double dYMax; ///< upper border for input range
	double dYStep; ///< step for input range
		
	// private constructors; used by the math operators, which are friends
	void init();
	void copy(const Function &);
	Function(FunctionType ft);
	Function(double &d, FunctionType);	
	Function(double &d, string name, FunctionType);
	Function(Function &left, FunctionType ft);
	Function(Function &left, Function &right, FunctionType ft);
	Function(Function &left, Function &right, Function &plus, FunctionType ft);
	Function(Function &left, Matrix *m, FunctionType ft);	
	
	// execution functions
	double plus() const { return xLeft->getValue() + xRight->getValue(); };
	double id() const { return *xSrc; };
	double constant() const { return dValue; };
	double minus() const { return xLeft->getValue() - xRight->getValue(); };
	double neg() const { return -xRight->getValue(); };
	double times() const { return xLeft->getValue() * xRight->getValue(); };
	double over() const { return xLeft->getValue() / xRight->getValue(); };
	double exponential() const { return exp(xRight->getValue()); };
	double lognat() const { return log(xRight->getValue()); };
	double sinus() const { return sin(xRight->getValue()); };
	double cosinus() const { return cos(xRight->getValue()); };
	double square() const { double d = xRight->getValue(); return d*d; };
	double squareroot() const { return sqrt(xRight->getValue()); };
	double power() const { return pow(xLeft->getValue(), xRight->getValue()); };
	double root() const { return pow(xLeft->getValue(), 1 / xRight->getValue()); };
	double integral_x() const;
	double integral_y() const { return 0.0; };
	double increments() const;
	double inputx() const { return dValue; };
	double inputy() const { return dValue; };
	double greater() const { return xLeft->getValue() > xRight->getValue() ? 1.0 : 0.0; };
	double greater_equal() const { return xLeft->getValue() >= xRight->getValue() ? 1.0 : 0.0; };
	double smaller() const { return xLeft->getValue() < xRight->getValue() ? 1.0 : 0.0; };
	double smaller_equal() const { return xLeft->getValue() <= xRight->getValue() ? 1.0 : 0.0; };
	double signum() const { double d = xRight->getValue(); return d > 0.0 ? 1.0 : (d < 0.0 ? -1.0 : 0.0); };
	double x_table() const;
	
protected:
	int nRefs; ///< we do reference counting
	vector<Function *> vParents; ///< parents, for ref counting
	string sName;
	
	/// Default Constructor
	/** Constructs an empty function. This is mainly for use in the derived classes X and Y. */
	Function() {
		init();
	};	

	/// Set unvalid.
	/** After this, the function will perform a full computation, if getValue() is called. Otherwise just the last value is return. This function needs to be called if some input has changed lower down in the execution tree. */
	void unvalidate();
	
public:

	/// Copy.
	Function(const Function &);
	
	/// Double to Function
	/** Constructs a constant from a double. Same as cnst(double). */
	Function(double d);	

	/// Destructor.
	virtual ~Function();
	
	/// Returns the name.
	/** \return The name of the Function.
	This is the string which has been set with setName(), and which will appear as a gnuplot lable when this function is appended to a gnuplot display. */
	virtual string getName() const;
	
	/// Set function to be verbose.
	/** If this is set, the function will append its current value and its name to a stream, each time getValue() is called. setVerbose() will set all subfunctions to be verbose as well. This way a log of the whole calculation will be appended to a stream. */
	Function &setVerbose(
		bool verbose = true, ///< whether the function is verbose (sends state log-messages to a stream)
		ostream *log = &cout ///< the stream receiving the log messages
	);
	
	/// Returns the value.
	/** Computes the current value of the function. This is dependent on variables and input.*/
	virtual double getValue() const;
	
	/// Returns the value at a point.
	/** Computes the current value of the function. This is dependent on variables and input.*/
	double operator()(double x) {
		setXInput(x);
		return getValue();
	};
	
	/// Returns the value at a point.
	/** Computes the current value of the function. This is dependent on variables and input.*/
	double operator()(double x, double y) {
		setXInput(x);
		setYInput(y);
		return getValue();
	};
	
	/// Set x input.
	/** This sets the x input of the whole tree to the given value. */
	void setXInput(double);
	
	/// Set y input.
	/** This sets the y input of the whole tree to the given value. */
	void setYInput(double);
	
	/// Get x input.
	/** This gets the x input of the whole tree. If input values are different at different branches, the first which is encountered will be returned. */
	double getXInput();
	
	/// Get y input.
	/** This gets the y input of the whole tree. If input values are different at different branches, the first which is encountered will be returned. */
	double getYInput();
	
	/// Returns the value.
	/** Returns the last value of the function. This is the value which was produced by the last getValue() call. No recomputation is executed. */
	virtual double getLastValue() const;
	
	/// Returns the function graph.
	/** Returns the function graph. This is dependent on variables and input range.*/
	// virtual Matrix &getGraph() const;
	
	/// Sets the physical type.
	/** This sets a physical name (like 'voltage'), and a unit (like mV). */
	Function &setPhysical(string s, Unit u);
	
	/// Set the x range.
	/** Sets the x range of the function, i.e. the function will be plotted in a range from minimum to maximum, with the given step size. */
	virtual Function &setXRange(/** minimum */ double min, /** maximum */ double max, /** step size */ double stepsize);
	
	/// Set the y range.
	/** Sets the y range of the function, i.e. the function will be plotted in a range from minimum to maximum, with the given step size. */
	virtual Function &setYRange(/** minimum */ double min, /** maximum */ double max, /** step size */ double stepsize);
	
	/// Get input name.
	/** Returns the name of the first input, for instance 't' or 'x'. */
	string getXInputName();
	
	/// Get input name.
	/** Returns the name of the second input, for instance 't' or 'x'. */
	string getYInputName();
	
	/// Sets the name.
	/** This function returns a reference to its object for convenience. */
	Function &setName(string s);
	
	/// Add.
	/** Adds the given function to this function object. */
	void operator+=(Function &function);
	friend Function &operator+(Function &left, Function &right);
	friend Function &operator+(double left, Function &right);
	friend Function &operator+(Function &left, double right);
	
	/// Subtract.
	/** Subtracts the given function from the object.. */
	void operator-=(Function &function);
	friend Function &operator-(Function &left, Function &right);
	friend Function &operator-(double left, Function &right);
	friend Function &operator-(Function &left, double right);
	friend Function &operator-(Function &right);
	
	/// Multiply.
	/** Multiplies the given function to this function object. */
	void operator*=(Function &function);
	friend  Function &operator*(Function &left, Function &right);
	friend  Function &operator*(double left, Function &right);
	friend  Function &operator*(Function &left, double right);
	
	/// Divide.
	/** Divides the given function to this function object. */
	void operator/=(Function &function);
	friend Function &operator/(Function &left, Function &right);
	friend Function &operator/(double left, Function &right);
	friend Function &operator/(Function &left, double right);
	
	friend Function &exp(Function &right);
	friend Function &ln(Function &right);
	friend Function &sin(Function &right);
	friend Function &cos(Function &right);
	friend Function &sqrt(Function &right);
	friend Function &square(Function &right);
	friend Function &root(Function &left, Function &n);
	friend Function &power(Function &left, Function &n);
	friend Function &integral(Function &integrand, Function &integrator);
	friend Function &increments(Function &right);
	friend Function &greater(Function &left, Function &right);
	friend Function &smaller(Function &left, Function &right);
	friend Function &operator>(Function &left, Function &right);
	friend Function &operator<(Function &left, Function &right);
	friend Function &operator<=(Function &left, Function &right);
	friend Function &operator>=(Function &left, Function &right);
	friend Function &sign(Function &right);	
	//friend Function &special(string s);
	friend ostream& operator<<(ostream& os, Function& f);
	friend Function &input(string);
	friend Function &inputX();
	friend Function &inputY();
	friend Function &cnst(double);
	friend Function &cnst(double, string);
	friend Function &xtable(Matrix &, Function &right);
	friend Function &empty();
};


#endif
