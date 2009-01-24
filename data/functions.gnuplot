set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "t, " font "Helvetica,14pt"
set ylabel "[]" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/functions.0.data" title "0 * sin(0 * t + 0)" with lines , "data/functions.1.data" title "1.2 * sin(0.4 * t + -0.4)" with lines , "data/functions.2.data" title "2.4 * sin(0.8 * t + -0.8)" with lines , "data/functions.3.data" title "3.6 * sin(1.2 * t + -1.2)" with lines , "data/functions.4.data" title "4.8 * sin(1.6 * t + -1.6)" with lines 
unset grid
set origin 0.0,0.5
set size 0.5,0.5
clear
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "t, " font "Helvetica,14pt"
set ylabel "[]" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/functions.5.data" title "sin(t)" with lines 
unset multiplot
