set terminal png
set output "data/CoupledNeuronsSample.png"
set title 
set multiplot
set grid

set grid
set origin 0.0,0.5
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, millisecond" font ",12pt"
set ylabel "voltage, millivolt" font ",12pt"
set zlabel "" font ",12pt"


plot "data/CoupledNeuronsSample.0.data" title " sample of neuron-1 (Integrate-and-Fire Neuron)" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, millisecond" font ",12pt"
set ylabel "voltage, millivolt" font ",12pt"
set zlabel "" font ",12pt"


plot "data/CoupledNeuronsSample.1.data" title " sample of neuron-2 (Integrate-and-Fire Neuron)" with lines 
unset multiplot
