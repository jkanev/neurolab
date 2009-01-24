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

#ifndef __PARAMETRIC_HXX
#define __PARAMETRIC_HXX

#include <iostream>

using namespace std;

/// A type for setting properties estimators.
/*! Types for use with any setProperty() or getProperty() function. Properties can be combined using OR (|). Possible values are (defined globally as const): EST_SAMPLE (take a single sample); EST_MEAN (take the mean); EST_VAR (take the variance), EST_CUR (take the curtosis). Additional properties are to set the values for distributions; these cannot be combined by OR with any other value. Use as set/getProperty( Property, double), where the Property can be EST_DIST_MIN (the lowest point of the distribution range), EST_DIST_MAX (the highest point of the distribution range), and EST_DIST_BINS (the number of BINS in the distribution recording). */
typedef int Property;


/// Class describing objects which can be parametrized.
/*! Objects of this class (such as stochastic variables) can be parametrized in various ways. A paremeter (const Property&) is an integer constant which identifies the parameter. */
class Parametric
{
public:
	Parametric() {};
	virtual ~Parametric() {};
	
	/// Set parameter.
	/*! Set a parameter if the parameter can only be turned on or off. */
	virtual void setParameter( const Property& p ) { 
		cout << "setParameter(p) not implemented" << endl;
	};
	
	/// Set parameter.
	/*! Set a parameter which is set to a double value. The usual case. */
	virtual void setParameter( const Property& p, double d ) { 
		cout << "setParameter(p, d) not implemented" << endl;
	};
	
	/// Set parameter.
	/*! Set a sub-parameter. This is interesting for classes which derive from larger differential equations. */
	virtual void setParameter( const Property& p, int n, double d ) { 
		cout << "setParameter(p, n, d) not implemented" << endl;
	};
	
	/// Get parameter.
	/*! Get a parameter which is a double. */
	virtual double dParameter( const Property& p ) {
		cout << "getParameter(p) not implemented" << endl;
		return 0.0;
	};

	/// Get parameter.
	/*! Get a sub-parameter which is a double. */
	virtual double dParameter( const Property& p, int n ) {
		cout << "getParameter(p, n) not implemented" << endl;
		return 0.0;
	};

};

#endif
