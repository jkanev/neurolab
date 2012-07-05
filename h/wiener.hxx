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

#ifndef __WIENER_H__
#define __WIENER_H__

#include "stochastic.hxx"
#include "differentiable.hxx"

using namespace std;

/// Properties for all Wiener processes
/*const Property WIENER_DT = 0; ///< time step for Wiener process
const Property WIENER_MEAN = 1; ///< mean of Wiener process
const Property WIENER_VARIANCE = 2; ///< variance of Wiener process
const Property CPP_W = 3; ///< single weight of condensed Poisson process
const Property CPP_LAMBDA = 4; ///< single rate of condensed Poisson process
const Property CPP_N = 5; ///< number of processes in condensed Poisson process
const Property CPP_LAMBDA_RC = 6; ///< relative coincidence rate in condensed Poisson process
const Property CPP_N_RC = 7; ///< relative coincidence width in condensed Poisson process
*/
/// Standard Functions of Wiener processes.
/** This class implements a Wiener processes. The process can have a time-proportional mean \f$\mu t\f$ and variance \f$\sigma^2 t\f$. */
class Wiener: public RandN, public StochasticVariable
{
private:
	double wienerDiff;
	double wienerSum;

protected:
	double wienerMean;
	double wienerStdDev;
	double wienerSqrtDt;

public:
	Wiener(class Time *time, const string& name="", const string& type="Wiener Process");
	~Wiener(){};
	virtual void init();
	virtual void prepareNextState();
	
	virtual double getMean();
	virtual double getVariance();
	virtual double getStdDev();
	virtual void setMean(double);
	virtual void setVariance(double);
	virtual void setStdDev(double);
	virtual double getDelta();
	virtual string getParameter(const string&) const;
	virtual void setParameter(const string&, const string&);
};

/// Condensed Poisson Process
/** Lets assume a set of Poisson process \f$s^i_t, i=1..n\f$, where \f$t\f$ denotes time, and \f$i\f$ is the index of the process. Each process is multiplied by a weight \f$w_i\f$, \f$s^i_t\f$ takes values of \f$0\f$ or \f$1\f$ (where \f$1\f$ indicates an event), and events occur with a rate of \f$\lambda_i\f$. We are interested in the sum \f[s_t = \sum_i w_i s^i_t.\f] 
Let's assume that in this collection of events coincident events happen. Coincidences are  events which occur at the same time at various processes. If these events are viewed as a separate process \f$s^c_i\f$ with a weight \f$w_c\f$ and a rate \f$\lambda_c\f$, all processes stay independent. 
The impact of the coincidence events depends on the sum of the weights of the processes taking part. We shall call the sum of the weights of the processes which take part the <i>Coincidence Weight</i>. The coincidences occur with a specific rate which we will call <i>Coincidence Rate</i> \f$\lambda_c\f$.

Since the events which form the coincidence event are taken out of the process \f$s_i\f$ where the happened, the rate of these single processes is expected to decrease, according to the coincidence weight and the coincidence rate: \f$\lambda_i\f$ will become \f$\lambda_i - \frac{w_c\lambda_c}{\sum_iw_i}\f$. Thus the sum of events \f$s_t\f$, as well as its mean \f$\mbox{E}{s_t}\f$ will not be affected by the correlations:
\f[  s_t = \sum_{i}w_is^i_t + w_cs^c_t. \f]
\f[  \mbox{E}{s_t} = \sum_i w_i\left(\lambda_i - \frac{w_c\lambda_c}{\sum_iw_i}\right) + w_c\lambda_c \f]
\f[  = \sum_i w_i\lambda_i \f]
The variance of this process \f$s_t\f$ will be:
\f[\mbox{Var}\{s_t\} = \sum_i w_i^2\left(\lambda_i - \frac{w_c\lambda_c}{\sum_iw_i}\right) + w_c^2\lambda_c.\f]

These parameters will be used to construct a white-noise (Wiener) process, which feeds the neuron: Let's assume the neuron is connected to many synapses which each have small weights. 
Lets substitute the correlation weight \f$w_c\f$ - since all weights are the same - by the amount \f$n_c\f$ of synapses taking part in a coincident pulse times their weight. We will call this amount the <i>Coincidence Width</i>.

Since the coincidence rate \f$ \lambda_c \f$ will not be higher than the overall rate \f$ \lambda \f$, and the coincidence width \f$ n_c \f$ will not be higher than the overall number of synapses, it makes sense to use a <i>relative coincidence width</i> which ranges from \f$ 0 \f$ to \f$ 1 \f$ 
\f[ n_{rc} =  \frac{n_c}{n},\f]
and a <i>relative coincidence rate</i> which ranges from \f$0\f$ to \f$1\f$ 
\f[\lambda_{rc} =  \frac{\lambda_c}{\lambda}\f]
This amounts to the following expressions for mean and variance of the white-noise process:

\f[ \mu :=  nw\lambda \f]
\f[ \sigma^2 :=  nw^2\lambda\left(1 - n_{rc} \lambda_{rc} + nn_{rc}^2\lambda_{rc} \right) \f]

The condensed poisson process implements such a white-noise process.
*/
class WienerCpp: public Wiener
{
private:
	double condW; // single weight
	double condN; // number of processes
	double condLambda; // single rate
	double condCoincWidth; // relative coincidence width
	double condCoincRate; // relative coincidence rate
public:
	/// Constructor.
	/** \param time Time object
		\param w single weight 
		\param n number of processes in the sum of processes 
		\param lambda rate of a single process
		\param n_rc relative coincidence width
		\param lambda_rc relative coincidence height */
	WienerCpp(class Time *time, double w, double n, double lambda, double n_rc, double lambda_rc);
	~WienerCpp();
	virtual string getParameter(const string& p) const;
	virtual void setParameter(const string& p, const string& d);
	virtual void init();
};

#endif
