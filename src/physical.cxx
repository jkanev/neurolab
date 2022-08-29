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

#include "../h/physical.hxx"
#include <sstream>
#include <vector>

// Unit methods
Unit::Unit(int exp, int m, int kg, int s, int A, int K, int mol, int cd)
{
	bInvalid = false;
	nExpPrefix = exp;
	siUnits[0] = m;
	siUnits[1] = kg;
	siUnits[2] = s;
	siUnits[3] = A;
	siUnits[4] = K;
	siUnits[5] = mol;
	siUnits[6] = cd;
	setNames();
}

Unit::Unit( const Unit& u )
{
	nExpPrefix = u.nExpPrefix;
	for( int i=0; i<7; i++ )
		siUnits[i] = u.siUnits[i];
	bInvalid = u.bInvalid;
	setNames();
}

Unit::Unit(string prefix, string name)
{
	bInvalid = false;
	
	// the prefix
	if (prefix=="y") nExpPrefix = -24;
	else if (prefix=="z") nExpPrefix = -21;
	else if (prefix=="a") nExpPrefix = -18;
	else if (prefix=="f") nExpPrefix = -15;
	else if (prefix=="p") nExpPrefix = -12;
	else if (prefix=="n") nExpPrefix = -9;
	else if (prefix=="u") nExpPrefix = -6;
	else if (prefix=="m") nExpPrefix = -3;
	else if (prefix=="c") nExpPrefix = -2;
	else if (prefix=="d") nExpPrefix = -1;
	else if (prefix=="") nExpPrefix = 0;
	else if (prefix=="da") nExpPrefix = 1;
	else if (prefix=="h") nExpPrefix = 2;
	else if (prefix=="k") nExpPrefix = 3;
	else if (prefix=="M") nExpPrefix = 6;
	else if (prefix=="G") nExpPrefix = 9;
	else if (prefix=="T") nExpPrefix = 12;
	else if (prefix=="P") nExpPrefix = 15;
	else if (prefix=="E") nExpPrefix = 18;
	else if (prefix=="Z") nExpPrefix = 21;
	else if (prefix=="Y") nExpPrefix = 24;
	else cout << "warning: prefix " << prefix << " not known." << endl;
	
	// the basics
	if (name=="") set(0,0,0,0,0,0,0);
	else if (name=="m") set(1,0,0,0,0,0,0);
	else if (name=="kg") set(0,1,0,0,0,0,0);
	else if (name=="s") set(0,0,1,0,0,0,0);
	else if (name=="A") set(0,0,0,1,0,0,0);
	else if (name=="K") set(0,0,0,0,1,0,0);
	else if (name=="mol") set(0,0,0,0,0,1,0);
	else if (name=="cd") set(0,0,0,0,0,0,1);
	
	// special
	else if (name=="N") set(1,1,-2,0,0,0,0);
	else if (name=="Pa") set(-1,1,-2,0,0,0,0);
	else if (name=="J") set(2,1,-2,0,0,0,0);
	else if (name=="W") set(2,1,-3,0,0,0,0);
	else if (name=="C") set(0,0,1,1,0,0,0);
	else if (name=="V") set(2,1,-3,-1,0,0,0);
	else if (name=="F") set(-2,-1,4,2,0,0,0);
	else if (name=="O") set(2,1,-3,-2,0,0,0);
	else if (name=="S") set(-2,-1,3,2,0,0,0);
	else if (name=="Wb") set(2,1,-2,-1,0,0,0);
	else if (name=="T") set(0,1,-2,-1,0,0,0);
	else if (name=="H") set(2,1,-2,-1,0,0,0);
	else if (name=="lx") set(-2,0,0,0,0,0,1);
	else if (name=="kat") set(0,0,-1,0,0,1,0);
	else if (name=="Hz") set(0,0,-1,0,0,0,0);
	else cout << "warning: unit " << name << " not known." << endl;
}

Unit::Unit()
{
	bInvalid = false;
	nExpPrefix = 0;
	for( int i=0; i<7; i++ )
		siUnits[i] = 0;
}


Unit::~Unit() {}

void Unit::set(int exp, int m, int kg, int s, int A, int K, int mol, int cd)
{
	nExpPrefix = exp;
	siUnits[0] = m;
	siUnits[1] = kg;
	siUnits[2] = s;
	siUnits[3] = A;
	siUnits[4] = K;
	siUnits[5] = mol;
	siUnits[6] = cd;
	setNames();
}

void Unit::set(int s, int kg, int m, int p, int c, int cd, int ph)
{
	siUnits[0] = s;
	siUnits[1] = kg;
	siUnits[2] = m;
	siUnits[3] = p;
	siUnits[4] = c;
	siUnits[5] = cd;
	siUnits[6] = ph;
	setNames();
}

void Unit::setPrefix( int n )
{
	nExpPrefix = n;
	setNames();
}

void Unit::setNames()
{
	// standard
	bool even = true;
	if (!setNamesStandard() ) {
		// even?
		for( int i=0; i<7; i++ )
			even = even && !(siUnits[i] & 1);
		even = even && !(nExpPrefix & 1);
		
		// square
		if( even ) {
			for( int i=0; i<7; i++ )
				siUnits[i] /= 2;
			nExpPrefix /= 2;
			even = setNamesStandard();
			for( int i=0; i<7; i++ )
				siUnits[i] *= 2;
			nExpPrefix *= 2;
			sUnitName = " (square) " + sUnitName;
			sUnitSymbol = "(" + sUnitSymbol + ")^2";
		}
	}
	
	// exotic
	if (!even ) {
		stringstream name;
		stringstream symbol;
		vector<string> unitNames;
		unitNames.push_back("m");
		unitNames.push_back("kg");
		unitNames.push_back("s");
		unitNames.push_back("A");
		unitNames.push_back("K");
		unitNames.push_back("mol");
		unitNames.push_back("cd");
		for (int i=0; i<7; ++i) {
			if (siUnits[i] != 0) {
				symbol << unitNames[i];
				if (siUnits[i] != 1)
					symbol << "^" << siUnits[i];
			}
		}
		sUnitName += name.str();
		sUnitSymbol += symbol.str();
	}
	
}

bool Unit::setNamesStandard()
{
	bool match = false;
	
	if (bInvalid) {
		sUnitName = "unit unvalid";
		sUnitSymbol = "uv";
		return true;
	}
	
	stringstream symbol;
	stringstream name;
	
	// the prefix
	switch(nExpPrefix) {
		case -24:
			symbol << 'y'; name << "yocto"; break;
		case -21:
			symbol << 'z'; name << "zepto"; break;
		case -18:
			symbol << 'a'; name << "atto"; break;
		case -15:
			symbol << 'f'; name << "femto"; break;
		case -12:
			symbol << 'p'; name << "pico"; break;
		case -9:
			symbol << 'n'; name << "nano"; break;
		case -6:
			symbol << 'u'; name << "micro"; break;
		case -3:
			symbol << 'm'; name << "milli"; break;
		case -2:
			symbol << 'c'; name << "centi"; break;
		case -1:
			symbol << 'd'; name << "deci"; break;
		case 1:
			symbol << "da"; name << "deca"; break;
		case 0:
			symbol << ""; name << ""; break;
		case 2:
			symbol << 'h'; name << "hecto"; break;
		case 3:
			symbol << 'k'; name << "kilo"; break;
		case 6:
			symbol << 'M'; name << "mega"; break;
		case 9:
			symbol << 'G'; name << "giga"; break;
		case 12:
			symbol << 'T'; name << "tera"; break;
		case 15:
			symbol << 'P'; name << "peta"; break;
		case 18:
			symbol << 'E'; name << "exa"; break;
		case 21:
			symbol << 'Z'; name << "zetta"; break;
		case 24:
			symbol << 'Y'; name << "yotta"; break;
		default:
			symbol << "10^" << nExpPrefix << ' '; break;
	}
/*	sUnitName = name.str();
	sUnitSymbol = symbol.str();
	name.clear();
	symbol.clear();
*/	
	// the basics
	if (equals(0,0,0,0,0,0,0)) {
		symbol << "";
		name << ""; // was "no unit"
		match = true;
	} else if (equals(1,0,0,0,0,0,0)) {
		symbol << "m";
		name << "meter";
		match = true;
	} else if (equals(0,1,0,0,0,0,0)) {
		symbol << "kg";
		name << "kilogram";
		match = true;
	} else if (equals(0,0,1,0,0,0,0)) {
		symbol << "s";
		name << "second";
		match = true;
	} else if (equals(0,0,0,1,0,0,0)) {
		symbol << "A";
		name << "ampere";
		match = true;
	} else if (equals(0,0,0,0,1,0,0)) { 
		symbol << "K";
		name << "kelvin";
		match = true;
	} else if (equals(0,0,0,0,0,1,0)) {
		symbol << "mol";
		name << "mole";
		match = true;
	} else if (equals(0,0,0,0,0,0,1)) {
		symbol << "cd";
		name << "candela";
		match = true;
	}
	
	// special
	else if (equals(1,1,-2,0,0,0,0)) {
		symbol << 'N';
		name << "newton";
		match = true;
	} else if (equals(-1,1,-2,0,0,0,0)) {
		symbol << "Pa";
		name << "pascal";
		match = true;
	} else if (equals(2,1,-2,0,0,0,0)) {
		symbol << 'J';
		name << "joule";
		match = true;
	} else if (equals(2,1,-3,0,0,0,0)) {
		symbol << 'W';
		name << "watt";
		match = true;
	} else if (equals(0,0,1,1,0,0,0)) {
		symbol << 'C';
		name << "coulomb";
		match = true;
	} else if (equals(2,1,-3,-1,0,0,0)) {
		symbol << 'V';
		name << "volt";
		match = true;
	} else if (equals(-2,-1,4,2,0,0,0)) {
		symbol << 'F';
		name << "farad";
		match = true;
	} else if (equals(2,1,-3,-2,0,0,0)) {
		symbol << 'O'; // should we do something like \Omega?
		name << "ohm";
		match = true;
	} else if (equals(-2,-1,3,2,0,0,0)) {
		symbol << 'S';
		name << "siemens";
		match = true;
	} else if (equals(2,1,-2,-1,0,0,0)) {
		symbol << "Wb";
		name << "weber";
		match = true;
	} else if (equals(0,1,-2,-1,0,0,0)) {
		symbol << 'T';
		name << "tesla";
		match = true;
	} else if (equals(2,1,-2,-1,0,0,0)) {
		symbol << 'H';
		name << "henry";
		match = true;
	} else if (equals(-2,0,0,0,0,0,1)) {
		symbol << "lx";
		name << "lux";
		match = true;
	} else if (equals(0,0,-1,0,0,1,0)) {
		symbol << "kat";
		name << "katal";
		match = true;
	} else if (equals(0,0,-1,0,0,0,0)) {
		symbol << "Hz";
		name << "Hertz";
		match = true;
	}
	
	sUnitName = name.str();
	sUnitSymbol = symbol.str();
	return match;
}

// checks equality of the unit array
bool Unit::equals(int m, int kg, int s, int A, int K, int mol, int cd)
{
	if( (siUnits[0] ==m)
			&& (siUnits[1]==kg)
			&& (siUnits[2]==s)
			&& (siUnits[3]==A)
			&& (siUnits[4]==K)
			&& (siUnits[5]==mol)
			&& (siUnits[6]==cd) )
		return true;
	else return false;
}

void Unit::operator+=(const Unit& u)
{
	if(!bInvalid) {
		for(int i=0; i<7; i++)
			if(siUnits[i] != u.siUnits[i])
				bInvalid = true;
		if(nExpPrefix != u.nExpPrefix)
			bInvalid = true;
	} else 
		return;
	if(bInvalid)
		cout << "warning: trying to add "
			<< u.sUnitName << " (" << u.sUnitSymbol << ") to "
			<< sUnitName << " (" << sUnitSymbol << ")" 
			<< endl;
}


void Unit::operator-=(const Unit& u)
{
	if( !bInvalid ) {
		for(int i=0; i<7; i++)
			if(siUnits[i] != u.siUnits[i])
				bInvalid = true;
		if(nExpPrefix != u.nExpPrefix)
			bInvalid = true;
	} else
		return;
	if(bInvalid)
		cout << "warning: trying to subtract "
			<< u.sUnitName << " (" << u.sUnitSymbol << ") from "
			<< sUnitName << " (" << sUnitSymbol << ")" 
			<< endl;
}

void Unit::operator*=(const Unit& u)
{
	nExpPrefix += u.nExpPrefix;
	for(int i=0; i<7; i++)
		siUnits[i] += u.siUnits[i];
	setNames();
}

void Unit::operator/=(const Unit& u)
{
	nExpPrefix -= u.nExpPrefix;
	for(int i=0; i<7; i++)
		siUnits[i] -= u.siUnits[i];
	setNames();
}

void Unit::sqrt()
{
	bool warning = false;
	if (nExpPrefix & 1)
		warning = true;
	nExpPrefix /= 2;
	for(int i=0; i<7; i++) {
		if (siUnits[i] & 1)
			warning = true;
		siUnits[i] /= 2;
	}
	if (warning)
		cout << "warning: trying to do sqrt(" << sUnitName << ")" << endl;
	setNames();
}

void Unit::dimensionless()
{
	setNames();
	bool warning = false;
	for(int i=0; i<7; i++) {
		if (siUnits[i])
			warning = true;
		siUnits[i] = 0;
	}
	if (warning)
		cout << "warning: using(" << sUnitName << ") as argument to a dimensionless function." << endl;
	setNames();
}

void Unit::operator=(const Unit& u)
{
	nExpPrefix = u.nExpPrefix;
	for(int i=0; i<7; i++)
		siUnits[i] = u.siUnits[i];
	bInvalid = u.bInvalid;
	setNames();
}

bool operator==(const Unit& u1, const Unit& u2)
{
	bool b = false;
	for(int i=0; i<7; i++)
		b = b && (u1.siUnits[i] == u2.siUnits[i]);
	b = b && (u1.nExpPrefix ==u2.nExpPrefix);
	
	return b;
}

Unit operator+(const Unit& u1, const Unit& u2)
{
	Unit u;
	if (!u1.bInvalid && !u2.bInvalid) {
		for(int i=0; i<7; i++)
			if(u1.siUnits[i] != u2.siUnits[i])
				u.bInvalid = true;
		if(u1.nExpPrefix != u2.nExpPrefix)
			u.bInvalid = true;
	} else 
		u.bInvalid = true;
		
    if (u.bInvalid)
        cout << "warning: trying to add "
                << u2.sUnitName << " (" << u2.sUnitSymbol << ") to "
                << u1.sUnitName << " (" << u1.sUnitSymbol << ")" 
                << endl;
	
		return u;
}

Unit operator*(const Unit& u1, const Unit& u2)
{
	Unit u(u1);
	u *= u2;
	return u;
}

Unit operator/(const Unit& u1, const Unit& u2)
{
	Unit u(u1);
	u /= u2;
	return u;
}

string Unit::getName()
{
	return sUnitName;
}

string Unit::getSymbol()
{
	return sUnitSymbol;
}

// physical methods
Physical::Physical()
{
	dQuantity = 0;
}

Physical::Physical( string description )
{
	physicalDescription = description;
}

Physical::Physical(const Physical &p)
{
	physicalDescription = p.physicalDescription;
	dQuantity = p.dQuantity;
	physicalUnit = p.physicalUnit;
}
	
Physical::Physical( string name, Unit unit )
{
	physicalDescription = name;
	physicalUnit = unit;
}

Physical::Physical( string name, string unitPrefix, string unitSymbol )
{
	physicalDescription = name;
	physicalUnit = Unit(unitPrefix, unitSymbol);
}

Physical::~Physical()
{}

Unit Physical::getUnit() const
{
	return physicalUnit;
}

void Physical::setUnit(Unit u)
{
	physicalUnit = u;
}

string Physical::getDescription()
{
	if (physicalUnit.getName().length())
        return physicalDescription + " (" + physicalUnit.getSymbol() + ")";
	else return physicalDescription;
}

void Physical::setDescription(string name)
{
	physicalDescription = name;
}

void Physical::setPhysicalDescription(string name)
{
	physicalDescription = name;
}

void Physical::setUnitPrefix(int n)
{
	physicalUnit.setPrefix(n);
}
