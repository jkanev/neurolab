set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "stim. variance" font "Helvetica,14pt"
set ylabel "isi mean" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/ItoVsStratMean2.0.data" title "data" with lines , "data/ItoVsStratMean2.1.data" title "data" with lines 
unset multiplot
