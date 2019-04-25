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

#include "../h/stochastic.hxx"
#include <fstream>

uint RandN::nRefs = 0;
uint32_t RandN::nState = 0;
mt19937_64 *RandN::randGenerator = 0;
int RandN::nRand = 0; // index into aRand
double RandN::aRand[2] = {0,0}; // two random variables
boost::random::uniform_real_distribution<double> RandN::dist(0.0,1.0);

RandN::RandN()
{
	// values for the gaussian distribution
	RandN::nRand = 1;
	RandN::aRand[0] = 0.0;
	RandN::aRand[1] = 0.0;
	
	// creates a random number generator of type luxury2
	// see the docu of the GSL for other possible types
	if (!RandN::randGenerator) {
		RandN::randGenerator = new boost::random::mt19937_64;
		
		// seed
		if (!RandN::nState) {
			ifstream devrandom("/dev/urandom");
			if (devrandom)
				devrandom.read((char *)&RandN::nState, sizeof(RandN::nState));
			cout << "seeding random number generator with " << RandN::nState << endl;
			randGenerator->seed( RandN::nState );
		}
	}
	RandN::nRefs++;
};
	
RandN::~RandN()
{
	RandN::nRefs--;
	if (!RandN::nRefs && RandN::randGenerator) {
		delete RandN::randGenerator;
		RandN::randGenerator = 0;
	}
	RandN::nState = 0;
};
	
 double RandN::dRandN()
 {
	 if(!RandN::nRand)
		 return RandN::aRand[++RandN::nRand];
	else {
		// Polar-Marsaglia method for normal distribution
		double test = 2.0, u1, u2, c;
		while(test>1.0) {
			u1 = dRandE();
			u2 = dRandE();
			u1 = 2.0*u1 - 1.0;
			u2 = 2.0*u2 - 1.0;
			test = u1*u1 + u2*u2;
		}
		c = sqrt(-(2.0/test)*log(test));
		RandN::aRand[0] = c*u1;
		RandN::aRand[1] = c*u2;
		
		return RandN::aRand[--RandN::nRand];
	}
};

double RandN::dRandE()
{
	return RandN::dist(*RandN::randGenerator);
};

void StochasticEventGenerator::proceedToNextState()
{
    if (stochNextStateIsPrepared) {
        stochCurrentValue = stochNextValue;
        eventCurrentValue = eventNextValue;
        stochNextStateIsPrepared = false;
    }
}
