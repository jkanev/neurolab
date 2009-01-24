set title 
set multiplot
set grid

set grid
set origin 0.0,0.5
set size 1.0,0.5
clear
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "voltage, millivolt" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/CoupledNeuronsSample.0.data" title "sample of LIF Neuron membrane" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel "voltage, millivolt" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/CoupledNeuronsSample.1.data" title "sample of LIF Neuron membrane" with lines 
unset multiplot
