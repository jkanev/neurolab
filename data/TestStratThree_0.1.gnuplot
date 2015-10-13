set terminal png
set output "data/TestStratThree_0.1.png"
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
set xlabel "" font ",12pt"
set ylabel "" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratThree_0.1.0.data" title " relative error Ito (%)" with lines 
set grid
set origin 0.0,0.0
set size 1.0,0.5
clear
set xtics font ",10pt"
set ytics font ",10pt"
set ztics font ",10pt"
set xlabel "" font ",12pt"
set ylabel "" font ",12pt"
set zlabel "" font ",12pt"


plot "data/TestStratThree_0.1.1.data" title " relative error Stratonovich (%)" with lines 
unset multiplot
