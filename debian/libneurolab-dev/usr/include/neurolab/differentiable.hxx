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
/*! Class implementing stochastic differential equations (SDEs). Ito-
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
   mode can be set by calling setStrat(). Which mode is used can
   be tested by querying bIsIto() (true if Ito mode) or bIsStrat() (true
   if Stratonovich integrals are used).
   At the moment there is only the 0.5-strong-order Euler scheme
   implemented, but Milstein and Taylor of higher order are planned for the future. See the main page for display of the error made when integrating. Here is a short example of the difference of Ito and Stratonovich integration (the equation is \f$dX_t = aX_tdt + bX_tdW_t\f$ or \f$dX_t = aX_tdt + bX_t\circ dW_t\f$ with \f$a=-8.0\f$ and \f$b=0.8\f$.):

   The integrands \f$f_i(X_t)\f$ are objects of the type Integrand,
   and the integrators \f$dY_i\f$ are objects of the type
   Integrator. They are stored in arrays of function objects,
   and their Integrand::operator()(double) or Integrator::operator()()
   are called each time step() is called. Use dGetValue() (from StochVar)
   or operator(double) to get the current value of the integral.
   The function step() must be used to calculate the values of the next
   time step, the function next() must be used to update the object.
   New integrators and integrands may be added easily
   by calling nAddIntegrator() or nAddIntegrand().

   Differentiable itself is of type Integrator and of type Integrand,
   so complex cascades of differential equations may be implemented.
   If you have a complex network of interdependent SDEs, you must first call
   the step() function of all objects, then the next() function of all objects.
   Otherwise circular dependencies will not get updated correctly.
   
*/
class Differentiable: public StochVariable
{
protected:
	double dX0; /// starting value
public:
	/// Constructor.
	/*! \param x0 Starting value
	\param dx0 Starting increment */
	Differentiable(double x0, double dx0);
	
	/// Constructor.
	/*! \param x0 Starting value
	\param dx0 Starting increment */
	Differentiable(const Time *time, double x0, double dx0);
	
	/// Construct.
	/*! The same as using Differentiable(0.0, 0.0). */
	Differentiable() : StochVariable(0) {
		Differentiable(0.0, 0.0);
		setIto();
	};
	
	virtual ~Differentiable(); ///< Destructor.

	/// \name Integration
	// @{
	/// Calculate value for next time step.
	/*! This just calculates the next value.
		Nothing is changed before step() is called. */
	virtual void next();
	/// Sets a starting value.
	virtual void init(double x0);
	/// Sets the active starting value
	virtual void init();
	// @}

	/// \name Setting the function
	// @{
	/// Add term.
	int nAddTerm(StochFunction *integrand, StochVariable *integrator);
	/// Remove term.
	void rmTerm(int n);
	/// Set term.
	void setTerm(int n, StochFunction *integrand, StochVariable *integrator);
	
	StochFunction *getIntegrand(int);          ///< get integrand
	StochVariable *getIntegrator(int);        ///< get integrator
	//@}

	/// \name Ito/Stratonovich mode
	// @{
	void (Differentiable::* mMethod)();   ///< member pointer to one of the privat fcns.
	bool bIsIto();                        ///< ito integrating mode?
	bool bIsStrat();                      ///< stratonovich integrating mode?
	void setIto();                        ///< set integrating mode to ito
	void setStrat();                      ///< set integrating mode to stratonovich
	// @}

private:
	int nTerms; // number of terms
	vector<StochFunction *> vIntegrand;     // integrand
	vector<StochVariable *> vIntegrator;   // integrator
	double dDiff;                   // the current  increment
	// calculate value according to K. Ito (Euler method)
	void stepEulerIto();
	// calculate value according to Stratonovich (Euler method)
	void  stepEulerStratonovich();
};


#endif
