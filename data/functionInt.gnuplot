set title 
set multiplot
set grid
set xtics font "Helvetica,12pt"
set ytics font "Helvetica,12pt"
set ztics font "Helvetica,12pt"
set xlabel "t, " font "Helvetica,14pt"
set ylabel "[]" font "Helvetica,14pt"
set zlabel "" font "Helvetica,14pt"


plot "data/functionInt.0.data" title "0.398942 * integral {exp(-(t)^2 / 2)} d t" with lines , "data/functionInt.1.data" title "0.398942 * exp(-(t)^2 / 2)" with lines 
unset multiplot
