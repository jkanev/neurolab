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

#ifndef __NOISES_HXX
#define __NOISES_HXX

#include "matrix.hxx"
#include "stochastic.hxx"

using namespace std;

const Property NOISE_WIENER = 0;
const Property NOISE_POISSON = 1;
const Property NOISE_ORNSTEIN_UHLENBECK = 2;
const Property NOISE_THRESHOLD = 3;

/// Inverse error function.
/*! This helper is needed for threshold estimation. */
double erfinv(double);


/// A correlated noise source.
/*! This is a noise source made by the NoiseSource class. These noise sources are used as stochastic variables (StochVar) in differential equations (Differentiable). Note that they cannot be set active, as they are driven by the NoiseSource object which made the class. These objects are either of type PoissonNoise, WienerNoise or OUNoise. The difference does only matter for the NoiseSource class, which sets their values accordingly. 
\todo
Finish implementing the noise class. */

class Noise: public StochVariable
{	
protected:
	
	Property pType; // type of noise
	class NoiseSource *pParent; // the noises object which has produced this object
	int nIndex; // the index into the parent's object arrays
	double dNoiseWeight;
	double dNoiseRate;
	double dNoiseSigma;
	double dNoiseMu;
	
	/// Construct.
	/*! This constructor is protected, because this class should only be instantiated by calls from the NoiseSource class. 
	\param parent The generating noise source
	\param index The number in the parent's noise tables */
	Noise(class NoiseSource *parent, int index);
	
	/// Construct.
	/*! This class mustn't be copied. */
	Noise( const Noise & );
	
	virtual void setNext(double d) = 0;
	
public:
	~Noise();
	
	/// Make next value current value.
	virtual void step() {};
	
	/// Compute next value.
	virtual void next() {};
	
	/// Set the weight.
	virtual void setWeight(double w);
	/// Get the weight.
	double dWeight() {return dNoiseWeight;};
	
	/// Set the rate.
	virtual void setRate(double r);
	/// Get the rate
	double dRate() {return dNoiseRate; };
	
	/// Set the mean.
	virtual void setMu(double m);
	/// Get the mean.
	double dMu() {return dNoiseMu;};
	
	/// Set the standard deviation coefficient.
	virtual void setSigma(double s);
	double dSigma() {return dNoiseSigma;};
	
	/// Set a property.
	/*! \param p The property.
	\param d The value.
	Possible properties are NOISE_MEAN, NOISE_SIGMA, NOISE_WEIGHT, NOISE_RATE. */
	virtual void setParameter( const Property& p, double d );
	
	/// Get a property.
	/* \param p The property.
	\returns The value of the property.
	Possible properties are NOISE_MEAN, NOISE_SIGMA, NOISE_WEIGHT, NOISE_RATE.*/
	virtual double dParameter( const Property& p );
	
friend class NoiseSource;
};

const Property NOISE_MEAN = 4;
const Property NOISE_SIGMA = 5;
const Property NOISE_WEIGHT = 6;
const Property NOISE_RATE = 7;


/// A poisson noise source.
/*! Internal class: You never need to acces this class directly, since all public members are defined in Noise. This special derivation of Noise is only important internally, for the NoiseSource class. */

class PoissonNoise: public Noise
{
private:
	double dThreshold;
	void setThreshold();
protected:
	/// Construct.
	/*! This constructor is protected, because this class can only be instantiated by calls from the NoiseSource class. */
	PoissonNoise(class NoiseSource *, int index);
	
	/// Construct.
	/*! This class mustn't be copied. */
	PoissonNoise( const PoissonNoise & );
	
	/// Set the next values.
	/*! The main method. The NoiseSource class uses this to set the indicator values. If the indicator is above the current threshold, the value is set to w (the weight of the process), otherwise to 0.0. */
	virtual void setNext(double indicator);
	
public:
	~PoissonNoise();
	
	/// Set the weight.
	virtual void setWeight(double w);
	
	/// Set the rate.
	virtual void setRate(double r);
	
	/// Set the mean.
	virtual void setMu(double m);
	
	/// Set the standard deviation coefficient.
	virtual void setSigma(double s);
	
	/// Set parameter.
	/*! In addition to the parameters from the Noise class, the Poisson class understands NOISE_THRESHOLD. */
	void setParameter(const Property &p, double d);
	
	/// Get parameter.
	/*! In addition to the parameters from the Noise class, the Poisson class understands NOISE_THRESHOLD. */
	double dParameter(const Property &p);
	
friend class NoiseSource;
};


/// A Wiener process (white) noise source.
/*! Internal class: You never need to acces this class directly, since all public members are defined in Noise. This special derivation of Noise is only important internally, for the NoiseSource class. */

class WienerNoise: public Noise
{
private:
	double dSqrtDt; // square root of time step
	
protected:
	/// Construct.
	/*! This constructor is protected, because this class can only be instantiated by calls from the NoiseSource class. */
	WienerNoise(class NoiseSource *parent, int index);
	
	/// Construct.
	/*! This class mustn't be copied. */
	WienerNoise( const WienerNoise & );
	
	/// Set the next values.
	/*! \param indicator The stochastic indicator.
	This is the main method. The NoiseSource class uses this to set the indicator values. If the indicator is above the current threshold, the value is set to w (the weight of the process), otherwise to 0.0. */
	virtual void setNext(double indicator);
	
public:
	~WienerNoise();
	
friend class NoiseSource;
};


/// A set of mutually correlated noise sources (Wiener, Poisson and Ornstein-Uhlenbeck processes).
/*! An object of this class creates various noise source which may be correlated. The correlation is achieved by mixing independent noise sources with a mixing matrix (details below).

Construction and usage: The constructor will set up an object with n noise sources. They can be used straight away. The mixing matrix, as well as type and parameters for each process can be set at runtime. Mean, variance and covariation processes can also be obtained at runtime.

Method: 
\f[ \omega -> dXt -> dPt / dWt \f] 
     
\f[ \left<\left[G^i,G^j\right]\right> = \sigma_jc_{ij} c_j w_j\lambda_j \f]
with
\f[ c_j = \sqrt{\frac{2}{\pi}} \frac {e^{-\frac{\lambda_j^2dt}{2\mbox{Var}\left\{dX^i_t\right\}}}} {\left(1-\mbox{erf}{\frac{\lambda_j\sqrt{dt}}{\sqrt{2\mbox{Var}\left\{dX^i_t\right\}}}}\right)\sqrt{\mbox{Var}\left\{dX^i_t\right\}}}.   \f]

*/

class NoiseSource: public Stochastic, public RandN
{
public:
//	private:
	// for perfomance reasons we'll use normal arrays only.
	double *aOmega; // first indicator variables
	double **aA; // the mixing matrix
	class Noise **aNoises; // the noise objects
	int nNoises; // number of noises in arrays
	
public:
	/// Construct.
	/*! Constructs a NoiseSource object, which produces n noise processes. The default mixing matrix is the identity matrix (Zeros with diagonal Ones). Use setMixingMatrix() to change. */
	NoiseSource(
		const Time *time, ///< Adress of a (central) time object.
		int n ///< Number of processes.
	);
	
	/// Construct.
	/*! Constructs a NoiseSource object, where the processes are mixed as stated in the given mixing matrix. */
	NoiseSource(
		const Time *time, ///< Adress of a (central) time object.
		Matrix a ///< Mixing matrix.
	);
	
	/// Destruct.
	~NoiseSource();
	
	/// Set/Get a noise object.
	/*! This will retrieve the address of a noise object. If the object is not existent, or of a different type as stated by the p, it will be created.*/
	Noise *xNoise(
		int n, ///< The index of the noise.
		const Property& p ///< The type of the noise (can be NOISE_WIENER, NOISE_POISSON or NOISE_ORNSTEIN-UHLENBECK).
	);
	
	/// Get a noise object.
	/*! This returns only existent noise objects. */
	Noise *operator[](
		int n ///< The index of the noise.
	);
		
	/// Set the mixing matrix.
	/*! Just sets a new mixing matrix to a given set of noises. Note that the dimensions of the matrix must match (i.e. for n noise sources, you want an nxn Matrix. */
	void setMixingMatrix(
		Matrix a ///< The mixing matrix.
	);	
	
	/// Get variance.
	double dIndicatorVariance( int n );
	
	/// Get covariation coefficient.
	/*! The covariation process (see Covariation) is in many cases just time multiplied by a scalar. The method retrieves this scalar. 
	\returns The scalar s, so that \f$ \left[X^n,X^m\right]_t = st\f$*/
	double dCovariationCoeff(
		int n, ///< Index of first noise process.
		int m ///< Index of second noise process.
	);
	
	/// Callback for Noise objects.
	/*! Called by a noise object n, to notify its parent that it has died. */
	void destructionNotify(int n);
	
	/// Compute next value.
	/*! This will compute the next value of all indicator variables. The values of the noises are not affected.  */
	virtual void next();
	
	/// Set next time step.
	/*! This will set all noises to the next time step. */
	virtual void step();	
};


#endif
