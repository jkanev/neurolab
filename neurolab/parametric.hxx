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

#include <vector>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/function.hpp>

using namespace std;
using boost::bind;
using boost::function;

/// Class describing objects which can be parametrized.
/** Objects of this class (such as stochastic variables) can be parametrized in various ways. A paremeter (const Property&) is an integer constant which identifies the parameter. */
class Parametric
{

private:

	/// Initialisation without name is forbidden.
	Parametric() {};
	
	/// Type name of object.
	string parametricType;
	
	/// (Unique) name of object.
	string parametricName;
	
	/// List of parameters.
	vector<string> parametricStrings;
	
	/// Current indent for i/o.
	static uint parametricOutputIndent;
	
	/// Current line end for parameter output.
	static bool parametricOutputLineEnd;
	
protected:

	/// Create object of type and name.
	Parametric(const string& name, const string& type);
		
	/// Destroy object.
	virtual ~Parametric() {};
	
public:

	/// Type of object.
	string getType() const {
		return parametricType;
	}
	
	/// Name of object.
	string getName() const {
		return parametricName;
	}
	
	/// Name of object.
	void setName(const string& name) {
		parametricName = name;
	}
	
	/// Returns the configuration of the object.
	/** This includes object type and name, and a list with all settings. */
	string getConfiguration() const;
	
	/// String with all parameter settings.
	string getAllParameters() const;
	
	/// Add a parameter.
	/** Adds another string definition to the class. If the parameter already exists, it is not added. */
	void addParameter( const string& name );
		
	/// Remove a parameter.
	/** Removes the string from the parameter list. */
	void rmParameter( const string& name );
		
	/// Set parameter.
	/** Sets the value of a parameter using strings. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter (not implemented yet). */
	virtual void setParameter (
		const string& name,   ///< name of parameter
		const string& value   ///< value of parameter (used with operator<<)
	);
	
	/// Get parameter.
	/** In a derived class, override this to handle every parameter you implement. If a parameter is described using multiple strings separated by space, this indicates a parameter of a parameter.  */
	virtual string getParameter (
		const string& name   ///< name of parameter
	) const;

};


#endif
