set terminal png
set output "data/TestStratThree_0.001.png"
set title 
set multiplot
set grid

set origin 0.0,0.5
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, second" font ",12pt"
set ylabel "stochastic process" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratThree_0.001.0.data" title "error Ito" with lines 
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, second" font ",12pt"
set ylabel "stochastic process" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratThree_0.001.1.data" title "error Stratonovich" with lines 
unset multiplot
