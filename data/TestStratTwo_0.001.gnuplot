set terminal png
set output "data/TestStratTwo_0.001.png"
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
set xlabel "time, second" font ",12pt"
set ylabel "" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratTwo_0.001.0.data" title " dXt = a Xt dt + b Xt dWt (Ito)" with lines , "data/TestStratTwo_0.001.1.data" title " Xt = exp((a - 1/2 b²) Wt + b Wt) (explicit, Ito)" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, second" font ",12pt"
set ylabel "" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratTwo_0.001.2.data" title " dXt = a Xt dt + b Xt o dWt (Stratonovich)" with lines , "data/TestStratTwo_0.001.3.data" title " Xt = exp(a Wt + b Wt) (explicit, Stratonovich)" with lines 
unset multiplot
