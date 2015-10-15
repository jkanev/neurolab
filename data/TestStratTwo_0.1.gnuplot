set terminal png
set output "data/TestStratTwo_0.1.png"
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


plot "data/TestStratTwo_0.1.0.data" title " dX_t = a X_t dt + b X_t dW_t (Itô)" with lines , "data/TestStratTwo_0.1.1.data" title " X_t = exp((a - ½ b²) t + b W_t) (explicit, Itô)" with lines 
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


plot "data/TestStratTwo_0.1.2.data" title " dX_t = a X_t dt + b X_t o dW_t (Stratonovich)" with lines , "data/TestStratTwo_0.1.3.data" title " X_t = exp(a t + b W_t) (explicit, Stratonovich)" with lines 
unset multiplot
