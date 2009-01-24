set terminal png
set output "data/TestStratOne_0.01.png"
set title 
set multiplot
set grid
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, second" font ",12pt"
set ylabel "stochastic process" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratOne_0.01.0.data" title "dXt = a Xt dt + b Xt dWt (Ito)" with lines , "data/TestStratOne_0.01.1.data" title "dXt = a Xt dt + b Xt o dWt (Stratonovich)" with lines 
unset multiplot
