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

#include "../h/noises.hxx"
#include <cmath>


// Inverse error function.
double erfinv(double target)
{
	double stepsize = 0.4; // starting stepsize
	double source = 0.0;
	int direction = 1; // current direction 1-up, -1-down
	double error = 0.000000001;
	
	if( target > 1.0 || target < -1.0 ) {
		cout << "argument for erfinv outside of domain" << endl;
		return 0.0; // I mean NaN
	}
	if( target==0.0 ) return 0.0;
	if( target==1.0 ) return 1e24; // replacement for inf
	if( target==-1.0 ) return -1e24; // replacement for -inf
	
	int k;
	for( k=0; k<1000000; k++) {
		double current = erf( source );
		
		if(current > target) {
			if(direction == 1) {
				// too large and direction up - decrease stepsize and turn
				stepsize *= 0.6;
				direction *= -1;
			} else
				// too large and direction down - increase stepsize
				stepsize *= 1.1;
		} else {
			if(direction == -1) {
				// too small and direction down - decrease stepsize and turn
				stepsize *= 0.6;
				direction *= -1;
			} else
				// too small and direction up - increase stepsize
				stepsize *= 1.1;
		}
		source += direction * stepsize;
		
		// early stopping
		if( stepsize < error )
			break;
	}
/*	// note
	cout << "erfinv(" << target 
		<< ") = " << source 
		<< ", stepsize = " << stepsize 
		<< ", " << k << " steps, "
		<< " error = " << erf(source) - target << endl;
*/	
	return source;
}



//__________________________________________________________________________
//   Construct
Noise::Noise(NoiseSource *parent, int index, const string& name, const string& type)
	 : StochasticVariable(parent->getTime(), name, type)
{
	nIndex = index;
	pParent = parent;
	noiseWeight = 1.0;
	noiseRate = 1.0; // -> 1 event per ms
	noiseSigma = 1.0;
	noiseMu = 1.0;
	addParameter("mean");
	addParameter("std.dev.");
	addParameter("rate");
	addParameter("weight");
};	
	
Noise::Noise( const Noise &noise)
	: StochasticVariable(noise.xTime)
{}

//__________________________________________________________________________
//   Destruct.
Noise::~Noise()
{
	// notify parent of our destruction
	if(pParent)
		pParent->destructionNotify( nIndex );
};
	
//__________________________________________________________________________
//   set parameter

void Noise::setParameter( const string& name, const string& value )
{
	stringstream param;
	double d;
	param << value;
	param >> d;
	
	if (name == "mean")
		setMean(d);
	else if (name == "std.dev.")
		setStdDev(d);
	else if (name == "weight")
		setWeight(d);
	else if (name == "rate")
		setRate(d);
	else
		StochasticVariable::setParameter(name, value);
};


//__________________________________________________________________________
//   get parameter

string Noise::getParameter( const string& name ) const
{
	stringstream param;
	if (name == "mean")
		param << noiseMu;
	else if (name == "std.dev.")
		param << noiseSigma;
	else if (name == "weight")
		param << noiseWeight;
	else if (name == "rate")
		param << noiseRate;
	else
		param << StochasticVariable::getParameter(name);
		
	return param.str();
};



//__________________________________________________________________________
//   set mean

Noise &Noise::setMean(double mu)
{
	noiseMu = mu;
	noiseRate = (noiseMu*noiseMu) / (noiseSigma*noiseSigma);
	noiseWeight = (noiseSigma*noiseSigma) / noiseMu;

	return *this;
};


//__________________________________________________________________________
//   set std. deviation

Noise &Noise::setStdDev(double sigma)
{
	noiseSigma = sigma;
	noiseRate = (noiseMu*noiseMu) / (noiseSigma*noiseSigma);
	noiseWeight = (noiseSigma*noiseSigma) / noiseMu;

	return *this;
};


//__________________________________________________________________________
//   set weight

Noise &Noise::setWeight(double weight)
{
	noiseWeight = weight;
	noiseMu = noiseRate*noiseWeight;
	noiseSigma = sqrt(noiseRate) * noiseWeight;

	return *this;
};


//__________________________________________________________________________
//   set rate

Noise &Noise::setRate(double lambda)
{
	noiseRate = lambda;
	noiseMu = noiseRate*noiseWeight;
	noiseSigma = sqrt(noiseRate) * noiseWeight;

	return *this;
};



//__________________________________________________________________________
//   poisson noise source

//__________________________________________________________________________
//   construct / destroy

PoissonNoise::PoissonNoise(NoiseSource *parent, int index, const string& name, const string& type)
	: Noise(parent, index, name, type)
 {
	setThreshold();
}

PoissonNoise::PoissonNoise(const PoissonNoise &noise)
	: Noise(noise)
 {
	setThreshold();
}

PoissonNoise::~PoissonNoise() {};
	
//__________________________________________________________________________
//   set/get parameters

Noise &PoissonNoise::setWeight( double weight )
{
	Noise::setWeight(weight);
	setThreshold();

	return *this;
}

Noise &PoissonNoise::setRate( double rate )
{
	Noise::setRate(rate);
	setThreshold();

	return *this;
}

Noise &PoissonNoise::setStdDev( double sigma )
{
	Noise::setStdDev(sigma);
	setThreshold();

	return *this;
}

Noise &PoissonNoise::setMean( double mu )
{
	Noise::setMean(mu);
	setThreshold();

	return *this;
}

void PoissonNoise::setThreshold()
{
	double var = pParent->getIndicatorVariance(nIndex);
	poissonThreshold = erfinv(1.0 - 2.0*noiseRate*xTime->dt) * sqrt(2*var);
//	cout << "threshold " << dThreshold << " for rate " << noiseRate << " with indicator variance " << var << endl;
}

double PoissonNoise::getThreshold()
{
	return poissonThreshold;
}

//__________________________________________________________________________
//   set indicator

void PoissonNoise::setNext(double omega)
{
	stochCurrentValue = stochNextValue;
	if( omega > poissonThreshold )
		stochNextValue += noiseWeight;
}

//////////////////////////////////////////////////
//   Wiener Noise Source
//////////////////////////////////////////////////

//__________________________________________________________________________
//   construct

WienerNoise::WienerNoise(NoiseSource *parent, int index, const string& name, const string& type)
	: Noise(parent, index, name, type)
{
	dSqrtDt = sqrt(xTime->dt);
}
	
	
//__________________________________________________________________________
//   destruct

WienerNoise::~WienerNoise()
{}

//////////////////////////////////////////////////
//   construct
WienerNoise::WienerNoise( const WienerNoise &w )
	: Noise( w )
{}
	
//////////////////////////////////////////////////
//   calculate next value
void WienerNoise::setNext(double omega)
{
	stochCurrentValue = stochNextValue;
	stochNextValue += noiseMu * xTime->dt + noiseSigma * omega * dSqrtDt;
}



//////////////////////////////////////////////////
//   NoiseSource
//////////////////////////////////////////////////

//////////////////////////////////////////////////
// Construct.
NoiseSource::NoiseSource( Time *time,  int n )
	: StochasticProcess(time)
{
	nNoises = n;
	
	// noise array
	aNoises = new Noise *[nNoises];
	for(int i=0; i<n; i++)
		aNoises[i] = 0;
	
	// mixing array
	aA = new double *[nNoises];
	for(int i=0; i<n; i++) {
		aA[i] = new double[nNoises];
		for(int j=0; j<nNoises; j++)
			if (i==j)
				aA[i][j] = 1.0 / sqrt( double(nNoises) );
			else
				aA[i][j] = 0.0;
	}
	
	// omega array
	aOmega = new double[nNoises];
	for(int i=0; i<n; i++)
		aOmega[i] = dRandN();	
}

//////////////////////////////////////////////////
// Construct.
NoiseSource::NoiseSource(Time *time, Matrix a )
	: StochasticProcess(time)
{
	setMixingMatrix(a);
}

//////////////////////////////////////////////////
// Destruct.
NoiseSource::~NoiseSource()
{
	// noises array
	for(int i=0; i<nNoises; ++i)
		if(aNoises[i])
			delete aNoises[i];
	delete[] aNoises;
	
	// mixing array
	for(int i=0; i<nNoises; i++)
		delete[] aA[i];
	delete[] aA;
	
	// omega array
	delete[] aOmega;
}

//////////////////////////////////////////////////
//   Set the mixing matrix.
void NoiseSource::setMixingMatrix( Matrix a )
{
	if(a.nDimension() != 2) {
		cout << "NoiseSource::setMixingMatrix(Matrix): Matrix has " 
			<< a.nDimension() 
			<< " dimension(s), instead of two." << endl;
		return;
	}
	if(a.nSize(0) != nNoises)  {
		cout << "NoiseSource::setMixingMatrix(Matrix): Matrix' first dimension has size of " 
			<< a.nSize(0) 
			<< " instead of " 
			<< nNoises << endl;
		return;
	}
	if(a.nSize(1) != nNoises)  {
		cout << "NoiseSource::setMixingMatrix(Matrix): Matrix' first dimension has size of " 
			<< a.nSize(1) 
			<< " instead of " 
			<< nNoises << endl;
		return;
	}
	for(int i=0; i<nNoises; ++i)
		for(int j=0; j<nNoises; ++j)
			aA[i][j] = a[i][j].to_d();
}
	
//////////////////////////////////////////////////
//   Get indicator variance.
double NoiseSource::getIndicatorVariance( int n )
{
	double var = 0;
	for(int i=0; i<nNoises; ++i)
		var += aA[n][i] * aA[n][i];
	return var;
}
	
//////////////////////////////////////////////////
//   Get covariation coefficient.
double NoiseSource::getCovariationCoeff( int n, int m )
{
	// correlation factor
	double c_nm = 0.0;
	for(int i=0; i<nNoises; ++i)
		c_nm += aA[n][i] * aA[m][i];
	
	// correction offset for large time steps
	double stepCorrection = xTime->dt * aNoises[n]->getMean() * aNoises[m]->getMean();
	stepCorrection = 0.0;

	// two wiener noises: sigma_n * sigma_m * c_nm
	if (aNoises[n]->getType()=="correlated_wiener_process" && aNoises[m]->getType()=="correlated_wiener_process")
		return stepCorrection + c_nm * aNoises[n]->getStdDev() * aNoises[m]->getStdDev();

	// wiener and poisson: sigma_n * (w_m * rate_m * s_m) * c_nm
	if (aNoises[n]->getType()=="correlated_wiener_process" && aNoises[m]->getType()=="correlated_poisson_process") {
		double var = getIndicatorVariance(m);
		double pi = 3.141592653589793115997963468544185161590576171875;
		double theta = ((PoissonNoise*)aNoises[m])->getThreshold() / sqrt(2.0 * var);
		double cndSum = -sqrt(2.0*var/pi); // conditional sum: < x | x>theta >
		cndSum *= exp(- theta*theta);
		cndSum /= erf(theta) - 1;
		double s_m = cndSum / var;
		
		return stepCorrection + sqrt(xTime->dt) * c_nm * aNoises[n]->getStdDev() * s_m * aNoises[m]->getMean();
	}
	
	// poisson and wiener: (w_n * rate_n * s_n) * sigma_m * c_nm
	if (aNoises[n]->getType()=="correlated_poisson_process" && aNoises[m]->getType()=="correlated_wiener_process") {
		double var = getIndicatorVariance(n);
		double pi = 3.141592653589793115997963468544185161590576171875;
		double theta = ((PoissonNoise*)aNoises[m])->getThreshold();
		theta /= sqrt(2.0 * var);
		double cndSum = sqrt(2.0*var/pi); // conditional sum: < x | x>theta >
		cndSum *= exp(- theta*theta);
		cndSum /= 1 - erf(theta);
		double s_n = cndSum / var;
		
		return stepCorrection + sqrt(xTime->dt) * c_nm * s_n * aNoises[n]->getMean() * aNoises[m]->getStdDev();
	}
	
	// poisson and poisson: (w_n * rate_n) * (w_m * rate_m) * c_nm
	if (aNoises[n]->getType()=="correlated_poisson_process" && aNoises[m]->getType()=="correlated_poisson_process") {
		if (n==m)
			return aNoises[n]->getWeight() * aNoises[m]->getWeight() * aNoises[n]->getRate();
		else
			return // this is not correct. Use numerical integration to cut gaussian dists. in
					// multidimensional space.
				xTime->dt
				* c_nm / (getIndicatorVariance(n) * getIndicatorVariance(m))
				* sqrt(aNoises[n]->getRate() * aNoises[m]->getRate()) 
				* aNoises[n]->getWeight() * aNoises[m]->getWeight();
	}
	
	// ornstein-uhlenbeck processes involved - covariation process vanishes
	return 0.0; 
}

//////////////////////////////////////////////////
//   get noise source object

Noise *NoiseSource::createNoise( int n, const string &type )
{
	if( !aNoises[n] ) {
		stringstream name;
		if (type=="correlated_wiener_process") {
			name << "Wiener process" << "[" << n << "]" << flush;
			aNoises[n] = new WienerNoise(this, n, name.str());
		}
		else if (type=="correlated_poisson_process") {
			name << "Poisson process" << "[" << n << "]" << flush;
			aNoises[n] = new PoissonNoise(this, n, name.str());
		}
		else if (type=="correlated_ornstein_uhlenbeck_process") {
			cout << "Ornstein-Uhlenbeck process not implemented yet." << endl;
			name << "Wiener process" << "[" << n << "]" << flush;
			aNoises[n] = new WienerNoise(this, n, name.str());
		}
		if( aNoises[n] )
			aNoises[n]->setDescription(name.str());
	}
	 
	return aNoises[n];
}

//////////////////////////////////////////////////
//   get noise source object
Noise *NoiseSource::operator[]( int n )
{
	if (n<nNoises)
		return aNoises[n];
	else
		return 0;
}

//////////////////////////////////////////////////
//   noise n  is dead
void NoiseSource::destructionNotify( int n )
{
		aNoises[n] = 0;
}

//////////////////////////////////////////////////
//   next indicator values
void NoiseSource::prepareNextState()
{
	for(int i=0; i<nNoises; ++i)
		aOmega[i] = dRandN();
	stochNextStateIsPrepared = true;
}

//////////////////////////////////////////////////
//   proceed all noises
void NoiseSource::proceedToNextState()
{
	for(int i=0; i<nNoises; ++i) {
		if(aNoises[i]) {
			double omega = 0.0;
			
			// sum up next value
			for(int n=0; n<nNoises; ++n)
				omega += aA[i][n] * aOmega[n];
			
			// notify noise process
			aNoises[i]->setNext(omega);
		}
	}
}
