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
set xlabel "stim. variance,  (square) millisiemens" font "Helvetica,14pt"
set ylabel "isi mean" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratMean.0.data" title " response mean of neuron using Ito" with lines , "data/ItoVsStratMean.1.data" title " response mean of neuron using Stratonovich" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "cond. variance,  (square) millisiemens" font "Helvetica,14pt"
set ylabel "isi variance" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"

set logscale y

plot "data/ItoVsStratMean.2.data" title " response variance of neuron using Ito" with lines , "data/ItoVsStratMean.3.data" title " response variance of neuron using Stratonovich" with lines 
unset multiplot
