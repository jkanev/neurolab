#include "../h/spikeestimator.hxx"
#include "../h/graph.hxx"

//________________________________________________________________________________
// construct

SpikeEstimator::SpikeEstimator (const Property &property, StochasticEventGenerator *A, StochasticEventGenerator *B, Time *time, uint length, const string& name, const string& type)
: Estimator(nullptr, time, name, type)
{
    // store the parameters
    spkA = A;
    spkB = B;
	nEstimate = property;
    spkLength = length;
    pSource = nullptr;     // we're not using the standard estimator source
	
	// create arrays
	if (nEstimate & EST_SAMPLE)
        spkSample = new double[spkLength];
	else
        spkSample = nullptr;

    // initialise all arrays
	init();
}

SpikeEstimator::~SpikeEstimator()
{
    if (spkSample)
        delete[] spkSample;
}

//_________________________________________
// initialise
void SpikeEstimator::init()
{
    spkALast = 0;
    spkBLast = 0;
    spkDiffAB = 0;
    spkDiffBA = 0;
    initA = false;
    initB = false;

	nSamples = 0;
    if(spkSample)
        for(uint i=0; i<spkLength; i++)
            spkSample[i] = 0.0;
    spkOne = 0.0l;
};

//________________________________________________________________________________
// eat next data point

void SpikeEstimator::collect()
{
    // spkALast stores the time passed since the latest event from A (spkBLast does the same for B).
    // spkDiffAB stores the time difference between spkALast and its closest event in B (spkDiffBA B-A).
    //
    // If a new spike in A comes in, spkALast is set to 0.
    // 1. the difference spkDiffBA is udpated: If spkBLast-spkALast is smaller, it is replaced.
    // 2. the difference spkDiffAB is calculated new (can be bigger).
    //
    // in any case:
    // 3. the current value (spkDiffBA + spkDiffAB) / (spkALast + spkBLast) / 8.0 is calculated.

    // advance time count for all values
    ++spkALast;
    ++spkBLast;

    bool eventA = spkA->getEventAmount();
    bool eventB = spkB->getEventAmount();

    // if event occured, set value of last event to zero
    if (eventA) spkALast = 0;
    if (eventB) spkBLast = 0;

    // if A and not B, update B-A and renew A-B. Note: spkALast is zero and can be ommitted in all sums and differences.
    if (eventA && !eventB) {
        if (spkDiffBA > spkBLast || !initA)
            spkDiffBA = spkBLast;
        spkDiffAB = spkBLast;
        spkCurrentValue = double(spkDiffBA + spkDiffAB) / double(spkBLast) / 2.0;
    }

    // if B and not A, update A-B and renew B-A. Note: spkBLast is zero and can be ommitted in all sums and differences.
    else if (!eventA && eventB) {
        if (spkDiffAB > spkALast || !initB)
            spkDiffAB = spkALast;
        spkDiffBA = spkALast;
        spkCurrentValue = double(spkDiffBA + spkDiffAB) / double(spkALast) / 2.0;
    }

    // if both B and A, set everything to zero
    else if (eventA && eventB) {
        spkDiffAB = 0;
        spkDiffBA = 0;
        spkCurrentValue = 0.0l;
    }

    // if neither, just calculate the difference
    else {
        spkCurrentValue = double(spkDiffBA + spkDiffAB) / double(spkALast + spkBLast) / 2.0;
    }

    if (!initA && eventA) initA = true;
    if (!initB && eventB) initB = true;

    // collect moments
    processCurrentSample();
}

//________________________________________________________________________________
// add new samples to moment estimates

void SpikeEstimator::processCurrentSample()
{
    if (nEstimate & EST_SAMPLE)
        if (nSamples < spkLength)
            spkSample[nSamples] = spkCurrentValue;
    if (nEstimate & EST_MEAN)
        spkOne += spkCurrentValue;
    ++nSamples;
}

Matrix SpikeEstimator::getSample()
{
    Graph a{int(spkLength)};
    a.setName("O-SPIKE measure " + spkA->getName() + " - " + spkB->getName());
    a.setPhysical(0, *estimatorTime );

    for(uint i=0; i<spkLength; i++) {
        a[int(i)][0] = double(i) * estimatorTime->dt;
        a[int(i)][1] = spkSample[i];
    }

    return a;
}

double SpikeEstimator::getMean()
{
    if (nSamples > 0.0)
        // If we have collected samples, we return the standard distance.
        return double(spkOne / long(nSamples));
    else
        // At least one neuron has not been spiking - it doesn't get any worse than that,
        // we return the maximal distance.
        return 1.0;
}

Matrix SpikeEstimator::getEstimate(const Property& what)
{
    if (what==EST_SAMPLE)
        return getSample();
    else if (what==EST_MEAN) {
        Matrix a{1};
        a.setName("1/T * integral( O-SPIKE " + spkA->getName() + " - " + spkB->getName() + ") dt");
        a[0] = getMean();
        return a;
    }
    else {
        Matrix a{0};
        a.setName("Property not implemented");
        return a;
    }
}
