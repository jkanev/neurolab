
set title 
set multiplot
set grid

set grid
set origin 0.0,0.5
set size 1.0,0.5
clear
set xtics font ",12pt"
set ytics font ",12pt"
set ztics font ",12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel ", millifarad" font "Helvetica,14pt"
set zlabel "" font ",14pt"


plot "data/CoupledNeuronsRsc.0.data" title "conditional mean of stochastic process" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "time, second" font "Helvetica,14pt"
set ylabel ", millifarad" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/CoupledNeuronsRsc.1.data" title "conditional mean of stochastic process" with lines 
unset multiplot
