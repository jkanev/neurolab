set terminal png
set output "data/CoupledNeuronsRsc.png"
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
set ylabel "conductance density, microfarad" font ",12pt"
set zlabel "" font ",12pt"


plot "data/CoupledNeuronsRsc.0.data" title " conditional mean of stimulus-1 (Wiener Process)" with lines , "data/CoupledNeuronsRsc.1.data" title " 2 * exp(t) * t < 0 + 5" with lines , "data/CoupledNeuronsRsc.2.data" title " 2 * exp(0.5 * t) * t < 0 + 5" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, millisecond" font ",12pt"
set ylabel "conductance density, microfarad" font ",12pt"
set zlabel "" font ",12pt"


plot "data/CoupledNeuronsRsc.3.data" title " conditional mean of stimulus-2 (Wiener Process)" with lines , "data/CoupledNeuronsRsc.4.data" title " (2 * exp(t) * t < 0 + 5 - 2 * exp(0.5 * t) * t < 0 + 5) / 5 + 5" with lines 
unset multiplot
