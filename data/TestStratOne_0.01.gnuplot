set terminal png
set output "data/TestStratOne_0.01.png"
set title 
set multiplot
set grid
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "time, second" font ",12pt"
set ylabel "" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratOne_0.01.0.data" title " dX_t = a X_t dt + b X_t dW_t (Itô)" with lines , "data/TestStratOne_0.01.1.data" title " dX_t = a X_t dt + b X_t o dW_t (Stratonovich)" with lines 
unset multiplot
