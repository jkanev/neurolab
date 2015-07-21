/**
\mainpage
\section Introduction
NeuroLab is a class library which offers simulation of stochastic differential (integral) equations, as well as recording and display of data. Its main use is intended to be in the neuroscience field.

\section General Principles
Stochastic Variables (StochasticVariable), such as stochastic differential equations (DifferentialEquation), produce data. This data can be recorded (Matrix) by various estimators, which look for mean, variance and/or higher moments. Using a display (Display) the data can then be  displayed, compared to analytical functions (Function), and saved as data (gnuplot format), or postscript file, ready to be included in your paper.

Stochastic differential equations can be interpreted in the Ito or in the Stratonovich sense. The DifferentialEquation class lets you switch between these two versions, even at runtime. 

Data can be recorded conditionally. This means the mean (or any other moment) of a process as function of the time distance to an observed event can be taken. This is done by a ConditionalEstimator, which records the time course of a stochastic variable (StochasticVariable) before and after an observed event (Event).

\section Neural Simulations
A leaky integrate-and-fire neuron is implemented (IfNeuron), which can be calibrated automatically to give a certain response rate (IfNeuron::calibrate()). A neuron is also a SDE, so you may set up small neural networks, having a system of inter-dependent SDEs, where mutual dependencies are allowed as well. Since a neuron produces events (spikes), you can also do spike-triggered averaging.

A neuron may receive its stimulus via three different ways: 

An independent stochastic process. These are objects of the classes Poisson, Wiener, WienerCpp. They can be linked to a neuron with the IfNeuron::addStimulus() method. Each such object may drive one or many neurons.

A dependent stochastic process. A Noises object produces NoiseSource objects which resemble poisson and wiener processes, and which may be correlated. It is even possible to correlate a wiener and a poisson process.

Another neuron. In this case a synapse has to be built, which connects the neurons. Possible classes are Synapse (a simple synapse which transforms the spike events of the pre-synaptic neuron into Zeros and Ones [multiplied by a weight factor]), or SimpleSynapse (a more complex synapse, which is driven by a differential equation).


\section Quality

For a short quality check the equations \f[dX_t = aX_tdt + bX_tdW_t\f] (Ito) and \f[dX_t = aX_tdt + bX_t\circ dW_t\f] (Stratonovich) with \f$a=-8.0\f$ and \f$b=0.8\f$ are simulated using the DifferentialEquation class. This equation has an explicit solution, which was plotted on top of the simulated version at different step sizes:

<b>Step size = 0.1:</b><img src="../../data/TestStratTwo_0.1.png" width="640px">
Above: Ito and Stratonovich equations plotted on top of their explicit solutions at a time step of 0.1ms.

<b> Step-size=0.01:</b>
<img src="../../data/TestStratTwo_0.01.png" width="640px">
Above: Ito and Stratonovich equations plotted on top of their explicit solutions at a time step of 0.01ms.

<b> Step-size=0.001:</b>
<img src="../../data/TestStratTwo_0.001.png" width="640px">
Above: Ito and Stratonovich equations plotted on top of their explicit solutions at a time step of 0.001ms.

*/
