
#include "../h/stochastic.hxx"
#include "../h/timedependent.hxx"
#include "../h/estimator.hxx"


//__________________________________________________________________________
// create

Estimator::~Estimator()
{
	estimatorTime->remove( this );
};


//__________________________________________________________________________
// destroy

Estimator::Estimator(StochasticProcess *src, Time *time, const string& name, const string& type) : Parametric(name, type)
{
	pSource = src;
	estimatorTime = time;
	estimatorTime->add( this );
};

