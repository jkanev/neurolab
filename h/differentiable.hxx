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

#ifndef _DIFFERENTIABLE_HXX
#define _DIFFERENTIABLE_HXX

#include <vector>
#include <iostream>
#include "stochastic.hxx"

using namespace std;

/// A stochastic differential equation.
/** Class implementing stochastic differential equations (SDEs). Ito-
    and Stratonovich integrals can be chosen, integrators and
    integrands can be added at runtime.

    This class implements stochastic differential equations (SDEs) of the form
       \f[ dX = \sum_i f_i(X) dY_i \f]
   (using It\^o integrals), or
       \f[ dX = \sum_i f_i(X) \circ dY_i \f]
   (using Stratonovich integrals). The sum goes over multiple
   different functions \f$f_i\f$.
   These should be intepreted as integral equations of the form
       \f[ X - X_0 = \sum_i \intop f_i(X) dY_i \f]
   (Ito), or
       \f[ X - X_0 = \sum_i \intop f_i(X) \circ dY_i \f]
   (Stratonovich). It\^o mode can be set by calling setIto(), Stratonovich
   mode can be set by calling setStratonovich(). Which mode is used can
   be tested by querying isIto() (true if Ito mode) or isStratonovich() (true
   if Stratonovich integrals are used).
   At the moment there is only the 0.5-strong-order Euler scheme
   implemented, but Milstein and Taylor of higher order are planned for the future. See the main page for display of the error made when integrating. Here is a short example of the difference of Ito and Stratonovich integration (the equation is \f$dX_t = aX_tdt + bX_tdW_t\f$ or \f$dX_t = aX_tdt + bX_t\circ dW_t\f$ with \f$a=-8.0\f$ and \f$b=0.8\f$.):

   The integrands \f$f_i(X_t)\f$ are objects of the type Integrand,
   and the integrators \f$dY_i\f$ are objects of the type
   Integrator. They are stored in arrays of function objects,
   and their Integrand::operator()(double) or Integrator::operator()()
   are called each time proceedToNextState() is called. Use dGetValue() (from StochVar)
   or operator(double) to get the current value of the integral.
   The function proceedToNextState() must be used to calculate the values of the next
   time step, the function prepareNextState() must be used to update the object.
   New integrators and integrands may be added easily
   by calling nAddIntegrator() or nAddIntegrand().

   DifferentialEquation itself is of type Integrator and of type Integrand,
   so complex cascades of differential equations may be implemented.
   If you have a complex network of interdependent SDEs, you must first call
   the proceedToNextState() function of all objects, then the prepareNextState() function of all objects.
   Otherwise circular dependencies will not get updated correctly.
   
*/
class DifferentialEquation: public StochasticVariable
{
protected:
	double eqnX0; /// starting value
public:
	/// Construct.
	DifferentialEquation (
		double x0, ///< starting value
		double dx0, ///< starting increment
		const string& name="",   ///< object name
		const string& type="Differential Equation"   ///< object type
	);
	
	/// Construct.
	DifferentialEquation (
		Time *time,   ///< global time object
		double x0, ///< starting value
		double dx0, ///< starting increment
		const string& name="",   ///< object name
		const string& type="Differential Equation"   ///< object type
	);
	
	/// Construct.
	/** The same as using DifferentialEquation(0.0, 0.0). */
	DifferentialEquation (
		const string& name="",   ///< object name
		 const string& type="Differential Equation" )   ///< object type
	: StochasticVariable(0, name, type)
	{
		DifferentialEquation(0.0, 0.0);
		setIto();
		addParameter("equation");
	};
	
	/// Destroy.
	virtual ~DifferentialEquation(); 

	/// Calculate value for next time step.
	/** This just calculates the next value.
		Nothing is changed before proceedToNextState() is called. */
	virtual void prepareNextState();
	
	/// Sets value to a starting value.
	virtual void init (
		double x0   ///< starting value
	);
	
	/// Sets value to starting value
	virtual void init();

	/// Get the starting value
	double getStartingValue() const;
	
	/// Add term.
	/** \return index of new term */
	int addTerm (
		StochasticFunction *integrand, /// integrand part of term
		StochasticVariable *integrator   ///< integrator part of term
	);
	
	/// Remove term.
	void rmTerm (
		int n   ///< index of term
	);
	
	/// Set term.
	void setTerm (
		int n,   ///< index of term
		StochasticFunction *integrand, /// integrand part of term
		StochasticVariable *integrator   ///< integrator part of term
	);
	
	/// Get number of terms.
	/** \return number of terms in differential equation */
	int getNTerms() const { 
		return eqnTermAmount;
	}
	
	/// Get integrand part of a term.
	StochasticFunction *getIntegrand (
		int n   ///< index of term
	);
	
	/// Get integrator part of a term.
	StochasticVariable *getIntegrator (
		int n   ///< index of term
	);
	
	/// Get parameter.
	/** In a derived class, override this to handle every parameter you implement. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter.  */
	virtual string getParameter (
		const string& name   ///< name of parameter
	) const;
	
	/// Member pointer to one of the privat fcns.
	void (DifferentialEquation::* eqnMethodPtr)();
	
	/// Is integration mode Ito?
	bool isIto() const;
	
	/// Is integration mode Stratonovich?
	bool isStratonovich()const;
	
	/// Set integration mode to Ito.
	void setIto();
	
	/// Set integration mode to Stratonovich
	void setStratonovich();

private:
	int eqnTermAmount; // number of terms
	vector<StochasticFunction *> eqnIntegrands;     // integrand
	vector<StochasticVariable *> eqnIntegrators;   // integrator
	double eqnIncrement;                   // the current  increment
	// calculate value according to K. Ito (Euler method)
	void stepEulerIto();
	// calculate value according to Stratonovich (Euler method)
	void  stepEulerStratonovich();
};


#endif
