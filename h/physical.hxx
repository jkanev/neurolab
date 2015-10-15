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

#ifndef __PHYSICAL
#define __PHYSICAL

#include <string>
#include <iostream>

 using namespace std;
 
	
/// A physical unit.
/** This is mainly a helper class for the class Physical. It describes physical units such as Volt or Meter. */
class Unit {
private:
	// true if an error occured (f.i. after adding meters to volts)
	bool bInvalid;
	
	// the exponents of the seven SI units
	int siUnits[7];
	
	// the name (like "voltage")
	string sUnitName;
	string sUnitSymbol;
	
	// coefficients - milli is x*10^-3, in which case nExpPrefix = -3
	int nExpPrefix;
	
	// checks equality of the unit array
	bool equals(int m, int kg, int s, int A, int K, int mol, int cd);
	
	// sets the unit's name and symbol
	void setNames();
	bool setNamesStandard();
public:
	/// Construct a physical unit
	/** \param exp Prefix
	\param m Meters
	\param kg Kilograms
	\param s Seconds
	\param A Ampere
	\param K Kelvin
	\param mol Mole
	\param cd Candela
	Construct a unit. The prefix's exponent (like -3 for milli, 0 for nothing or 3 for kilo), and the exponents of the individual SI units have to be given. Example: if acceleration is given as -3 km/s^2, the constructor must be called as Unit(3, 1,0,-2,0,0,0). */
	Unit(int exp, int m, int kg, int s, int A, int K, int mol, int cd);
	
	/// Construct a physical unit
	/** Unit u('m','V');*/
	Unit(string prefix, string name);
	
	/// Set values.
	/** \param exp Prefix
	\param m Meters
	\param kg Kilograms
	\param s Seconds
	\param A Ampere
	\param K Kelvin
	\param mol Mole
	\param cd Candela
	Construct a unit. The prefix's exponent (like -3 for milli, 0 for nothing or 3 for kilo), and the exponents of the individual SI units have to be given. Example: if acceleration is given as -3 km/s^2, the constructor must be called as Unit(3, 1,0,-2,0,0,0). */
	void set(int exp, int m, int kg, int s, int A, int K, int mol, int cd);
	
	/// Set values.
	/** \param m Meters
	\param kg Kilograms
	\param s Seconds
	\param A Ampere
	\param K Kelvin
	\param mol Mole
	\param cd Candela
	Construct a unit. The prefix's exponent (like -3 for milli, 0 for nothing or 3 for kilo), and the exponents of the individual SI units have to be given. Example: if acceleration is given as -3 km/s^2, the constructor must be called as Unit(3, 1,0,-2,0,0,0). */
	void set(int m, int kg, int s, int A, int K, int mol, int cd);
	
	/// Simple constructor.
	Unit();
	
	/// Copy constructor.
	Unit( const Unit& );
	~Unit();
	
	/// Get name.
	/** This retrieves the name as symbol, f.e. 'mV'. Note that you have to set the description for yourself. */
	string getSymbol();
	
	/// Get long name.
	/** This retrieves the complete name, like 'millivolt'. Note that the long name is not always existant. (A unit like s^4 will have no long name.)*/
	string getName();
	
	/// Set the prefix.
	/** Set the prefix by giving an integer from -24 to 24. Should be a multiple of 3, exception: -2,-1,2,1. */
	void setPrefix(int n);
	
	void operator+=(const Unit&);
	void operator-=(const Unit&);
	void operator*=(const Unit&);
	void operator/=(const Unit&);
	void operator=(const Unit&);
	friend bool operator==(const Unit&, const Unit&);
	friend Unit operator*(const Unit&, const Unit&);
	friend Unit operator+(const Unit&, const Unit&);
	friend Unit operator/(const Unit&, const Unit&);
	void sqrt();
	void dimensionless();
};

/// A physical value.
/** This is a 'something' that has a physical dimension (like time, weight, length...). It has a physical unit and a name (like 'voltage'). Use physical.getUnit().set(...) to set the physical unit.*/
class Physical {
protected:
	// the name, like "membrane"
	string physicalDescription;
	double dQuantity;
	Unit physicalUnit;
public:
	
	/// Construct.
	Physical();
	
	/// Copy.
	Physical(const Physical &);
	
	/// Construct.
	Physical(string name);
	
	/// Construct.
	Physical(string name, string unitPrefix, string unitSymbol);
	
	/// Construct.
	Physical( string name, Unit unit );
	
	virtual ~Physical();
	
	/// Retrive the physical name.
	/** The name is constructed from the physical name (like "membrane voltage") and the unit symbol (like "mV"). In this example is would read "membrane voltage [mV]". */
	virtual string getDescription();

	/// Returns the physical description.
	/** This is the name of the physical dimension of the object, if it has any. ("" otherwise) */
	virtual string getPhysicalDescription() { 
		return physicalDescription;
	}
	
	/// Returns the unit name.
	virtual string getUnitName() {
		return physicalUnit.getName();
	}
	
	/// Returns the unit name.
	virtual string getUnitSymbol() {
		return physicalUnit.getSymbol();
	}
	
	/// Set the name.
	/** The name of the physical quantity, like 'voltage' */
	virtual void setDescription(string name);
		
	/// Set the name. Same as setDescription().
	/** The name of the physical quantity, like 'voltage' */
	virtual void setPhysicalDescription(string name);
	
	/// Set the unit.
	virtual void setUnit(Unit u);
	
	/// Set unit prefix
	/** Set the unit prefix by giving an integer from -24 to 24. Should be a multiple of 3, exception: -2,-1,2,1. */
	virtual void setUnitPrefix(int n);
	
	/// Retrieve the unit.
	virtual Unit getUnit() const;	
};


#endif
